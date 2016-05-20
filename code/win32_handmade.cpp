#include <windows.h>

LRESULT CALLBACK
MainWindowCallback(
    HWND   window,
    UINT   message,
    WPARAM wParam,
    LPARAM lParam
) {
    LRESULT result = 0;

    switch (message) {
        case WM_SIZE: {
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY: {
            OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_CLOSE: {
            OutputDebugStringA("WM_CLOSE\n");
        } break;

        case WM_ACTIVATEAPP: {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            PatBlt(deviceContext, x, y, width, height, WHITENESS);
            EndPaint(window, &paint);
        } break;

        default: {
            // OutputDebugStringA("default\n");
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }

    return result;
}

int CALLBACK
WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR     commandLine,
    int       showCode
) {
    WNDCLASS WindowClass = {};

    // TODO: Are hredraw and vredraw important?
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = instance;
    // WindowClass.hIcon = ;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

    if (RegisterClass(&WindowClass)) {
        HWND windowHandle = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            "Handmade Hero",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            instance,
            0
        );

        if (windowHandle) {
            while (true) {
                MSG message;
                BOOL messageResult = GetMessage(&message, windowHandle, 0, 0);
                if (messageResult > 0) {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                } else {
                    break;
                }
            }
        } else {
            // TODO: logging
        }
    } else {
        // TODO: Logging of some sort
    }

    return 0;
}
