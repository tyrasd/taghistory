cmake_minimum_required(VERSION 3.5)

project(taghistory) 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Osmium REQUIRED COMPONENTS io pbf)
include_directories(${OSMIUM_INCLUDE_DIRS})

ADD_SUBDIRECTORY(backend)