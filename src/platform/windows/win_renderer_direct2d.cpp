#include "win_renderer_direct2d.h"

#include "win_common.h"
#include "win_error.h"

#include "random.h"

#include "world.h"

#include <d2d1.h>

namespace asr::win
{

namespace
{

class renderer_direc2d_backend final : public renderer_backend
{
public:
    explicit renderer_direc2d_backend(HWND const target)
        : m_window(target)
    {
    }

    void initialize() override;

    void set_data(renderer_data const& data) override;

    void render_frame() override;
private:
    pcg_engine m_engine;

    HWND                   m_window;
    ID2D1Factory*          m_factory       = nullptr;
    ID2D1HwndRenderTarget* m_target        = nullptr;
    ID2D1SolidColorBrush*  m_default_brush = nullptr;

    renderer_data const*   m_data = nullptr;
};

void renderer_direc2d_backend::initialize()
{
    RECT client_area {};
    if (!::GetClientRect(m_window, &client_area))
    {
        ASR_THROW(get_last_error(), "d2d", "GetClientRect failed");
    }

    if (auto const hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory);
        FAILED(hr))
    {
        ASR_THROW(make_com_error(hr), "d2d", "D2D1CreateFactory failed");
    }

    auto target_properties = D2D1::RenderTargetProperties();
    target_properties.type = D2D1_RENDER_TARGET_TYPE::D2D1_RENDER_TARGET_TYPE_HARDWARE;
    //target_properties.pixelFormat.format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_TYPELESS;

    auto window_target_properties = D2D1::HwndRenderTargetProperties(m_window);
    window_target_properties.pixelSize.width  = client_area.right  - client_area.left;
    window_target_properties.pixelSize.height = client_area.bottom - client_area.top;

    if (auto const hr = m_factory->CreateHwndRenderTarget(target_properties, window_target_properties, &m_target);
        FAILED(hr))
    {
        ASR_THROW(make_com_error(hr), "d2d", "D2D1CreateFactory::CreateHwndRenderTarget failed");
    }

    m_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Chocolate), &m_default_brush);
}

void renderer_direc2d_backend::set_data(renderer_data const& data)
{
    m_data = &data;
}

void renderer_direc2d_backend::render_frame()
{
    ASR_ASSERT(m_data);

    auto const& world_data = *m_data->world;

    rect_u32 const region {{0, 0}, {10, 10}};

    auto const& terrain_data = world_data.terrain(region);

    auto& target = *m_target;

    target.BeginDraw();

    target.SetTransform(D2D1::Matrix3x2F::Identity());

    target.Clear(nullptr);

    for (size_t i = 0; i < 10 * 10; ++i)
    {
        auto const x = i % 10;
        auto const y = i / 10;

        auto const r = D2D1::RectF(x * 32, y * 32, (x + 1) * 32, (y + 1) * 32);

        target.FillRectangle(r, m_default_brush);
    }

    D2D1_TAG tag1 {};
    D2D1_TAG tag2 {};

    auto const hr = m_target->EndDraw(&tag1, &tag2);
    if (FAILED(hr))
    {
        ASR_ASSERT(false);
    }
}

} // namespace

std::unique_ptr<renderer_backend> make_direct2d_renderer(HWND const target)
{
    return std::make_unique<renderer_direc2d_backend>(target);
}

} // namespace asr::win
