#pragma once

#include <memory>

namespace asr
{

struct engine_options
{
};

class engine
{
public:
    virtual ~engine();

    virtual void stop() = 0;
};

[[nodiscard]] std::unique_ptr<engine> make_engine(
    engine_options options);

} // namespace asr
