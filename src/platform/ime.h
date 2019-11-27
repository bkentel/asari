#pragma once

#include <memory>

namespace asr
{

class input_manager
{
public:
    virtual ~input_manager();
};

[[nodiscard]] std::unique_ptr<input_manager> make_input_manager();

} // namespace asr
