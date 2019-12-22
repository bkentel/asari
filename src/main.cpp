#include "common.h"
#include "engine.h"
#include "renderer.h"

#include "platform/ime.h"
#include "platform/app.h"

#include "world.h"

namespace asr
{

namespace
{

#if defined(ASR_COMPILER_MSVC)
#   define ASR_VFPRINTF(a, b, c) ::_vfprintf_l(a, b, nullptr, c)
#else
#   define ASR_VFPRINTF(a, b, c) ::vfprintf(a, b, c)
#endif // _MSC_VER

void log_sink_v(log_data const& data, va_list const args) noexcept
{
    auto const fmt = data.format.data();
    auto const n   = data.format.size();

    ASR_ASSERT(*(fmt + n) == '\0');

    ASR_VFPRINTF(stdout, fmt, args);

    if (n > 0 && fmt[n - 1] != '\n')
    {
        ::fputc('\n', stdout);
    }
}

#undef ASR_VFPRINTF

} // namespace

void log_sink(log_data const& data, void* const dummy, ...) noexcept
{
    va_list args;
    va_start(args, dummy);

    log_sink_v(data, args);

    va_end(args);
}

class application_kernel final : public application_observer
{
    static auto make_application(application_kernel& instance)
    {
        ASR_ASSERT(instance.m_ime.get());

        application_options options {};
        options.ime      = instance.m_ime.get();
        options.observer = &instance;

        return asr::make_application(std::move(options));
    }

    static auto make_input_manager(application_kernel&)
    {
        return asr::make_input_manager();
    }
public:
    application_kernel();

    void start();

    void on_system_event(std::monostate const&) noexcept { ASR_ABORT_UNREACHABLE(); }

    void on_system_event(mouse_event    const& event) noexcept;
    void on_system_event(button_event   const& event) noexcept;
    void on_system_event(keyboard_event const& event) noexcept;
public:
    // application_observer

    void on_ready() noexcept override;

    void on_system_event(system_event const& event) noexcept override;
private:
    std::atomic_bool               m_ready = false;

    std::unique_ptr<engine>        m_engine;
    std::unique_ptr<input_manager> m_ime;
    std::unique_ptr<application>   m_app;
    std::unique_ptr<world>         m_world;
    std::unique_ptr<renderer>      m_renderer;
};

application_kernel::application_kernel()
    : m_ime(make_input_manager(*this))
    , m_app(make_application(*this))
    , m_world(make_world())
{
    m_ready = true;
}

void application_kernel::on_ready() noexcept
{
    ASR_ASSERT(m_ready);
    ASR_ASSERT(!m_renderer);
    ASR_ASSERT(!m_engine);

    m_renderer = make_renderer({&m_app->get_renderer_backend(), m_world.get()});
    m_engine   = make_engine({});
}

void application_kernel::on_system_event(mouse_event const&) noexcept
{
}

void application_kernel::on_system_event(button_event const&) noexcept
{
}

void application_kernel::on_system_event(keyboard_event const& event) noexcept
{
    auto const is_non_printable = event.character < 0x20 || event.character >= 0x7F;

    if (is_non_printable)
    {
        ASR_LOGA("[main] keyboard_event '0x%x'", static_cast<char>(event.character));
    }
    else
    {
        ASR_LOGA("[main] keyboard_event '%c'", static_cast<char>(event.character));
    }
}

void application_kernel::on_system_event(system_event const& event) noexcept
{
    ASR_ASSERT(m_ready);

    auto const visitor = [this](auto&& v) noexcept {
        on_system_event(v);
    };

    std::visit(visitor, event);
}

void application_kernel::start()
{
    ASR_ASSERT(m_app);
    m_app->start();
}

int main(std::vector<std::string_view> const& args)
{
    intial_setup();

    ASR_LOGA("[main] args[%zu]", args.size());

    application_kernel kernel;
    kernel.start();

    return 0;
}

} // namespace asr
