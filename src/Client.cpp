#include "../include/Client.h"


namespace reactor
{
    // 针对static的callback进行申明
    onMessageCallBack Client::_cb;
    void Client::thraedHandlerMsg()
    {
        // 将通信连接设置成为阻塞   
        if(_con.get()) _con->setBlock(); // 将连接设置成为阻塞
        while(true)
        {
            std::string buffer;
            _con->readMsg(&buffer);
            if(_cb)
                _cb(_con,buffer);
        }
        
    }
}