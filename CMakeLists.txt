cmake_minimum_required(VERSION 3.10)
project(botDaru)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(botDaru
    src/main.cc
    src/uci_loop.cc
    src/chess_board.cc
)
