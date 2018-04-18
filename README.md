# Gipf AI

An agent to play the board game [Gipf](http://www.gipf.com/gipf/index.html).

## Build Instructions

1. Get a C++ compiler and Python.
2. Install CMake, SWIG and Boost.
3. Clone this repository recursively with `git clone --recursive https://github.com/SooryaN/gipf-ai.git`.
4. `cd` to the project root and `mkdir build && cd build`.
5. `cmake ..`
6. `make`
7. Run `./simulator` to view the AI playing against itself. `gipf.py` and `_gipf.so` are Python bindings for the simulator.
