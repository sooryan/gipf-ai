#pragma once

/**
       N
  SW   |   NE
     >-o-<
  NW   |   SE
       S
*/


enum class direction {
	N,
	S,
	NE,
	NW,
	SE,
	SW,
};


std::istream &operator>>(std::istream &is, direction &i) {
	int tmp;
	if (is >> tmp)
		i = static_cast<direction>(tmp);
	return is;
}
