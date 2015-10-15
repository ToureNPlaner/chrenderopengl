#include <vector>
#include <utility>
#include "graph_basics.h"

struct Core {
  Core(Draw&& draw_in) : draw(std::move(draw_in)) {}
  Core(const Draw& draw_in) : draw(draw_in) {}
  Draw draw;
};
