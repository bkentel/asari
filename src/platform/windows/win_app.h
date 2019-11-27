#pragma once

#include "platform/app.h"

#include "win_common.h"

#include <exception>
#include <memory>
#include <optional>

namespace asr::win
{

class text_input_manager;

class application final
    : public asr::application
{
public:
    explicit application(application_options options);
private:
    void init(HWND window) noexcept;

    LRESULT window_proc(
          UINT   message
        , WPARAM wParam
        , LPARAM lParam) noexcept;

    template <UINT Message>
    std::optional<LRESULT> window_proc(
          WPARAM wParam
        , LPARAM lParam);
private:
    static application& get_window_instance(
        application* const value = nullptr) noexcept;

    static LRESULT __stdcall top_level_window_proc(
          HWND   hWnd
        , UINT   message
        , WPARAM wParam
        , LPARAM lParam) noexcept;

private:
    std::exception_ptr  m_window_proc_error;
    text_input_manager* m_input_manager = nullptr;
    HWND                m_window        = nullptr;
    bool                m_initialized   = false;
};

} // namespace asr::win
