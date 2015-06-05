cmake_minimum_required(VERSION 2.8)

project(beka)

set(CMAKE_CXX_FLAGS "-std=c++11")
set(sources 
	"src/core.cpp"
)

add_executable(beka ${sources} "src/client.cpp" "beka.cpp")
target_link_libraries(beka readline)
add_executable(bekad ${sources} "src/server.cpp" "bekad.cpp")
include_directories("./include/")
