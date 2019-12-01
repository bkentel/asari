#pragma once

#include <string_view>
#include <array>
#include <string>
#include <utility>

namespace asr
{

constexpr bool to_hex(
      char*       const out
    , size_t      const out_size
    , char const* const data
    , size_t      const data_size) noexcept
{
    constexpr char table[] {
          '0', '1', '2', '3'
        , '4', '5', '6', '7'
        , '8', '9', 'A', 'B'
        , 'C', 'D', 'E', 'F'
    };

    std::string_view const sv {data, data_size};

    auto it   = out;
    auto last = it + out_size;

    for (auto const c : sv)
    {
        if (it == last || it + 1 == last)
        {
            return false;
        }

        *it++ = table[(c & 0xF0) >> 4];
        *it++ = table[(c & 0x0F) >> 0];
    }

    return true;
}

constexpr bool widen(
      wchar_t*    const out
    , size_t      const out_size
    , char const* const data
    , size_t      const data_size) noexcept
{
    std::string_view const sv {data, data_size};

    auto it   = out;
    auto last = it + out_size;

    for (auto const c : sv)
    {
        if (it == last)
        {
            return false;
        }

        auto const w = static_cast<wchar_t>(c & 0x7F);

        *it++ = w ? w : L' ';
    }

    return true;
}

template <typename T>
auto to_hex(T const& bytes) noexcept
{
    std::array<char, sizeof(T) * 2 + 1> result {};

    to_hex(
          data(result)
        , size(result)
        , reinterpret_cast<char const*>(std::addressof(bytes))
        , sizeof(bytes));

    return result;
}

template <typename Container>
std::wstring widen(Container const& narrow) noexcept
{
    using std::size;
    using std::data;

    std::wstring result;
    result.resize(narrow.size());

    widen(data(result), size(result), data(narrow), size(narrow));

    return result;
}

} // namespace asr
