cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "gstreamer-app-boilerplate")
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} main.cpp producer.cpp consumer.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Find Gstreamer
find_package(PkgConfig)
pkg_check_modules(Gstreamer REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-pbutils-1.0)
target_include_directories(${PROJECT_NAME} PUBLIC ${Gstreamer_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${Gstreamer_LIBRARIES})
