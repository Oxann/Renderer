#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DXGI.lib")

#include "MainWindow.h"
#include "Graphics/Graphics.h"
#include "Renderer/RendererManager.h"
#include "Input.h"
#include "Time.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    MainWindow::Init(L"Game", 1024, 576);
    Graphics::Init();
    RendererManager rm(MainWindow::GetHWND(), 1024, 576);

    while (true)
    {
        auto FrameBegin = std::chrono::steady_clock::now();
        Input::FlushKeys();
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return static_cast<int>(msg.wParam);
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        rm.Render();

        //Time 
        auto FrameEnd = std::chrono::steady_clock::now();
        Time::DeltaTime = std::chrono::duration<float>(FrameEnd - FrameBegin).count();
        Time::AppTime += Time::DeltaTime;
    }

    return 0;
}