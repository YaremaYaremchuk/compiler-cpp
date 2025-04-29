cmake_minimum_required(VERSION 3.20)


project(compiler)


set(CMAKE_CXX_STANDARD 20)


add_executable(comp src/main.cpp
        src/tokeniser.h
        src/parser.h
        src/gencode.h
        src/arenaloc.h)