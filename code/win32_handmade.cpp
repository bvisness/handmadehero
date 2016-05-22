#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

// TODO: This is a global for now
global_variable bool running;

global_variable BITMAPINFO bitmapInfo;
global_variable void* bitmapMemory;
global_variable HBITMAP bitmapHandle;
global_variable HDC bitmapDeviceContext;

internal void
Win32ResizeDIBSection(int width, int height) {
    // TODO: bulletproof this
    // Maybe don't free first, fre after, then free first if that fails

    if (bitmapHandle) {
        DeleteObject(bitmapHandle);
    } 

    if (!bitmapDeviceContext) {
        // TODO: should we recreate these under certain special circumstances?
        bitmapDeviceContext = CreateCompatibleDC(0);
    }

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    bitmapHandle = CreateDIBSection(
        bitmapDeviceContext,
        &bitmapInfo,
        DIB_RGB_COLORS,
        &bitmapMemory,
        NULL,
        NULL
    );

    ReleaseDC(0, bitmapDeviceContext);
}

internal void
Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height) {
    StretchDIBits(
        deviceContext,
        x, y, width, height,
        x, y, width, height,
        bitmapMemory,
        &bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK
Win32MainWindowCallback(
    HWND   window,
    UINT   message,
    WPARAM wParam,
    LPARAM lParam
) {
    LRESULT result = 0;

    switch (message) {
        case WM_SIZE: {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            Win32ResizeDIBSection(width, height);
        } break;

        case WM_CLOSE: {
            // TODO: Handle this with a message to the user?
			running = false;
        } break;

        case WM_DESTROY: {
            // TODO: Handle this as an error - recreate window?
            running = false;
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
            Win32UpdateWindow(deviceContext, x, y, width, height);
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
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
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
            running = true;
            while (running) {
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
