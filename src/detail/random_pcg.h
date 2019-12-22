#include "config.h"

#include <cstdint>

namespace asr::detail
{

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

struct pcg32_random_t
{
    uint64_t state;
    uint64_t inc;
};

[[nodiscard]] ASR_FORCEINLINE constexpr uint32_t pcg32_random_r(pcg32_random_t& rng) noexcept
{
    auto const oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 6364136223846793005ull + (rng.inc | 1u);
    // Calculate output function (XSH RR), uses old state for max ILP
    auto const xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
    auto const rot        = static_cast<uint32_t>(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31u));
}

// pcg32_boundedrand(bound):
// pcg32_boundedrand_r(rng, bound):
//     Generate a uniformly distributed number, r, where 0 <= r < bound

[[nodiscard]] ASR_FORCEINLINE constexpr uint32_t pcg32_boundedrand_r(pcg32_random_t& rng, uint32_t const bound) noexcept
{
    // To avoid bias, we need to make the range of the RNG a multiple of
    // bound, which we do by dropping output less than a threshold.
    // A naive scheme to calculate the threshold would be to do
    //
    //     uint32_t threshold = 0x100000000ull % bound;
    //
    // but 64-bit div/mod is slower than 32-bit div/mod (especially on
    // 32-bit platforms).  In essence, we do
    //
    //     uint32_t threshold = (0x100000000ull-bound) % bound;
    //
    // because this version will calculate the same modulus, but the LHS
    // value is less than 2^32.

    auto const threshold =( ~bound + 1u) % bound;

    // Uniformity guarantees that this loop will terminate.  In practice, it
    // should usually terminate quickly; on average (assuming all bounds are
    // equally likely), 82.25% of the time, we can expect it to require just
    // one iteration.  In the worst case, someone passes a bound of 2^31 + 1
    // (i.e., 2147483649), which invalidates almost 50% of the range.  In
    // practice, bounds are typically small and only a tiny amount of the range
    // is eliminated.
    for (;;)
    {
        auto const r = pcg32_random_r(rng);
        if (r >= threshold)
        {
            return r % bound;
        }
    }
}

} // namespace asr::detail
