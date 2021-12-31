cmake_minimum_required(VERSION 3.20)
project(kmeans_pp C)
add_executable(kmeans_pp capi_kmeans_pp.c)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(kmeans_pp ${PYTHON_LIBRARIES})
