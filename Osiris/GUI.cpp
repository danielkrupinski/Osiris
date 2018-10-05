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
        static bool isDown{ false };
        static bool isClicked{ false };
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            isClicked = false;
            isDown = true;
        }
        else if (!GetAsyncKeyState(VK_INSERT) && isDown) {
            isClicked = true;
            isDown = false;
        }
        else {
            isClicked = false;
            isDown = false;
        }
        if (isClicked) {
            isOpen = !isOpen;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
