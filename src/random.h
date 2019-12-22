#include "config.h"

#include "detail/random_pcg.h"

namespace asr
{

class pcg_engine
{
public:
    constexpr pcg_engine() noexcept
        : m_state {100, 100}
    {
    }

    [[nodiscard]] constexpr uint32_t generate() noexcept
    {
        return detail::pcg32_random_r(m_state);
    }

    template <uint32_t Bound>
    [[nodiscard]] ASR_FORCEINLINE constexpr uint32_t generate_bounded() noexcept
    {
        return detail::pcg32_boundedrand_r(m_state, Bound);
    }
private:
    detail::pcg32_random_t m_state;
};

[[nodiscard]] constexpr bool flip_fair_coin(pcg_engine& engine) noexcept
{
    return !!engine.generate_bounded<2>();
}

} // namespace asr
