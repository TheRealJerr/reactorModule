#include "Common.h"

// 自定义缓冲区
// 待完善


namespace reactor
{
    // 内存采用队列的方式
    class Buffer
    {
    public:
        uint32_t readInt32();

        std::string readBuffer();


    private:
        std::queue<char> _buffer;
    };
}