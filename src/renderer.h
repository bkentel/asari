#pragma once

#include <memory>

namespace asr
{

class renderer_backend
{
public:
    virtual ~renderer_backend();

    virtual void initialize() = 0;

    virtual void render_frame() = 0;
};

struct renderer_options
{
    renderer_backend* native_target;
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
