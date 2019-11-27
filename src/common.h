#pragma once

#include <type_traits>
#include <system_error>
#include <exception>
#include <string_view>

#include <cstdio>
#include <cstdarg>

#define ASR_NOINLINE __declspec(noinline)

namespace asr
{

template <typename...>
inline constexpr bool dependent_false = false;

template <typename E>
[[noreturn]] ASR_NOINLINE void throw_system_error(E&& e)
{
    using T = std::remove_cvref_t<E>;

    using std::make_error_code;

    if constexpr (std::is_enum_v<T>)
    {
        throw std::system_error(make_error_code(e), "");
    }
    else if constexpr (std::is_same_v<T, std::error_code>)
    {
        throw std::system_error(std::forward<E>(e), "");
    }
    else if constexpr (std::is_base_of_v<std::system_error, T>)
    {
        throw e;
    }
    else if constexpr (std::is_same_v<std::exception_ptr, T>)
    {
        try
        {
            std::rethrow_exception(std::forward<E>(e));
        }
        catch (std::system_error const& se)
        {
            throw se;
        }
        catch (std::exception const& se)
        {
            throw std::system_error(make_error_code(std::errc::interrupted), se.what());
        }
        catch (...)
        {
            throw std::system_error(make_error_code(std::errc::interrupted), "unknown exception");
        }
    }
    else
    {
        static_assert(dependent_false<E>);
    }
}

[[noreturn]] inline void assertion_handler()
{
    std::abort();
}

[[noreturn]] void fail_fast();

inline constexpr bool volatile const always_false = false;

struct log_data
{
    std::string_view format;
};

void log_sink(log_data const& data, void*, ...) noexcept;

} // namespace asr

#define ASR_THROW(e, component, fmt, ...) ::asr::throw_system_error(e)

#define ASR_ASSERT(x) ((x) || (::asr::assertion_handler(), true))

#define ASR_ABORT_UNREACHABLE() (always_false || (::asr::fail_fast(), true))

#define ASR_DISCARD_RESULT (void)

#define ASR_APPEND_IMPL(x, y) x ## y

#define ASR_APPEND(x, y) ASR_APPEND_IMPL(x, y)

#if defined(_MSC_VER)
#   define ASR_LOG_MESSAGE(fmt, ...) \
    { \
        using namespace std::string_view_literals; \
        static constexpr ::asr::log_data ASR_APPEND(data, __LINE__) {ASR_APPEND(fmt, sv)}; \
        ::asr::log_sink(ASR_APPEND(data, __LINE__), nullptr, __VA_ARGS__); \
    } \
    (::asr::always_false && (::_printf_p((fmt), __VA_ARGS__), 0))
#else
#   define ASR_LOG_MESSAGE(fmt, ...) (void)
#endif

#define ASR_LOGE(fmt, ...) ASR_LOG_MESSAGE(fmt, __VA_ARGS__)
#define ASR_LOGA(fmt, ...) ASR_LOG_MESSAGE(fmt, __VA_ARGS__)
#define ASR_LOGV(fmt, ...) ASR_LOG_MESSAGE(fmt, __VA_ARGS__)
