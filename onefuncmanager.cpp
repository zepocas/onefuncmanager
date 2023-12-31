#include <Windows.h>
#include <fstream>
#include <sstream>
#include <unordered_map>

const UINT MY_HOTKEY_ID = 1;
RECT savedRect = {0}; // Declare savedRect globally

// Structure to hold hotkey configuration
struct HotkeyConfig {
    int modifiers;
    int key;
};

// Function to read hotkey configuration from a file
HotkeyConfig ReadHotkeyConfig(const std::string& filename) {
    HotkeyConfig config = {0, 0}; // Default values

    std::ifstream configFile(filename);
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value, ';')) {
                if (key == "MODIFIERS") {
                    // Split the value at "|" and convert each part separately
                    std::istringstream modifiersStream(value);
                    std::string modifier;
                    while (std::getline(modifiersStream, modifier, '|')) {
                        config.modifiers |= std::stoi(modifier);
                    }
                } else if (key == "KEY") {
                    config.key = std::stoi(value);
                }
            }
        }
        configFile.close();
    }
    std::printf("%d - %d",config.key, config.modifiers);
    return config;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_HOTKEY:
        if (wParam == MY_HOTKEY_ID)
        {
            // Get the foreground window
            HWND foregroundWindow = GetForegroundWindow();

            // Get the current window position
            RECT windowRect;
            GetWindowRect(foregroundWindow, &windowRect);

            // Calculate the center position
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            int windowWidth = windowRect.right - windowRect.left;
            int windowHeight = windowRect.bottom - windowRect.top;

            int centerX = (screenWidth - windowWidth) / 2;
            int centerY = (screenHeight - windowHeight) / 2;

            // Toggle window position between center and original
            if (windowRect.left == centerX && windowRect.top == centerY)
            {
                // Bring the window back to its original position
                SetWindowPos(foregroundWindow, HWND_TOP, savedRect.left, savedRect.top, 0, 0, SWP_NOSIZE);
            }
            else
            {
                // Save the current window position
                savedRect = windowRect;

                // Bring the window to the center
                SetWindowPos(foregroundWindow, HWND_TOP, centerX, centerY, 0, 0, SWP_NOSIZE);
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

// Change these constants according to the new hotkey
const UINT NEW_HOTKEY_ID = 1;
const int NEW_HOTKEY_MODIFIERS = MOD_CONTROL | MOD_ALT;
const int NEW_HOTKEY_VK = 'F'; // Virtual key code for the 'F' key

int main() {
    // Read hotkey configuration from the file
    HotkeyConfig hotkeyConfig = ReadHotkeyConfig("config.txt");
    

    // Register the window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"SimpleWindowManager";
    RegisterClassW(&wc);

    // Create the window
    HWND hwnd = CreateWindowExW(0, L"SimpleWindowManager", L"Simple Window Manager", 0, 0, 0, 0, 0, 0, 0, wc.hInstance, 0);

    // Register the hotkey with the configuration values
    RegisterHotKey(hwnd, MY_HOTKEY_ID, hotkeyConfig.modifiers, hotkeyConfig.key);

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, 0, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unregister the hotkey
    UnregisterHotKey(hwnd, MY_HOTKEY_ID);

    return 0;
}
