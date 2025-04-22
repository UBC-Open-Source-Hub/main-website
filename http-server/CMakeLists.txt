cmake_minimum_required(VERSION 3.15...4.0)

set(EXEC_NAME "server")

project(Server VERSION 1.0
               DESCRIPTION "A HTTP Server"
               LANGUAGES CXX)

# Compile the supporting libraries
add_library(utils STATIC utils/utils.cpp utils/utils.hpp)
target_include_directories(utils PUBLIC utils)
target_compile_features(utils PUBLIC cxx_std_20)

# Compile the main server program
add_executable(server
   server.cpp
   serverModelSimple.cpp)
set_target_properties(server PROPERTIES
   CXX_STANDARD 20
   CXX_STANDARD_REQUIRED YES
   CXX_EXTENSIONS NO)
target_include_directories(server PUBLIC inc)
target_link_libraries(server PUBLIC utils)
