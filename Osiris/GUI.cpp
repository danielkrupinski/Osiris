#include <chrono>
#include <thread>

#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "GUI.h"

GUI::GUI()
{
    
}

void GUI::waitForToggle()
{
    while (true) {
        if (GetAsyncKeyState(VK_INSERT) & 1)
            isOpen = !isOpen;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
