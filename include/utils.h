#include "board.h"

static std::map<std::string, direction> m = {
    {"N", direction::N}, {"NE", direction::NE}, {"SE", direction::SE},
    {"S", direction::S}, {"SW", direction::SW}, {"NW", direction::NW}};
