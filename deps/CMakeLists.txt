add_subdirectory(expected)
add_subdirectory(GSL)
add_subdirectory(json)

add_library(range-v3 INTERFACE)
target_include_directories(range-v3 SYSTEM INTERFACE range-v3/include)
