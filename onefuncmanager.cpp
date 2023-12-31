#include <Windows.h>
#include <fstream>
#include <sstream>
#include <vector>

const UINT MY_HOTKEY_ID = 1;
// Update the custom message value
const UINT TRAY_ICON_MESSAGE = WM_USER + 2;
RECT savedRect = {0}; // Declare savedRect globally

// Structure to hold hotkey configuration
struct HotkeyConfig {
    std::vector<int> modifiers;
    int key{};
};

// Function to create and show the system tray icon
void CreateSystemTrayIcon(HWND hwnd);

// Function to remove the system tray icon
void RemoveSystemTrayIcon(HWND hwnd);

// Function to read hotkey configuration from a file
HotkeyConfig ReadHotkeyConfig(const std::wstring& filename) {
    HotkeyConfig config; // Default values

    std::wifstream configFile(filename.c_str());
    if (configFile.is_open()) {
        std::wstring line;
        while (std::getline(configFile, line)) {
            std::wistringstream iss(line);
            std::wstring key, value;
            if (std::getline(iss, key, L'=') && std::getline(iss, value, L';')) {
                if (key == L"MODIFIERS") {
                    std::wistringstream modifiersStream(value);
                    std::wstring modifier;
                    while (std::getline(modifiersStream, modifier, L'|')) {
                        config.modifiers.push_back(std::stoi(modifier));
                    }
                } else if (key == L"KEY") {
                    config.key = std::stoi(value);
                }
            }
        }
        configFile.close();
    }

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

    case TRAY_ICON_MESSAGE:
        // Handle tray icon events here
        if (lParam == WM_RBUTTONDOWN) {
            HMENU hPopupMenu = CreatePopupMenu();
            AppendMenuW(hPopupMenu, MF_STRING, 1, L"Exit");

            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, nullptr);
            SendMessage(hwnd, WM_NULL, 0, 0);
            DestroyMenu(hPopupMenu);
        }
            break;

    case WM_COMMAND:
        // Handle menu item selection
        switch (LOWORD(wParam)) {
            case 1:
                // Handle "Exit" option
                // Add your exit logic here
                PostMessage(hwnd, WM_CLOSE, 0, 0);
                break;
        }
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

// Function to create and show the system tray icon
void CreateSystemTrayIcon(HWND hwnd) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hwnd;
    nid.uID = 1; // Unique ID for the icon
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = TRAY_ICON_MESSAGE; // Updated custom message for tray icon events

    // Load the icon using LoadIconW directly with IDI_APPLICATION
    nid.hIcon = LoadIconW(nullptr, reinterpret_cast<LPCWSTR>(IDI_APPLICATION));

    if (nid.hIcon != nullptr) {
        lstrcpyW(nid.szTip, L"onefuncmanager"); // Tooltip for the icon
        Shell_NotifyIconW(NIM_ADD, &nid);
    }
}


// Function to remove the system tray icon
void RemoveSystemTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}

int main() {
    // Read hotkey configuration from the file
    HotkeyConfig hotkeyConfig = ReadHotkeyConfig(L"config.txt");

    // Register the window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"onefuncmanager";
    RegisterClassW(&wc);

    // Create the window
    HWND hwnd = CreateWindowExW(0, L"onefuncmanager", L"onefuncmanager", 0, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

    // Create and show the system tray icon
    CreateSystemTrayIcon(hwnd);

    // Register the hotkey with the configuration values
    int combinedModifiers = 0;
    for (int modifier : hotkeyConfig.modifiers) {
        combinedModifiers |= modifier;
    }
    // std::printf("combined modifiers %d\n", combinedModifiers);
    RegisterHotKey(hwnd, MY_HOTKEY_ID, combinedModifiers, hotkeyConfig.key);

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unregister the hotkey
    UnregisterHotKey(hwnd, MY_HOTKEY_ID);

    return 0;
}