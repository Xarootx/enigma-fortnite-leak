#pragma once

// Platform-specific includes
#ifdef _KERNEL_MODE
#include <ntddk.h>
#include <ntdef.h>
#include <xtr1common>
#else
#include <Windows.h>
#include <utility>
#endif
#include <Intrin.h>

/*
 * Function call spoofing implementation
 * Supports both kernel mode and user mode (x64)
 * Requirements:
 * - Kernel mode: Disable Control Flow Guard (CFG) /guard:cf, C++14+
 * - User mode: C++17+
 */

// Helper macros for call spoofing
#define SPOOF CallSpoofer::SpoofFunction spoof(_AddressOfReturnAddress());

#ifdef _KERNEL_MODE
#define SPOOF_CALL(ret_type,name) (CallSpoofer::SafeCall<ret_type,std::remove_reference_t<decltype(*name)>>(name))
#else 
#define SPOOF_CALL(name) (CallSpoofer::SafeCall(name))
#endif

// Configuration constants
#define MAX_FUNC_BUFFERED 100
#define SHELLCODE_GENERATOR_SIZE 500

namespace CallSpoofer {

// Platform-specific type definitions and utilities
#ifdef _KERNEL_MODE
typedef unsigned __int64 uintptr_t, size_t;

// Minimal std::forward implementation for kernel mode
namespace detail {
    template<class T>
    struct remove_reference {
        using type = T;
        using _Const_thru_ref_type = const T;
    };

    template<class T>
    using remove_reference_t = typename remove_reference<T>::type;

    template<class>
    constexpr bool is_lvalue_reference_v = false;

    template<class T>
    constexpr bool is_lvalue_reference_v<T&> = true;

    template<class T>
    constexpr T&& forward(remove_reference_t<T>& arg) noexcept {
        return static_cast<T&&>(arg);
    }

    template<class T>
    constexpr T&& forward(remove_reference_t<T>&& arg) noexcept {
        static_assert(!is_lvalue_reference_v<T>, "bad forward call");
        return static_cast<T&&>(arg);
    }
}

using namespace detail;
#else
using namespace std;
#endif

// Core spoof function implementation
class SpoofFunction {
private:
    uintptr_t temp = 0;
    static constexpr uintptr_t xor_key = 0xff00ff00ff00ff00;
    void* ret_addr_in_stack = nullptr;

public:
    explicit SpoofFunction(void* addr) : ret_addr_in_stack(addr) {
        temp = *(uintptr_t*)ret_addr_in_stack;
        temp ^= xor_key;
        *(uintptr_t*)ret_addr_in_stack = 0;
    }

    ~SpoofFunction() {
        temp ^= xor_key;
        *(uintptr_t*)ret_addr_in_stack = temp;
    }
};

// Platform-specific shellcode allocation
#ifdef _KERNEL_MODE
__forceinline PVOID LocateShellCode(PVOID func, size_t size = 500) {
    void* addr = ExAllocatePoolWithTag(NonPagedPool, size, (ULONG)"File");
    return addr ? memcpy(addr, func, size) : nullptr;
}
#else
__forceinline PVOID LocateShellCode(PVOID func, size_t size = SHELLCODE_GENERATOR_SIZE) {
    void* addr = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    return addr ? memcpy(addr, func, size) : nullptr;
}
#endif

// Shellcode generator implementation
#ifdef _KERNEL_MODE
template<typename RetType, typename Func, typename... Args>
RetType
#else
template<typename Func, typename... Args>
typename std::invoke_result<Func, Args...>::type
#endif
__declspec(safebuffers) ShellCodeGenerator(Func f, Args&... args) {
    // Type definitions based on platform
#ifdef _KERNEL_MODE
    using this_func_type = decltype(ShellCodeGenerator<RetType, Func, Args&...>);
    using return_type = RetType;
#else
    using this_func_type = decltype(ShellCodeGenerator<Func, Args&...>);
    using return_type = typename std::invoke_result<Func, Args...>::type;
#endif

    // Return address manipulation
    static constexpr uintptr_t xor_key = 0xff00ff00ff00ff00;
    void* ret_addr_in_stack = _AddressOfReturnAddress();
    uintptr_t temp = *(uintptr_t*)ret_addr_in_stack;
    temp ^= xor_key;
    *(uintptr_t*)ret_addr_in_stack = 0;

    // Handle void and non-void returns
    if constexpr (std::is_same<return_type, void>::value) {
        f(args...);
        temp ^= xor_key;
        *(uintptr_t*)ret_addr_in_stack = temp;
    }
    else {
        return_type&& ret = f(args...);
        temp ^= xor_key;
        *(uintptr_t*)ret_addr_in_stack = temp;
        return ret;
    }
}

// Safe call wrapper class
#ifdef _KERNEL_MODE
template<typename RetType, class Func>
#else
template<class Func>
#endif
class SafeCall {
private:
    Func* funcPtr;

public:
    explicit SafeCall(Func* func) : funcPtr(func) {}

    template<typename... Args>
    __forceinline decltype(auto) operator()(Args&&... args) {
        SPOOF;

        // Platform-specific type definitions
#ifdef _KERNEL_MODE
        using return_type = RetType;
        using p_shell_code_generator_type = decltype(&ShellCodeGenerator<RetType, Func*, Args...>);
        PVOID self_addr = static_cast<PVOID>(&ShellCodeGenerator<RetType, Func*, Args&&...>);
#else
        using return_type = typename std::invoke_result<Func, Args...>::type;
        using p_shell_code_generator_type = decltype(&ShellCodeGenerator<Func*, Args...>);
        p_shell_code_generator_type self_addr = static_cast<p_shell_code_generator_type>(&ShellCodeGenerator<Func*, Args&&...>);
#endif

        // Shellcode caching
        static size_t count{};
        static p_shell_code_generator_type orig_generator[MAX_FUNC_BUFFERED]{};
        static p_shell_code_generator_type alloc_generator[MAX_FUNC_BUFFERED]{};
        p_shell_code_generator_type p_shellcode{};

        // Find or create shellcode
        for(unsigned i = 0; i < MAX_FUNC_BUFFERED && orig_generator[i]; i++) {
            if(orig_generator[i] == self_addr) {
                p_shellcode = alloc_generator[i];
                break;
            }
        }

        if(!p_shellcode && count < MAX_FUNC_BUFFERED) {
            p_shellcode = reinterpret_cast<p_shell_code_generator_type>(LocateShellCode(self_addr));
            if(p_shellcode) {
                orig_generator[count] = self_addr;
                alloc_generator[count] = p_shellcode;
                count++;
            }
        }

        return p_shellcode ? p_shellcode(funcPtr, args...) : return_type{};
    }
};

} // namespace CallSpoofer
