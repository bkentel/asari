#include "win_error.h"

namespace asr::win
{

namespace
{

class windows_error_cat final : public std::error_category
{
public:
    char const* name() const noexcept override
    {
        return "windows";
    }

    std::string message(int const e) const noexcept override
    {
        using namespace std::string_literals;

        switch (e)
        {
        case 0: return "none"s;
        }

        ASR_ABORT_UNREACHABLE();
        return "unknown"s;
    }
};

} // namespace

std::error_category const& windows_error_category() noexcept
{
    static windows_error_cat const instance {};
    return instance;
}

} // namespace asr::win
