#include "window_class_log_window.hpp"

window::window_class_log_window::window_class_log_window(const string& title)
    :m_window_title(title)
{

}

window::window_class_log_window::~window_class_log_window()
{
    global::system_global_manager_p->log_to_system_log_window(m_window_title + READ_ONLY_STRING(":is now closed."));
}

errors::codes window::window_class_log_window::go()
{
    set_font(m_font_size);
    window_settings();
    create_window();
    remove_x_log_window();
    m_scroll_p->set_scroll_info(m_window_handle);
	
	return errors::codes::success;
}

void window::window_class_log_window::window_settings()
{
    if (m_class_atm.load() != 0) {
        return;
    }


    m_wclw_wc.lpfnWndProc = WindowProc;
    m_wclw_wc.hInstance = m_hinst;
    m_wclw_wc.lpszClassName = m_class_name.c_str();

    ATOM atm = RegisterClass(&m_wclw_wc);

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

    m_class_atm.store( atm);
}

void window::window_class_log_window::create_window()
{
    m_window_handle = CreateWindowEx(
        0,                                              // Optional window styles.
        m_class_name.c_str(),                           // Window class
        m_window_title.c_str(),                         // Window text
        WS_OVERLAPPEDWINDOW | WS_VSCROLL,               // Window style

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

    ShowWindow(m_window_handle, SW_HIDE);
}
