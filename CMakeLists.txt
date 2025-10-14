# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(RIBAST)

# 设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加OpenXLSX子目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

add_subdirectory(src/tools/OpenXLSX)

# 定义通用的源文件集合，避免重复
set(COMMON_SOURCES
    src/globalParams.cpp
    src/operator.cpp
    src/dataLoader/excelLoader.cpp
    src/dataLoader/operatorData.cpp
    src/dataLoader/skillData.cpp
    src/dataLoader/playerOperatorData.cpp
    src/skillList/mfg.cpp
    src/skillList/skillTemplates.cpp
    src/skillList/trade.cpp
    src/skillList/others.cpp
)

# 为所有目标设置宏定义
function(setup_target target_name)
    target_compile_definitions(${target_name} PRIVATE "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
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