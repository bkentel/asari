#include "common.h"
#include "engine.h"

#include "platform/ime.h"
#include "platform/app.h"

namespace asr
{

namespace
{

#if defined(_MSC_VER)
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

int main([[maybe_unused]] std::vector<std::string_view> const& args)
{
    ASR_LOGA("[main] args[%zu]", args.size());

    intial_setup();

    std::unique_ptr<engine> engine;

    auto const ime = make_input_manager();

    application_options app_options {};
    app_options.ime = ime.get();
    app_options.on_ready = [&] {
        engine = make_engine();
    };

    auto const app = make_application(std::move(app_options));

    return 0;
}

} // namespace asr
