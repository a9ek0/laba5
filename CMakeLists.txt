cmake_minimum_required(VERSION 3.24)
project(laba5)

set(CMAKE_C_STANDARD 99)

add_executable(laba5 5laba/main.c 5laba/lyb/menu.c 5laba/lyb/cache.c 5laba/lyb/checks.c 5laba/lyb/memory.c 5laba/lyb/other.c)
