#include "window_classes.hpp"
/***********************************************
* File: window_classes.cpp
*
* Purpose: window_classes.hpp definitions
*
* Project: core
*
************************************************/

#include NAMES_INCLUDE
#include WINDOW_CLASS_INCLUDE_PATH



LRESULT core::standard_window::s_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // reroute to private window proc
    standard_window* p_window_rerouter = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        // Store the pointer to the window instance in the user data associated with the HWND.
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        p_window_rerouter = (standard_window*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p_window_rerouter);
    }
    else
    {
        // Retrieve the pointer to the window instance from the user data associated with the HWND.
        p_window_rerouter = (standard_window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (p_window_rerouter)
    {
        return p_window_rerouter->this_window_proc(hwnd, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

core::codes core::standard_window::load()
{
    {
        codes code = register_window(m_wc);
        if (code != codes::success) {
            return code;
        }
    }

    m_handle = create_window(m_wd);

    if (m_handle == nullptr) {
        return codes::hwnd_error;
    }
    else {
        ShowWindow(m_handle, SW_SHOW);
    }

    return codes::success;
}

UINT core::standard_window::width()
{
    RECT rc = {};
    if (GetClientRect(m_handle, &rc) == FALSE) {
        throw ce(codes::get_client_rect_fail, get_client_rect_fail_description);
    }

    return rc.right - rc.left;
}

UINT core::standard_window::height()
{
    RECT rc = {};
    if (GetClientRect(m_handle, &rc) == FALSE) {
        throw ce(codes::get_client_rect_fail, get_client_rect_fail_description);
    }

    return rc.bottom - rc.top;
}

core::drawing::drawing()
{
    fill_buffer(m_draw_width * m_draw_height * PIXEL);
    std::vector<std::complex<float>> path = generate_koch_snowflake(4); // or any shape
    fourier = compute_dft(path);
}

void core::drawing::draw()
{
    HDC hdc = GetDC(m_handle);

    // Define bitmap info header
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_draw_width;
    bmi.bmiHeader.biHeight = -m_draw_height; // negative = top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Draw the pixels to the window
    StretchDIBits(
        hdc,
        0, 0, m_draw_width, m_draw_height,         // dest rect
        0, 0, m_draw_width, m_draw_height,         // source rect
        m_frame_buffer_v.data(),         // pixel data
        &bmi,
        DIB_RGB_COLORS,
        SRCCOPY
    );

    ReleaseDC(m_handle, hdc);
}

void core::drawing::update(float dt)
{
    m_accum_time += dt;

    // Clear screen
    fill_buffer(m_draw_width * m_draw_height * PIXEL);

    std::complex<float> endpoint = draw_epicycles(m_accum_time, fourier, m_draw_width / 2, m_draw_height / 2);

    // Save path
    trail.push_back(endpoint);
    if (trail.size() > 1000)
        trail.erase(trail.begin());

    // Draw the trail
    for (const auto& p : trail) {
        put_pixel((int)p.real(), (int)p.imag(), 255, 255, 0);
    }

    m_accum_time += 1.0f / fourier.size(); // loop once per cycle
    if (m_accum_time > 1.0f)
        m_accum_time = 0;
}

void core::drawing::put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= m_draw_width || y < 0 || y >= m_draw_height)
        return;

    int i = (y * m_draw_width + x) * 3;
    m_frame_buffer_v[i + 0] = b;
    m_frame_buffer_v[i + 1] = g;
    m_frame_buffer_v[i + 2] = r;
}

void core::drawing::line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b)
{
    int dx = std::abs(x1 - x0), dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        put_pixel(x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

std::vector<core::FourierComponent> core::drawing::compute_dft(const std::vector<std::complex<float>>& input)
{
    int N = input.size();
    std::vector<FourierComponent> result;

    for (int k = 0; k < N; ++k) {
        std::complex<float> sum = 0;
        for (int n = 0; n < N; ++n) {
            float angle = 2.0f * M_PI * k * n / N;
            sum += input[n] * std::exp(std::complex<float>(0, -angle));
        }
        sum /= static_cast<float>(N);

        FourierComponent fc;
        fc.freq = k;
        fc.amplitude = std::abs(sum);
        fc.phase = std::arg(sum);
        fc.coeff = sum;
        result.push_back(fc);
    }

    // Sort by amplitude (largest vectors first)
    std::sort(result.begin(), result.end(), [](auto& a, auto& b) {
        return a.amplitude > b.amplitude;
        });

    return result;
}

std::vector<std::complex<float>> core::drawing::generate_circle(int numPoints, float radius)
{
    std::vector<std::complex<float>> points;
    for (int i = 0; i < numPoints; ++i) {
        float angle = 2 * M_PI * i / numPoints;
        points.push_back(std::polar(radius, angle));
    }
    return points;
}

std::vector<std::complex<float>> core::drawing::generate_bicycle_path()
{
    std::vector<std::complex<float>> path;

    // Wheel 1 (left)
    float r = 30;
    for (int i = 0; i < 32; ++i) {
        float a = 2 * M_PI * i / 32;
        path.push_back({ -80 + r * std::cos(a), -30 + r * std::sin(a) });
    }

    // Frame triangle
    path.push_back({ -80, -30 });
    path.push_back({ -20, 0 });   // pedal
    path.push_back({ 20,  0 });  // front
    path.push_back({ -20, 0 });   // back
    path.push_back({ -50, 20 });  // seat post
    path.push_back({ 0, 40 });   // top bar
    path.push_back({ 20, 0 });    // front again

    // Wheel 2 (right)
    for (int i = 0; i < 32; ++i) {
        float a = 2 * M_PI * i / 32;
        path.push_back({ 80 + r * std::cos(a), -30 + r * std::sin(a) });
    }

    // Handlebars
    path.push_back({ 20, 0 });
    path.push_back({ 40, 40 });
    path.push_back({ 35, 50 });
    path.push_back({ 45, 50 });

    // Return to start
    path.push_back(path.front());

    return path;
}

std::vector<std::complex<float>> core::drawing::generate_koch_snowflake(int level)
{
    float size = 200.0f;
    std::complex<float> A(-size / 2, -size / 3);
    std::complex<float> B(size / 2, -size / 3);
    float h = std::sqrt(3.0f) / 2.0f * size;
    std::complex<float> C(0, h - size / 3);

    std::vector<std::complex<float>> base = { A, B, C, A };
    subdivide_koch(base, level);
    return base;
}

void core::drawing::subdivide_koch(std::vector<std::complex<float>>& path, int level)
{
    if (level == 0) return;

    std::vector<std::complex<float>> new_path;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        std::complex<float> a = path[i];
        std::complex<float> b = path[i + 1];
        std::complex<float> v = (b - a) / 3.0f;
        std::complex<float> p1 = a + v;
        std::complex<float> p2 = p1 + v * std::polar(1.0f, (float)-M_PI / 3.0f);
        std::complex<float> p3 = a + v * 2.0f;

        new_path.push_back(a);
        new_path.push_back(p1);
        new_path.push_back(p2);
        new_path.push_back(p3);
    }

    new_path.push_back(path.back());

    path = new_path;
    subdivide_koch(path, level - 1);
}

std::complex<float> core::drawing::draw_epicycles(float time, const std::vector<FourierComponent>& fourier, int cx, int cy)
{
    std::complex<float> prev = { static_cast<float>(cx), static_cast<float>(cy) };

    for (const auto& f : fourier) {
        float angle = 2 * M_PI * f.freq * time + f.phase;
        std::complex<float> next = prev + std::polar(f.amplitude, angle);

        // Draw vector line
        line((int)prev.real(), (int)prev.imag(), (int)next.real(), (int)next.imag(), 255, 255, 255);

        prev = next;
    }

    return prev; // endpoint
}

void core::drawing::fill_buffer(std::size_t size)
{
    m_frame_buffer_v = std::vector<uint8_t>(size);
}

void core::drawing::resize_draw_window(UINT new_width, UINT new_height)
{
    std::size_t buffer_size = static_cast<std::size_t>(new_width * new_height * 3);
    fill_buffer(buffer_size);
    
    m_draw_width = std::clamp(new_width,(UINT)MIN_WIDTH,this->width());
    m_draw_height = std::clamp(new_height,(UINT)MIN_HEIGHT,this->height());
}


LRESULT core::standard_window::this_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    } // end of switch (uMsg)

    // no default switches needed
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND core::create_window(const window_description& wd) {

    /*

    HWND CreateWindowExW(
          [in]           DWORD     dwExStyle,
          [in, optional] LPCWSTR   lpClassName,
          [in, optional] LPCWSTR   lpWindowName,
          [in]           DWORD     dwStyle,
          [in]           int       X,
          [in]           int       Y,
          [in]           int       nWidth,
          [in]           int       nHeight,
          [in, optional] HWND      hWndParent,
          [in, optional] HMENU     hMenu,
          [in, optional] HINSTANCE hInstance,
          [in, optional] LPVOID    lpParam
        );

    */

    return CreateWindowEx(
        wd.dwExStyle,
        wd.lpClassName,
        wd.lpWindowName,
        wd.dwStyle,
        wd.X,
        wd.Y,
        wd.nWidth,
        wd.nHeight,
        wd.hWndParent,
        wd.hMenu,
        wd.hInstance,
        wd.lpParam
    );
}

core::codes core::register_window(const WNDCLASSEX& wc)
{
    ATOM atm = RegisterClassEx(&wc);

    if (atm == 0) {
        return codes::register_class_fail;
    }

    return codes::success;
}
