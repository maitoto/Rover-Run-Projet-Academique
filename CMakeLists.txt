cmake_minimum_required(VERSION 3.27)
project(roverrun C)

set(CMAKE_C_STANDARD 11)

add_executable(roverrun
        main.c
        loc.c
        loc.h
        moves.c
        moves.h
        map.c
        map.h
        queue.c
        queue.h
        stack.c
        stack.h
        tree.c
        tree.h
        phase.c
        phase.h
)