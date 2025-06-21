#include "system_log_window.hpp"

errors::codes window::system_log_window::go()
{
    set_font(m_font_size);
    window_settings();
    create_window();
    remove_x_log_window();
    m_scroll_p->set_scroll_info(m_window_handle);

    // signal that the window is fully operational
    m_fully_init_gate_latch.store(true);
    m_fully_init.notify_all();

    message_pump();
	return errors::codes::success;
}

errors::codes window::system_log_window::shutdown()
{
    SendMessage(m_window_handle, WM_CLOSE, 0, 0);
    return errors::codes();
}

errors::codes window::system_log_window::wait_until_init()
{
    // wait here
    std::mutex local_mtx;
    std::unique_lock<std::mutex> local_lock(local_mtx);
    m_fully_init.wait(local_lock, [this]
        {
            return m_fully_init_gate_latch.load();
        });
    
    return errors::codes();
}

void window::system_log_window::window_settings()
{
    m_slw_wc.lpfnWndProc = WindowProc;
    m_slw_wc.hInstance = m_hinst;
    m_slw_wc.lpszClassName = READ_ONLY_STRING("system log window");

    ATOM atm = RegisterClass(&m_slw_wc);

#if ENABLE_FULL_DEBUG
    if (atm == FALSE) {
        errors::handle_win32_error_codes(errors::win32_codes::register_class_fail);
    }
#endif

#if ENABLE_ALL_EXCEPTIONS
    if (atm == FALSE) {
        win32_code_objs::code_obj error(win32_code_objs::register_class_fail);
        throw errors::win32_register_class_fail(error);
    }
#endif
}

void window::system_log_window::create_window()
{
    m_window_handle = CreateWindowEx(
        0,                                                  // Optional window styles.
        m_slw_class_name.c_str(),                           // Window class
        m_slw_title.c_str(),                                // Window text
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,                   // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,                                           // Parent window    
        NULL,                                           // Load the menu here
        m_hinst,                                        // Instance handle
        this                                            // Additional application data
    );

#if ENABLE_ALL_EXCEPTIONS
    if (m_window_handle == nullptr) {
        win32_code_objs::code_obj error(win32_code_objs::HWND_error);
        throw errors::win32_HWND_error(error);
    }
#endif

#if ENABLE_FULL_DEBUG
    if (m_window_handle == nullptr) {
        errors::handle_win32_error_codes(errors::win32_codes::HWND_error);
    }
#endif

    ShowWindow(m_window_handle, SW_SHOW);
}
