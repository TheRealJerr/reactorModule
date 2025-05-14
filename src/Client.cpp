#include "../include/Client.h"


namespace reactor
{
    // 针对static的callback进行申明
    onMessageCallBack Client::_cb;
    void Client::thraedHandlerMsg()
    {
        
        while(true)
        {
                std::string buffer;
            _con->readMsg(&buffer);
            if(_cb)
                _cb(_con,buffer);
        }
        
    }
}