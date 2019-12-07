#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <string_view>
#include <variant>

namespace asr
{

class input_manager;
class renderer_backend;

struct mouse_event
{
    int x;
    int y;
    int dx;
    int dy;
};

struct button_event
{
    int id;
    int state;
    int modifiers;
};

struct keyboard_event
{
    int id;
    int state;
    int modifiers;
    int character;
};

using system_event = std::variant<
      std::monostate
    , mouse_event
    , button_event
    , keyboard_event>;

class application_observer
{
public:
    virtual ~application_observer();

    //! called when the application has finished initializing
    virtual void on_ready() noexcept = 0;

    virtual void on_system_event(system_event const& event) noexcept = 0;
};

class application
{
public:
    virtual ~application();

    virtual void start() = 0;

    virtual renderer_backend& get_renderer_backend() const noexcept = 0;
};

struct application_options
{
    application_observer* observer; //!< not null
    input_manager*        ime;      //!< not null
};

[[nodiscard]] std::unique_ptr<application> make_application(
    application_options options);

void intial_setup();

[[nodiscard]] int main(std::vector<std::string_view> const& args);

} // namespace asr
