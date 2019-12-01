#pragma once

#include <memory>

namespace asr
{

struct renderer_options
{
};

class renderer
{
public:
    virtual ~renderer();

    virtual void stop() noexcept = 0;
};

[[nodiscard]] std::unique_ptr<renderer> make_renderer(
    renderer_options options);

} // namespace asr
