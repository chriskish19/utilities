#include "main_window_class_mt.hpp"

window::window_class_mt::~window_class_mt()
{
	// clean up

    if (m_thread_runner != nullptr) {
		delete m_thread_runner;
	}

    // the latch pointer
    if (m_wcmt_latches->load() != nullptr) {
        auto actual_p = m_wcmt_latches->load();
        delete actual_p;
    }

    // the std::atomic pointer
    if (m_wcmt_latches != nullptr) {
        delete m_wcmt_latches;
    }
}

void window::window_class_mt::go()
{
#if ENABLE_DEEP_LOGS

    global::system_global_manager_p->log_to_system_log_window(READ_ONLY_STRING("Launching master thread inside main_window_class_mt..."));

#endif

    // this-> is actually on a different instance of master_thread than run_windows_class_mt's instance
	auto j_thread_p = this->launch_master_thread(&run_windows_class_mt::threads_go, m_thread_runner);

#if ENABLE_ALL_EXCEPTIONS

    if (j_thread_p == nullptr) {
        code_error_objs::code_obj code(code_error_objs::pointer_is_nullptr);
        throw errors::pointer_is_nullptr(code,READ_ONLY_STRING("auto j_thread_p"));
    }

#endif


#if ENABLE_DEEP_LOGS

    if (j_thread_p != nullptr) {
        global::system_global_manager_p->log_to_system_log_window(READ_ONLY_STRING("master thread successfully launched inside main_window_class_mt..."));
    }

#endif
}

void window::window_class_mt::wait() noexcept
{
    // wait here
    std::mutex local_mtx;
    std::unique_lock<std::mutex> local_lock(local_mtx);
    m_wcmt_latches->load()->m_safe_exit.wait(local_lock, [this]
        {
            return m_wcmt_latches->load()->m_safe_exit_gate_latch.load();
        });
}

window::window_class_mt::run_windows_class_mt::~run_windows_class_mt()
{
    if (m_wm != nullptr) {
        delete m_wm;
    }
}

void window::window_class_mt::run_windows_class_mt::threads_go()
{
	// launch a first thread window
	this->launch_thread(&window_manager::windows_message_handler, m_wm);

	// creates new windows on thread_master
	this->launch_thread(&run_windows_class_mt::new_window_gate, this);

	// exit program thread
	all_windows_closed_gate();
}

void window::window_class_mt::run_windows_class_mt::all_windows_closed_gate() noexcept
{
	std::mutex local_mtx;
	std::unique_lock<std::mutex> local_lock(local_mtx);
	m_wm->m_public_all_windows_closed_signaler.wait(local_lock, [this]
		{
			return m_wm->m_all_windows_closed_gate_latch.load();
		});

	// begin to exit the program
	m_exit_new_window_gate.store(true);
	m_wm->m_latches->load()->m_new_window_gate_latch.store(true);
	m_wm->m_latches->load()->m_window_create_signaler.notify_all();  // get the thread moving again if its stalled
}

void window::window_class_mt::run_windows_class_mt::new_window_gate() noexcept
{
	while (m_exit_new_window_gate.load() == false) {
		std::mutex local_mtx;
		std::unique_lock<std::mutex> local_lock(local_mtx);
		m_wm->m_latches->load()->m_window_create_signaler.wait(local_lock, [this]
			{
				return m_wm->m_latches->load()->m_new_window_gate_latch.load();
			});
		m_wm->m_latches->load()->m_new_window_gate_latch.store(false);

		// double check were not in a exit scenario
		if (m_exit_new_window_gate.load() == false) {
			this->launch_thread(&window_manager::windows_message_handler, m_wm);
		}
	}

	m_wm->m_latches->load()->m_safe_exit_gate_latch.store(true);
	m_wm->m_latches->load()->m_safe_exit.notify_all();
}

void window::window_class_mt::run_windows_class_mt::launch_new_window()
{
    this->launch_thread(&window_manager::windows_message_handler, m_wm);
}

LRESULT window::window_class_mt::window_relative::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // reroute to private window proc
    window_relative* p_window_rerouter = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        // Store the pointer to the window instance in the user data associated with the HWND.
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        p_window_rerouter = (window_relative*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p_window_rerouter);
    }
    else
    {
        // Retrieve the pointer to the window instance from the user data associated with the HWND.
        p_window_rerouter = (window_relative*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (p_window_rerouter)
    {
        return p_window_rerouter->PrivateWindowProc(hwnd, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT window::window_class_mt::window_relative::PrivateWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);

            break;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case static_cast<int>(window_menu_ids::Create): // menu button called "Create New Window"
                {
                    // signal to window_class_mt to create a new thread window
                    m_latches->load()->m_new_window_gate_latch.store(true);
                    m_latches->load()->m_window_create_signaler.notify_all();
                    break;
                }
                case static_cast<int>(window_menu_ids::view_log_window):
                {
                    m_show_log_window = !m_show_log_window;
                    view_log_window(m_show_log_window);
                    break;
                }
                default:
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
            } // end of switch (LOWORD(wParam))
            break;
        }
        case WM_CLOSE: 
        {
            // signal to window_class_mt to kill the thread??
            break;
        }

    } // end of switch (uMsg)

    // no default switches needed
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void window::window_class_mt::window_relative::register_class()
{
    // the class might already be registered
    if (m_class_atm.load() != 0) {
        return;
    }

    m_wc.lpfnWndProc = WindowProc;
    m_wc.hInstance = m_hinst;
    m_wc.lpszClassName = m_c_name.c_str();

    ATOM atm = RegisterClass(&m_wc);

#if ENABLE_FULL_DEBUG

    if (atm == FALSE) {
        win32_code_objs::code_obj code(win32_code_objs::register_class_fail);
        errors::win32_register_class_fail w32code(code);
        errors::show_error_message_window(w32code.full_error_message());
    }

#endif

#if ENABLE_ALL_EXCEPTIONS

    if (atm == FALSE) {
        win32_code_objs::code_obj code(win32_code_objs::register_class_fail);
        throw errors::win32_register_class_fail(code);
    }

#endif

    m_class_atm.store( atm );
}

window::window_class_mt::window_relative::window_relative(const string& title, std::atomic<latch*>* latches_p)
    :m_title(title),m_latches(latches_p)
{
    this->register_class();
     
    m_window_handle = CreateWindowEx(
        NULL,                                               // Optional window styles.
        m_c_name.c_str(),                                   // Window class
        m_title.c_str(),                                    // Window text
        WS_OVERLAPPEDWINDOW,                                // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,                                               // Parent window    
        NULL,                                               // Load the menu here
        m_hinst,                                            // Instance handle
        this                                                // Additional application data
    );

    ShowWindow(m_window_handle, SW_SHOW);

    this->build_relative_window_menu_bar();
}

window::window_class_mt::window_relative::~window_relative()
{
    global::system_global_manager_p->log_to_system_log_window(m_title + READ_ONLY_STRING(":is now closed."));
}

void window::window_class_mt::window_relative::change_title(const string& new_title) noexcept
{
    SetWindowText(this->m_window_handle, new_title.c_str());
}

errors::win32_codes window::window_class_mt::window_relative::build_relative_window_menu_bar()
{

// no error checking or exceptions
#if ENABLE_FULL_OPTIMIZATIONS
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();
    HMENU h_view_menu = CreateMenu();
    AppendMenu(hHelpMenu, MF_STRING, static_cast<int>(window_menu_ids::Help), READ_ONLY_STRING("&Help"));
    AppendMenu(hFileMenu, MF_STRING, static_cast<int>(window_menu_ids::Create), READ_ONLY_STRING("&Create New Window"));
    AppendMenu(h_view_menu, MF_STRING, static_cast<int>(window_menu_ids::view_log_window), READ_ONLY_STRING("&Log window"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, READ_ONLY_STRING("&File"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, READ_ONLY_STRING("&Help"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)h_view_menu, READ_ONLY_STRING("&View"));
    SetMenu(this->m_window_handle, hMenu);
    return errors::codes::success;
#endif


#if ENABLE_FULL_DEBUG

    HMENU hMenu = CreateMenu();
    {
        errors::win32_codes code = utilities::win32_menu_check(hMenu);
        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    HMENU hFileMenu = CreateMenu();
    {
        errors::win32_codes code = utilities::win32_menu_check(hFileMenu);
        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    HMENU hHelpMenu = CreateMenu();
    {
        errors::win32_codes code = utilities::win32_menu_check(hHelpMenu);
        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    HMENU h_view_menu = CreateMenu();
    {
        errors::win32_codes code = utilities::win32_menu_check(h_view_menu);
        if (code != errors::win32_codes::success) {
            return code;
        }
    }
    
    {
        errors::win32_codes code =
            utilities::win32_append_menu_check(
                AppendMenu(hHelpMenu, MF_STRING, static_cast<int>(window_menu_ids::Help), READ_ONLY_STRING("&Help"))
            );

        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    {
        errors::win32_codes code =
            utilities::win32_append_menu_check(
                AppendMenu(hFileMenu, MF_STRING, static_cast<int>(window_menu_ids::Create), READ_ONLY_STRING("&Create New Window"))
            );

        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    {
        errors::win32_codes code =
            utilities::win32_append_menu_check(
                AppendMenu(h_view_menu, MF_STRING, static_cast<int>(window_menu_ids::view_log_window), READ_ONLY_STRING("&Log window"))
            );

        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    {
        errors::win32_codes code =
            utilities::win32_append_menu_check(
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, READ_ONLY_STRING("&File"))
        );
        
        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    {
        errors::win32_codes code =
            utilities::win32_append_menu_check(
                AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, READ_ONLY_STRING("&Help"))
            );

        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    {
        errors::win32_codes code =
            utilities::win32_append_menu_check(
                AppendMenu(hMenu, MF_POPUP, (UINT_PTR)h_view_menu, READ_ONLY_STRING("&View"))
            );

        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    {
        errors::win32_codes code =
            utilities::win32_append_menu_check(
                SetMenu(this->m_window_handle, hMenu)
            );

        if (code != errors::win32_codes::success) {
            return code;
        }
    }

    return errors::win32_codes::success;

#endif // ENABLE_FULL_DEBUG

}

void window::window_class_mt::window_relative::run_window_logic(dx::devices_11* dx11_device_p, log_window* log_p)
{

#if ENABLE_ALL_EXCEPTIONS
    if (log_p == nullptr) {
        code_error_objs::code_obj code(code_error_objs::pointer_is_nullptr);
        throw errors::pointer_is_nullptr(code, READ_ONLY_STRING("log_window* log_p"));
    }
#endif
    // makes these safe to dereference in here since its run on a thread
    m_log_window_p.store(log_p);
    m_dx11_device_p.store(dx11_device_p);

    // graphics stuff!!
    m_swp_p = m_dx11_device_p.load()->get_swap_p();

#if ENABLE_ALL_EXCEPTIONS
    if (m_swp_p == nullptr) {
        code_error_objs::code_obj code(code_error_objs::pointer_is_nullptr);
        throw errors::pointer_is_nullptr(code,READ_ONLY_STRING("m_swp_p = dx11_device_p->get_swap_p()"));
    }
#endif

    while (m_public_exit_run_window_logic.load() == false) {
        m_swp_p->Present(1u, 0u);
    }

}

UINT window::window_class_mt::window_relative::get_window_width()
{
    RECT rc = {};
    if (GetClientRect(m_window_handle, &rc) == FALSE) {
#if ENABLE_ALL_EXCEPTIONS
        win32_code_objs::code_obj error_obj(win32_code_objs::get_client_rect_fail);
        throw errors::get_client_rect_failed(error_obj);
#endif

#if ENABLE_DEEP_LOGS
        global::system_global_manager_p->log_to_system_log_window(READ_ONLY_STRING
        ("GetClientRect(m_window_handle, &rc) failed... trying to get window width."));
#endif
    }

    return rc.right - rc.left;
}

UINT window::window_class_mt::window_relative::get_window_height()
{
    RECT rc = {};
    if (GetClientRect(m_window_handle, &rc) == FALSE) {
#if ENABLE_ALL_EXCEPTIONS
        win32_code_objs::code_obj code(win32_code_objs::get_client_rect_fail);
        throw errors::get_client_rect_failed(code);
#endif

#if ENABLE_DEEP_LOGS
        global::system_global_manager_p->log_to_system_log_window(READ_ONLY_STRING
        ("GetClientRect(m_window_handle, &rc) failed... trying to get window height."));
#endif
    }

    return rc.bottom - rc.top;
}

void window::window_class_mt::window_relative::close_window()
{
    // todo: handle the errors later
    SendMessage(m_window_handle, WM_CLOSE, 0, 0);
}

errors::codes window::window_class_mt::window_relative::view_log_window(bool show)
{
    HWND log_window_handle = m_log_window_p.load()->get_window_handle();
    
    // Get the handle to the menu
    HMENU hMenu = GetMenu(log_window_handle);
    HMENU hSubMenu = GetSubMenu(hMenu, 0); // View menu (first submenu)

    // Update the checkmark
    CheckMenuItem(hSubMenu, static_cast<int>(window_menu_ids::view_log_window),
        MF_BYCOMMAND | (show ? MF_CHECKED : MF_UNCHECKED));

    // Perform action based on the state
    if (show == true) {
        // Show the log window
        ShowWindow(log_window_handle, SW_SHOW);
    }
    else {
        // Hide the log window
        ShowWindow(log_window_handle, SW_HIDE);
    }
    
    
    return errors::codes::success;
}

void window::window_class_mt::window_manager::windows_message_handler() {
    
    m_open_window_count++;

#if USING_WIDE_STRINGS
    string window_number = std::to_wstring(m_open_window_count);
#endif

#if USING_NARROW_STRINGS
    string window_number = std::to_string(m_open_window_count);
#endif

    string new_window_name = READ_ONLY_STRING("Display Window #") + window_number;
    string new_logger_name = READ_ONLY_STRING("Logger Window #") + window_number;

    window_class_log_window* new_logger = new window_class_log_window(new_logger_name);
    new_logger->go();

    window_relative* new_window = new window_relative(new_window_name, m_latches);

#if TESTING
    m_windows_vec.push_back(new_window);
#endif

    dx::devices_11* new_device = new dx::devices_11(new_window->get_window_width(),new_window->get_window_height(),
        new_window->get_window_handle(),new_window_name);

    std::jthread logic_thread(&window::window_class_mt::window_relative::run_window_logic, new_window, new_device, new_logger);
   
    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    m_open_window_count--;

    if (m_open_window_count.load() == 0) {
        m_all_windows_closed_gate_latch.store(true);
        m_public_all_windows_closed_signaler.notify_all();
    }

    // exit the function
    new_window->m_public_exit_run_window_logic.store(true);


    // cleanup
    if (new_logger != nullptr) {
        delete new_logger;
    }

    if (new_window != nullptr) {
        delete new_window;
    }

    if (new_device != nullptr) {
        delete new_device;
    }
}
