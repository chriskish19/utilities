#include "log_window_class.hpp"

window::log_window::log_window() {
    m_line_height = m_font_size + 2;

    scrolling* scroll_p = new scrolling(m_logs.get_logs_p()->load(), m_line_height);
#if ENABLE_ALL_EXCEPTIONS
    if (scroll_p == nullptr) {
        code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
        throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("scrolling* scroll_p"));
    }
#endif
    
    m_scroll_p = scroll_p;
}

window::log_window::~log_window()
{
    if (m_scroll_p != nullptr) {
        delete m_scroll_p;
    }

    DeleteObject(m_hFont);
}

errors::codes window::log_window::go()
{
    set_font(m_font_size);
    window_settings();
    create_window();
    remove_x_log_window();
    SetWindowText(m_window_handle, READ_ONLY_STRING("log window"));
    m_scroll_p->set_scroll_info(m_window_handle);
    message_pump();
    return errors::codes::success;
}

std::atomic<utilities::logger::logs*>* window::log_window::get_logs_p()
{
    auto p = m_logs.get_logs_p();
#if ENABLE_ALL_EXCEPTIONS
    if (p == nullptr) {
        code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
        throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("utilities::logger::logs* p"));
    }
#endif
    return p;
}

errors::win32_codes window::log_window::update() {
#if ENABLE_FULL_DEBUG
    if (InvalidateRect(m_window_handle, nullptr, TRUE) == FALSE) {
#if ENABLE_ALL_EXCEPTIONS
        win32_code_objs::code_obj error(win32_code_objs::invalidate_rect_fail);
        throw errors::invalidate_rect_failed(error);
#endif // ENABLE_ALL_EXCEPTIONS
        return errors::win32_codes::invalidate_rect_fail;
    }
    return errors::win32_codes::success;
#endif // ENABLE_FULL_DEBUG

#if ENABLE_FULL_OPTIMIZATIONS
    InvalidateRect(m_window_handle, nullptr, TRUE);
    return errors::codes::success;
#endif
}

errors::codes window::log_window::remove_x_log_window()
{
    // Remove the Close button
    HMENU hMenu = GetSystemMenu(m_window_handle, FALSE);
    if (hMenu != nullptr) {
        RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
    }

    DrawMenuBar(m_window_handle); // Update the window's menu bar

    return errors::codes::success;
}

errors::codes window::log_window::add_x_log_window()
{
    HMENU hMenu = GetSystemMenu(m_window_handle, FALSE);
    if (hMenu != nullptr) {
        // Re-add the Close button
        AppendMenu(hMenu, MF_BYCOMMAND, SC_CLOSE, L"Close");
        DrawMenuBar(m_window_handle); // Update the menu bar
    }
    
    return errors::codes::success;
}

void window::log_window::create_window()
{
    m_window_handle = CreateWindowEx(
        0,                                      // Optional window styles.
        m_c_name.c_str(),                       // Window class
        m_title.c_str(),                        // Window text
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,       // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,                                           // Parent window    
        NULL,                                           // Load the menu here
        m_hinst,                                        // Instance handle
        this                                            // Additional application data
    );

    ShowWindow(m_window_handle, SW_SHOW);
}

LRESULT window::log_window::ThisWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:
        GetClientRect(hwnd, &m_client_window_size);
        break;
    case WM_SIZE:
        window_size_changed(hwnd);
        break;
    case WM_PAINT:
        draw_log_window();
        break;
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
            break;
        } // end of switch (LOWORD(wParam))
        break;
        }
    case WM_VSCROLL:
        m_scroll_p->handle_scroll(hwnd, wParam,0);
        break;
    case WM_MOUSEWHEEL:
        m_scroll_p->handle_scroll(hwnd, wParam, GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    } // end of switch (uMsg)
    // no default switches needed
    return starter::ThisWindowProc(hwnd, uMsg, wParam, lParam);
}

void window::log_window::message_pump()
{
    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void window::log_window::draw_log_window()
{   
    if (GetClientRect(m_window_handle, &m_client_window_size) == FALSE) {
        errors::handle_win32_error_codes(errors::win32_codes::get_client_rect_fail);
    }
    
    PAINTSTRUCT ps = {};
    ps.rcPaint = m_client_window_size;

    HDC hdc = BeginPaint(m_window_handle, &ps);

    SelectObject(hdc, m_hFont);

    // Set text color and background
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkColor(hdc, RGB(255, 255, 255));  // White background
    
    utilities::logger::logs* logs_p = m_logs.get_logs_p()->load();
    
    // Fill the background to avoid artifacts
    FillRect(hdc, &m_client_window_size, (HBRUSH)(COLOR_WINDOW + 1));

    size_t window_width = m_client_window_size.right - m_client_window_size.left;
    size_t window_height = m_client_window_size.bottom - m_client_window_size.top;

    
#if ENABLE_ALL_EXCEPTIONS
    if (m_line_height == 0) {
        code_error_objs::code_obj error(code_error_objs::division_by_zero);
        throw errors::division_by_zero(error);
    }
#endif
    size_t render_lines = window_height / m_line_height;
    size_t start = std::abs(m_scroll_p->get_scroll_position());
    size_t loop_count = start + render_lines;

    size_t yOffset = 0;
    for (size_t i = start; i < logs_p->get_vec_log_size() && i < loop_count; ++i) {
        m_scroll_p->set_line_rect(m_client_window_size, yOffset);
        DrawText(hdc, logs_p->get_log_by_index(i).c_str(), -1, &m_scroll_p->m_line_rect, DT_LEFT | DT_TOP );
        yOffset += m_line_height;
    }

    EndPaint(m_window_handle, &ps);
}

void window::log_window::set_font(LONG size)
{
    // Create a font
    m_hFont = CreateFont(
        size,                       // Height of font
        0,                          // Width of font (0 for default)
        0,                          // Escapement angle
        0,                          // Orientation angle
        FW_BOLD,                    // Font weight (FW_NORMAL, FW_BOLD, etc.)
        FALSE,                      // Italic
        FALSE,                      // Underline
        FALSE,                      // Strikeout
        DEFAULT_CHARSET,            // Character set
        OUT_DEFAULT_PRECIS,         // Output precision
        CLIP_DEFAULT_PRECIS,        // Clipping precision
        DEFAULT_QUALITY,            // Output quality
        DEFAULT_PITCH | FF_SWISS,   // Family and pitch
        L"Arial"                    // Typeface name
    );

#if ENABLE_FULL_DEBUG
    if (m_hFont == NULL) {
        win32_code_objs::code_obj error(win32_code_objs::font_error);
        throw errors::font_error(error);
    }
#endif
}

void window::log_window::window_size_changed(HWND hwnd)
{
    InvalidateRect(hwnd, nullptr, TRUE); // Mark the entire client area for redrawing
}

window::log_window::scrolling::scrolling(utilities::logger::logs* log_p, LONG line_height)
    :m_line_height(line_height)
{
#if ENABLE_FULL_DEBUG
    if (log_p == nullptr) {
        code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
        throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("utilities::logger::logs* log_p"));
    }
#endif
    m_number_of_log_lines = log_p->get_vec_log_size();
}

void window::log_window::scrolling::set_scroll_info(HWND hwnd)
{
    m_si.cbSize = sizeof(SCROLLINFO);
    m_si.fMask = SIF_ALL;
    m_si.nMin = 0;
    m_si.nMax = m_number_of_log_lines;
    m_si.nPage = 25;
    SetScrollInfo(hwnd, SB_VERT, &m_si, TRUE);
}

void window::log_window::scrolling::handle_scroll(HWND hwnd, WPARAM wParam,int wheel_delta)
{
    if (wheel_delta != 0) {
        // Handle mouse wheel scrolling
        int scroll_lines = wheel_delta / WHEEL_DELTA; // How many lines to scroll
        m_si.nPos = std::clamp(m_si.nPos - scroll_lines, m_si.nMin, m_si.nMax);
    }
    else {
        // Handle regular scroll events
        switch (LOWORD(wParam)) {
        case SB_LINEUP:
            m_si.nPos = std::max(m_si.nMin, m_si.nPos - 1);
            break;
        case SB_LINEDOWN:
            m_si.nPos = std::min(m_si.nMax, m_si.nPos + 1);
            break;
        case SB_PAGEUP:
            m_si.nPos = std::max(m_si.nMin, m_si.nPos - 25);
            break;
        case SB_PAGEDOWN:
            m_si.nPos = std::min(m_si.nMax, m_si.nPos + 25);
            break;
        case SB_THUMBTRACK:
            m_si.nPos = HIWORD(wParam);
            break;
        }
    }

    SetScrollInfo(hwnd, SB_VERT, &m_si, TRUE);

    int delta = m_scroll_pos - m_si.nPos;
    m_scroll_pos = m_si.nPos;

    // Scroll the window contents
    ScrollWindowEx(hwnd, 0, delta * m_line_height, nullptr, nullptr, nullptr, nullptr, SW_INVALIDATE);
}

void window::log_window::scrolling::set_line_rect(const RECT& client, const LONG& offset)
{
    m_line_rect.left = client.left;
    m_line_rect.right = client.right;
    m_line_rect.top = client.top + offset;
    m_line_rect.bottom = client.top + offset + m_line_height;
}

RECT window::log_window::scrolling::create_new_line_rect(const RECT& client, const LONG& offset)
{
    RECT line_rect;
    line_rect.left = client.left;
    line_rect.right = client.right;
    line_rect.top = client.top + offset;
    line_rect.bottom = client.top + offset + m_line_height;
    return line_rect;
}
