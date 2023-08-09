cmake_minimum_required(VERSION 3.0.0)
project(ai-flow-lb VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src/include)
add_library(nccl_flow_export_v1 SHARED src/nccl-export.cpp)
