set(CMAKE_C_FLAGS "-O3 -fPIC")
set(CMAKE_CXX_FLAGS "-O3 -fPIC")

include_directories(
    "${workspace_dir}/"
)

file(GLOB_RECURSE common_source
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/log/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/base64/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/csmtp/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/json/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/self/*.cc"

)

add_library(commonlib STATIC ${common_source})
target_link_libraries(commonlib 
    ssl
    crypto
)

file(GLOB_RECURSE test_source
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc"
)

set(shared_libs pthread gmock gtest gtest_main gmock_main commonlib)

foreach(test_source ${test_source})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} ${shared_libs})
endforeach()
