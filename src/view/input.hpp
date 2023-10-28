/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-13
 *==========================*/

#ifndef PXL_VIEW_INPUT_HPP
#define PXL_VIEW_INPUT_HPP

#include "types.hpp"

// TODO: Changeable depending on the frontend
#include "SDL_keycode.h"

namespace input {

// TODO: Changeable depending on the frontend
enum Keycode : u32 {
  A = SDLK_a,
  B = SDLK_b,
  C = SDLK_c,
  D = SDLK_d,
  E = SDLK_e,
  F = SDLK_f,
  G = SDLK_g,
  H = SDLK_h,
  I = SDLK_i,
  J = SDLK_j,
  K = SDLK_k,
  L = SDLK_l,
  M = SDLK_m,
  N = SDLK_n,
  O = SDLK_o,
  P = SDLK_p,
  Q = SDLK_q,
  R = SDLK_r,
  S = SDLK_s,
  T = SDLK_t,
  U = SDLK_u,
  V = SDLK_v,
  W = SDLK_w,
  X = SDLK_x,
  Y = SDLK_y,
  Z = SDLK_z,

  // MODIFIERS
  CTRL = 1 << 29,
  SHIFT = 1 << 28,
  ALT = 1 << 27,

  MOD_MASK = 0b111 << 27,
  KEY_MASK = ~MOD_MASK,
};

enum BtnMask : u32 {
  STATES = 0x0000'0003,
  HIGHLIGHT = 0x0000'0004,
  FOCUS = 0x0000'0008,
  THEME = 0xffff'fff0,
};

enum BtnState : u32 {
  DISABLED = 0b00,
  NORMAL = 0b01, // button state
  HOVER = 0b10,
  DOWN = 0b11,
};

enum BtnTheme : u32 {
  CUSTOM = 0U,
  TOOL_BTN = 0x10 * 1U,
};

enum class MouseType : u16 { LEFT, MIDDLE, RIGHT };

enum class MouseState : u16 {
  NONE,
  DOWN, // Mouse is just being clicked
  HOLD, // Holding the mouse btn down
  UP    // Mouse Released
};

struct KeyMod {
  bool ctrl = false;
  bool shift = false;
  bool alt = false;
};

} // namespace input

#endif

