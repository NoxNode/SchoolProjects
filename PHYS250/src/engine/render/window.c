#include "window.h"
#include <stdio.h>
#include "Windows.h"

// LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     LRESULT lResult = 0;
//     switch(message)
//     {
//         case WM_SIZE: {
//
//             break;
//         }
//         case WM_CLOSE: {
//             PostQuitMessage(0);
//             break;
//         }
//         case WM_DESTROY: {
//             PostQuitMessage(0);
//         }
//         case WM_LBUTTONDOWN: {
//             std::cout << "\nmouse left button down at (" << LOWORD(lParam) << ',' << HIWORD(lParam) << ")\n";
//             break;
//         }
//         default: {
//             lResult = DefWindowProc(windowHandle, message, wParam, lParam);
//             break;
//         }
//     }
//     return lResult;
// }
//
// void initWindowsWindow(Window* window, const char* title, int width, int height) {
//     const char* windowClassName = "noxEngineWindowClass";
//     WNDCLASSEX windowClass = {
//         sizeof(WNDCLASSEX),
//         CS_DBLCLKS,
//         WindowProcedure,
//         0,
//         0,
//         GetModuleHandle(0),
//         LoadIcon(0, IDI_APPLICATION),
//         LoadCursor(0, IDC_ARROW),
//         HBRUSH(COLOR_WINDOW+1),
//         0,
//         windowClassName,
//         LoadIcon(0, IDI_APPLICATION)
//     };
//
//     if(RegisterClassEx(&windowClass))
//     {
//         HWND windowHandle = CreateWindowEx(0,
//                                             windowClass.lpszClassName,
//                                             title,
//                                             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
//                                             CW_USEDEFAULT,
//                                             CW_USEDEFAULT,
//                                             CW_USEDEFAULT,
//                                             CW_USEDEFAULT,
//                                             0,
//                                             0,
//                                             GetModuleHandle(0),
//                                             0);
//
//         if(windowHandle)
//         {
//             MSG message;
//             bool errorMessage;
//             while((errorMessage = GetMessage(&message, 0, 0, 0)) != 0) {
//                 TranslateMessage(&message);
//                 DispatchMessage(&message);
//                 if(errorMessage == -1) {
//                     PostQuitMessage(0);
//                 }
//             }
//         }
//     }
// }
//
// void Window_init(Window* window, const char* title, int width, int height) {
//     initWindowsWindow(window, title, width, height);
// }
//
// void Window_deinit(Window* window) {
//
// }

void Window_init(Window* window, const char* title, int width, int height) {
    if(!SDL_WasInit(SDL_INIT_VIDEO)) {
        SDL_Init(SDL_INIT_VIDEO);
    }

    window->sdl_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if(window->sdl_window == NULL) {
        // TODO: handle error better
        printf("%s\n", "failed to create window");
    }

    // TODO: pass pixel format and stuff
    SDL_Surface* sdl_surface = SDL_GetWindowSurface(window->sdl_window);
    window->surface = (Surface*) malloc(sizeof(Surface));
    Surface_init(window->surface, sdl_surface, (bool) SDL_MUSTLOCK(sdl_surface));
}

void Window_deinit(Window* window) {
    if(window->sdl_window != NULL) {
        SDL_DestroyWindow(window->sdl_window);
    }
    if(window->surface != NULL) {
        free(window->surface);
    }
    // TODO: make sure I want to quit here and not somewhere else
    SDL_Quit();
}

void Window_updateSurface(Window* window) {
    SDL_UpdateWindowSurface(window->sdl_window);
}
