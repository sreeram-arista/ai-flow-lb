set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
add_library(nccl_flow_export_v1 SHARED nccl-export.cpp)
