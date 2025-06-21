#include "test.api.hpp"

void testing::handle_basic_error_codes(errors::codes code, const string& location) noexcept
{
    switch (code) {
    case errors::codes::success:
    {
        COUT << errors_cstr::success << ROS('\n');
        COUT << location << ROS('\n');
        break;
    }

    case errors::codes::division_by_zero:
    {
        COUT << errors_cstr::division_by_zero << ROS("\n");
        COUT << location << ROS("\n");
        break;
    }

    case errors::codes::test_failed:
    {
        COUT << errors_cstr::test_failed << ROS("\n");
        COUT << location << ROS("\n");
        break;
    }

    case errors::codes::test_success:
    {
        COUT << errors_cstr::test_success << ROS("\n");
        COUT << location << ROS("\n");
        break;
    }
    // add the rest

    default:
        COUT << ROS("code not found... \n");
    }
}

testing::string testing::open_test_file(const string& file_name)
{
    std::filesystem::path exe_path = std::filesystem::current_path();
    utilities::file_manager fm(exe_path / "test_files" / file_name);

    std::ios_base::openmode mode = std::ios::in | std::ios::binary;
    errors::codes code = fm.open(mode);
    testing::handle_basic_error_codes(code);

    // get the all the code into a string
    string data = fm.file_data_to_string();
    return data;
}

errors::codes testing::create_windows(size_t number_of_open_windows)
{
    window_t* local_window_test = new window_t;
    local_window_test->go();

    COUT << ROS("running window test...\n");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second
    COUT << ROS("3\n");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second
    COUT << ROS("2\n");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second
    COUT << ROS("1\n");

    local_window_test->create_windows(number_of_open_windows);
    COUT << ROS("running window test...\n");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second
    COUT << ROS("3\n");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second
    COUT << ROS("2\n");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second
    COUT << ROS("1\n");

    local_window_test->close_windows();

    local_window_test->join_master();

    if (local_window_test != nullptr) {
        delete local_window_test;
    }


	return errors::codes(errors::codes::test_success);
}

errors::codes testing::dx11_draw()
{
    testing::basic_window local_window;
    testing::draw local_dx_draw(local_window.width(), local_window.height(), local_window.window_handle(), ROS("Testing Draw"));

    local_dx_draw.ready_triangle();

    auto swap_p = local_dx_draw.get_swap_p();
    auto device_p = local_dx_draw.get_device_p();
    auto device_context_p = local_dx_draw.get_context_p();
    auto triangle_p = local_dx_draw.get_triangle_p();

    // Timer
    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();

    float angle = 0.0f;

    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Frame timing
            auto now = clock::now();
            std::chrono::duration<float> elapsed = now - last_time;
            last_time = now;
            float deltaTime = elapsed.count();

            // Update rotation
            angle += deltaTime * DirectX::XM_PIDIV2; // Rotate 90 degrees per second

            local_dx_draw.render_triangle(angle);
        }
    }

    return errors::codes::test_success;
}

errors::codes testing::string_conversions(const std::string& narrow_test)
{
    std::wstring wide = utilities::to_wide_string(narrow_test);

    std::string narrow = utilities::to_narrow_string(wide);

    if (narrow != narrow_test) {
        return errors::codes::strings_not_equal;
    }
    
    return errors::codes::test_success;
}

errors::codes testing::string_conversions_file(const std::filesystem::path& p)
{
    utilities::file_manager fm(p);
    errors::codes error_code = fm.open(std::ios::in);
    
    if (error_code != errors::codes::success) {
        testing::handle_basic_error_codes(error_code);
        return error_code;
    }

    string file_data = fm.file_data_to_string();

    std::string narrow = utilities::to_narrow_string(file_data);

    std::wstring wide = utilities::to_wide_string(narrow);

    if (file_data != wide) {
        return errors::codes::strings_not_equal;
    }

    
    return errors::codes(errors::codes::test_success);
}

errors::codes testing::window_logger()
{
    window::system_log_window local_log_window;
    std::thread lg_window_thread(&window::system_log_window::go, &local_log_window);
    local_log_window.wait_until_init();
    local_log_window.add_x_log_window();

    auto atomic_log_p = local_log_window.get_logs_p();
    auto logs_p = atomic_log_p->load();
    auto bl_log_p = logs_p->get_base_logger_p_by_index();
    auto log_message_p = bl_log_p->get_message_p();
    auto time_message_p = bl_log_p->get_time_p();

    std::size_t message_length = log_message_p->reserved_length_mem_heap - time_message_p->reserved_length_heap_mem;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(32, 126); // Printable ASCII characters

    auto end_time = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (std::chrono::steady_clock::now() < end_time) {
        string message;
        for (size_t i = 0; i < message_length; ++i) {
            message += dist(gen);
        }
        logs_p->log_message(message);
        local_log_window.update();
    }

    if (lg_window_thread.joinable()) {
        lg_window_thread.join();
    }
    
    return errors::codes(errors::codes::test_success);
}
