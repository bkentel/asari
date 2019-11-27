#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <string_view>

namespace asr
{

class input_manager;

class application
{
public:
    virtual ~application();
};

struct application_options
{
    std::function<void ()> on_ready;

    input_manager* ime;
};

[[nodiscard]] std::unique_ptr<application> make_application(
    application_options options);

void intial_setup();

[[nodiscard]] int main(std::vector<std::string_view> const& args);

} // namespace asr
