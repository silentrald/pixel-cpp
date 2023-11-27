/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-04
 *==========================*/

#include "./presenter.hpp"
#include "core/cfg/shortcut.hpp"
#include "core/file/png.hpp"
#include "core/file/pxl.hpp"
#include "core/history/caretaker.hpp"
#include "core/tool/enum.hpp"
#include "core/tool/eraser.hpp"
#include "core/tool/fill.hpp"
#include "core/tool/line.hpp"
#include "core/tool/pan.hpp"
#include "core/tool/pencil.hpp"
#include "core/tool/select.hpp"
#include "core/tool/zoom.hpp"
#include "model/model.hpp"

Model model_{};
View view_{};

tool::Pencil pencil_{};
tool::Eraser eraser_{};
tool::Line line_{};
tool::Fill fill_{};
tool::Select select_{};

tool::Pan pan_{};
tool::Zoom zoom_{};

cfg::Shortcut shortcut_{};

history::Caretaker caretaker_{};

file::Pxl pxl_{};
file::Png png_{};

