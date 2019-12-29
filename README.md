# First Person Shooter Game

Work in progress...

## Cloning the project

This project uses *git submodules*, so when you are cloning the project clone it with the following command:\
`git clone --recurse-submodules git@github.com:ivankatalenic/fps-game.git`

The submodules the project uses are:
- [glfw](https://github.com/glfw/glfw)
- [assimp](https://github.com/assimp/assimp)
- [glm](https://github.com/g-truc/glm)
- [stb](https://github.com/nothings/stb)

## Building the project

To successfully build this project you must have a modern C++ compiler, *cmake*, and *make* programs.

Open a terminal, and set your working directory to *fps-game*.

### On Linux

This project was built, and tested on *Linux Mint* with *gcc-8*.

Steps for building the project:
1. `cmake .`
2. `make -j <number-of-cores>`
3. `./thegame`

### On Windows

This project was built, and tested on *Windows 10* with [MSYS2's](https://www.msys2.org/) *gcc 9.2.0*.

Steps for building the project with *MSYS2*:
1. `cmake -G "MSYS Makefiles" .`
2. `make -j <number-of-cores>`
3. `thegame`
