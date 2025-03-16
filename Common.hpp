// Common.hpp的作用是使用一些常见的宏定义，比如DEBUG宏，用于输出调试信息。
#pragma once
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
// 将文件描述符设计成非阻塞

#define NON_BLOCKING(fd) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK)

