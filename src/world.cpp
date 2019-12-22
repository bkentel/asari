#include "world.h"

#include "common.h"

namespace asr
{

world::~world() = default;

namespace
{

class world_impl final : public world
{
public:
    world_impl()
    {
    }

    std::vector<int> const& terrain(rect_u32 const& region) const noexcept override
    {
        ASR_ASSERT(region.p1.x >= region.p0.x);
        ASR_ASSERT(region.p1.y >= region.p0.y);

        auto const w = region.p1.x - region.p0.x;
        auto const h = region.p1.y - region.p0.y;

        m_last_terrain_region = region;
        m_last_terrain.resize(w * h);

        return m_last_terrain;
    }
private:
    mutable std::vector<int> m_last_terrain;
    mutable rect_u32         m_last_terrain_region;
};

} // namespace

std::unique_ptr<world> make_world()
{
    return std::make_unique<world_impl>();
}

} // namespace asr
