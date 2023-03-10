#pragma once

#include <Windows.h>

class WindowProcedureHook {
public:
    explicit WindowProcedureHook(HWND window)
        : window{ window },
          originalWndProc{ WNDPROC(SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(&wndProc))) }
    {
    }

    static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    void uninstall()
    {
        SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(originalWndProc));
    }

private:
    HWND window;
    WNDPROC originalWndProc;
};
