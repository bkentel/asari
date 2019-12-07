#include "renderer.h"

#include "win_common.h"

#include <memory>

namespace asr::win
{

[[nodiscard]] std::unique_ptr<renderer_backend> make_direct2d_renderer(HWND target);

} // namespace asr::win
