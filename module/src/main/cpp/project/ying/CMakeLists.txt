cmake_minimum_required(VERSION 3.20)
project("ying" VERSION 1.0 LANGUAGES CXX)
set(library_name "main")
# add_definitions(-DkNO_KEYSTONE)
# file(GLOB source "../KittyMemory/*.cpp")
add_compile_options(-fdeclspec)
add_library(${library_name} SHARED main.cpp "${source}")
target_link_libraries(
${library_name}
PUBLIC
KittyMemory
keystone
log
dobby
)
set_target_properties(${library_name} PROPERTIES
LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/ying"
)
