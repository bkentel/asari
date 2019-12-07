#include "renderer.h"

#include "common.h"

#include <thread>
#include <condition_variable>
#include <mutex>

namespace asr::win
{

extern void foo(void* const window);

}

namespace asr
{

renderer_backend::~renderer_backend() = default;

renderer::~renderer() = default;

namespace
{

class renderer_impl final : public renderer
{
public:
    explicit renderer_impl(renderer_options options)
        : m_options(std::move(options))
        , m_thread([&]() noexcept { main(); })
    {
        ASR_ASSERT(m_options.native_target);
    }

    void stop() noexcept override
    {
        {
            std::scoped_lock const lock {m_mutex};
            m_running = false;
        }

        m_cv.notify_all();
    }

    ~renderer_impl()
    {
        stop();

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }
private:
    void main() noexcept
    {
        std::unique_lock lock {m_mutex};

        m_options.native_target->initialize();

        for (;;)
        {
            m_options.native_target->render_frame();

            m_cv.wait(lock, [&]() noexcept {
                return !m_running;
            });

            if (!m_running)
            {
                break;
            }
        }
    }
private:
    mutable std::mutex      m_mutex;
    std::condition_variable m_cv;
    renderer_options        m_options;
    bool                    m_running = true;
    std::thread             m_thread;
};

} // namespace

std::unique_ptr<renderer> make_renderer(
    renderer_options options)
{
    return std::make_unique<renderer_impl>(std::move(options));
}

} // namespace asr
