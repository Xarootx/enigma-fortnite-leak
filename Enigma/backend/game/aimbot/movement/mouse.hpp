// Helper namespace for mouse input and control
namespace SonyDriverHelper {
    // API class for mouse-related functionality
    class api {
    public:
        // Initialize mouse driver and settings
        static void Init();

        // Move mouse cursor by relative x,y coordinates
        // @param x: Horizontal movement in pixels 
        // @param y: Vertical movement in pixels
        static void MouseMove(float x, float y);

        // Check if a key is currently pressed
        // @param id: Virtual key code to check
        // @return: True if key is pressed, false otherwise
        static bool GetKey(int id);
    };
}

/* Removed deprecated mouse namespace in favor of SonyDriverHelper::api
   The old implementation had redundant functionality and less flexibility.
   New implementation provides better encapsulation and cleaner interface. */
