#include "basic_window.hpp"

UINT testing::basic_window::width()
{
    RECT rc = {};
    if (GetClientRect(m_window_handle, &rc) == FALSE) {
#if ENABLE_ALL_EXCEPTIONS
        win32_code_objs::code_obj error_obj(win32_code_objs::get_client_rect_fail);
        throw errors::get_client_rect_failed(error_obj);
#endif
        return 0;
    }

    return rc.right - rc.left;
}

UINT testing::basic_window::height()
{
    RECT rc = {};
    if (GetClientRect(m_window_handle, &rc) == FALSE) {
#if ENABLE_ALL_EXCEPTIONS
        win32_code_objs::code_obj code(win32_code_objs::get_client_rect_fail);
        throw errors::get_client_rect_failed(code);
#endif
        return 0;
    }

    return rc.bottom - rc.top;
}

LRESULT testing::basic_window::ThisWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {

    case WM_PAINT: 
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        



        EndPaint(hwnd, &ps);
        break;
    } 

    default:
        return window::starter::ThisWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
