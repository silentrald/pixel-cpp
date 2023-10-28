/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-07-27
 *===============================*/

#ifndef PXL_TOOL_UTILS_HPP
#define PXL_TOOL_UTILS_HPP

#include "../draw/layer.hpp"
#include "types.hpp"
#include <vector>

#include "view/sdl3/texture.hpp"
using Texture = view::sdl3::Texture;

namespace tool::utils {

/**
 * Refer: https://en.wikipedia.org/wiki/Bresenham's_line_algorithm#All_cases
 *
 * @param layer - nullable, if texture is the only thing needs to be updated
 * @param texture
 * @param size - size of the layer/texture
 * @param start - point where to start to draw the line
 * @param end - point where to end to draw the line
 * @param color - what color to paint on the layer/texture
 * @param mask - allowed pixel position to draw on the layer/texture
 **/
void draw_line(
    draw::Layer* layer, Texture& texture, ivec size, ivec start, ivec end,
    rgba8 color, const std::vector<bool>& mask
) noexcept;

} // namespace tool::utils

#endif

