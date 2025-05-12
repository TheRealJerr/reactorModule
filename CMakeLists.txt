# 指定CMake最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(MyProject
    VERSION 1.0.0
    LANGUAGES CXX
)

# 设置C++标准 (C++11/14/17/20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 如果项目需要C++20模块支持
# set(CMAKE_CXX20_MODULES ON)

# 递归收集所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)
# 设置头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# 如果需要链接库
# find_package(SomeLibrary REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE SomeLibrary::SomeLibrary)

# 添加编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

# 安装规则 (可选)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)