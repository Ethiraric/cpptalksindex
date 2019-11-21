cmake_minimum_required(VERSION 2.6)

#sources files
set(SRCS
  main.cpp
)

#binary
add_executable(tests ${SRCS})
target_include_directories(tests PRIVATE .)
