# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(RIBAST)

# 设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义项目包含目录 - 统一头文件管理
set(PROJECT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/dataStructure
    ${CMAKE_SOURCE_DIR}/src/dataLoader
    ${CMAKE_SOURCE_DIR}/src/skillList
    ${CMAKE_SOURCE_DIR}/src/tools
)

# 添加OpenXLSX子目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

add_subdirectory(src/tools/OpenXLSX)

# 定义通用的源文件集合，避免重复
set(COMMON_SOURCES
    src/dataStructure/globalParams.cpp
    src/dataStructure/operator.cpp
    src/dataLoader/excelLoader.cpp
    src/dataLoader/operatorData.cpp
    src/dataLoader/skillData.cpp
    src/dataLoader/playerOperatorData.cpp
    src/skillList/mfg.cpp
    src/skillList/skillTemplates.cpp
    src/skillList/trade.cpp
    src/skillList/others.cpp
)

# 为所有目标设置宏定义和包含目录
function(setup_target target_name)
    # 设置项目源码路径宏
    target_compile_definitions(${target_name} PRIVATE "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
    
    # 统一设置包含目录
    target_include_directories(${target_name} PRIVATE ${PROJECT_INCLUDE_DIRS})
    
    # 链接 OpenXLSX 库
    if(TARGET OpenXLSX::OpenXLSX)
        target_link_libraries(${target_name} PRIVATE OpenXLSX::OpenXLSX)
    endif()
endfunction()

# 主程序可执行文件
# add_executable(RIBAST
#     ${COMMON_SOURCES}
#     src/main.cpp  # 当有主程序时取消注释
# )
# setup_target(RIBAST)

# 测试程序可执行文件
# 数据加载器测试
add_executable(dataLoaderTest
    ${COMMON_SOURCES}
    src/test/dataLoaderTest.cpp
)
setup_target(dataLoaderTest)

add_executable(facilityTest
    ${COMMON_SOURCES}
    src/test/facilityTest.cpp
)
setup_target(facilityTest)

add_executable(facilitySpecTest
    ${COMMON_SOURCES}
    src/test/facilitySpecTest.cpp
)
setup_target(facilitySpecTest)

# Excel读取示例程序
add_executable(excelTest
    ${COMMON_SOURCES}
    src/test/excelTest.cpp
)
setup_target(excelTest)

# 如果有其他测试文件，可以按相同模式添加
# add_executable(otherTest
#     ${COMMON_SOURCES}
#     src/test/otherTest.cpp
# )
# setup_target(otherTest)

# =============================================================================
# 头文件管理辅助函数
# =============================================================================

# 添加新的包含目录
function(add_include_directory dir_path)
    list(APPEND PROJECT_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${dir_path})
    set(PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} PARENT_SCOPE)
    message(STATUS "添加包含目录: ${CMAKE_SOURCE_DIR}/${dir_path}")
endfunction()

# 为特定目标添加额外的包含目录
function(add_target_include target_name dir_path)
    target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/${dir_path})
    message(STATUS "为 ${target_name} 添加包含目录: ${CMAKE_SOURCE_DIR}/${dir_path}")
endfunction()

# 打印当前所有包含目录（调试用）
function(print_include_directories)
    message(STATUS "当前项目包含目录:")
    foreach(dir ${PROJECT_INCLUDE_DIRS})
        message(STATUS "  - ${dir}")
    endforeach()
endfunction()

# 可以调用此函数查看当前包含目录
# print_include_directories()