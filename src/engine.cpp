#include "engine.h"

#include "common.h"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace asr
{

namespace
{

class engine_impl final : public engine
{
public:
    explicit engine_impl(engine_options options)
        : m_options(options)
        , m_thread([&]() noexcept { main(); })
    {
    }

    ~engine_impl()
    {
        {
            std::scoped_lock const lock {m_mutex};
            stop_impl();
        }

        m_cv.notify_all();

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void stop() override
    {
        {
            std::scoped_lock const lock {m_mutex};
            ASR_ASSERT(m_running);
            stop_impl();
        }

        m_cv.notify_all();
    }
private:
    //! @pre must be locked
    void stop_impl()
    {
        m_running = false;
    }

    void main() noexcept
    {
        for (;;)
        {
            std::unique_lock lock {m_mutex};

            m_cv.wait(lock, [&] {
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
    bool                    m_running = true;
    engine_options          m_options;
    std::thread             m_thread;
};

} // namespace

engine::~engine() = default;

std::unique_ptr<engine> make_engine(engine_options options)
{
    return std::make_unique<engine_impl>(std::move(options));
}

} // namespace asr
