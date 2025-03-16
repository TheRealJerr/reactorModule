// 通过epoll实现多路转接
#include "Socket.hpp"
#include "epollClass.hpp"
#include <unordered_map>
#include "connection.hpp"
namespace epollServerModule
{
    using namespace epollClassModule;
    using namespace SocketModule;
    using namespace ConnectionModule;

    const int gdefault_epollevents_size = 64;

    using connection_t = std::unique_ptr<Connection>;
    class epollServer
    {
        bool isFdInConnections(int fd) const { return _connections.count(fd); }   
    public:
        epollServer(uint16_t port,int size = gdefault_epollevents_size):
            _maxNum(size),
            _is_running(true),
            _port(port),_socket(std::make_unique<TCPSocket>(port))
        {
            // 动态开辟
            _epoll_buffer = new struct epoll_event[_maxNum];
            if(_epoll_buffer == nullptr)
            {
                LOG(LogLevel::ERROR) << "new Epoll Buffer Error:" << strerror(errno);
                exit(1);
            }
            //
            _socket->createTCPSocket();
            _sockfd = _socket->getSocket();
            // 将套接字信息写入epoll模型
            _epollModule.insertFd(_sockfd,EPOLLIN);
            // 构造connetion写入 con = std::make_unique<Connection>(_sockfd);
            connection_t u_con = std::make_unique<Connection>(_sockfd);
            
            u_con->initCallFunc
            (
                [this]()
                {
                    // 得到读写套接字
                    int sock_com = this->_socket->accept();
                    // 
                    this->_epollModule.insertFd(sock_com,EPOLLIN);
                    // 构建新的连接
                    connection_t newcon = std::make_unique<Connection>(sock_com);
                    // 连接是全双工的
                    newcon->initCallFunc
                    (
                        [&newcon]()
                        {
                            char buffer[1024] = { 0 };
                            int n = ::recv(newcon->Fd(),buffer,1024,0);
                            if(n < 0)
                            {
                                LOG(LogLevel::ERROR) << "read error:" << strerror(errno);
                                return;
                            }
                            buffer[n] = 0;
                            // 将数据读到缓冲区
                            newcon->buffer() = buffer;
                            // 读完之后处理字符串
                            // verison1 不做处理,直接返回
                            // handler_request(buffer)
                            
                        },
                        nullptr,
                        nullptr
                    );
                },
                nullptr,
                nullptr
            );
            // 填充读写信息
            // unique_ptr禁止拷贝
            _connections.insert(std::make_pair(_sockfd,std::move(u_con)));
        }


        void loop()
        {
            while(_is_running)
            {
                // epoll模型等待
                int n =  _epollModule.Wait(_epoll_buffer,_maxNum);
                if(n < 0)
                {
                        LOG(LogLevel::ERROR) << "epoll wait error:" << strerror(errno);
                        exit(1);
                }
                
                // 遍历事件
                for(int i = 0;i < n;i++)
                {
                    int fd = _epoll_buffer[i].data.fd;
                    uint32_t event = _epoll_buffer[i].events;
                    if(event & EPOLLERR || event & EPOLLHUP)
                        event = EPOLLIN | EPOLLOUT;
                    if(event & EPOLLIN)
                    {
                        // 读事件就绪
                        _epollModule.insertFd(fd,EPOLL_CTL_DEL);
                        if(isFdInConnections(fd))
                        {
                            _connections[i]->recv();
                        }
                    }   
                    if(event & EPOLLOUT)
                    {
                        // 写事件就绪
                        if(isFdInConnections(fd))
                        {
                            _connections[i]->sendto();
                        }
                    }
                }
            }
        }

        bool insertFd(int fd,uint32_t cmd)
        {
            return _epollModule.insertFd(fd,cmd);
        }
        void stopServer()
        {
            _is_running = false;
        }
    private:
        // 服务器运行状态
        bool _is_running;
        // 绑定的端口号
        uint16_t _port;
        // 绑定的套接字
        int _sockfd;
        // epoll模型 
        epollClass _epollModule;
        // 创建tcp套接字
        std::unique_ptr<Socket> _socket;
        // 通过fd-connection 存储连接信息
        std::unordered_map<int,connection_t> _connections; // 连接表
        // 事件区间
        int _maxNum = 64;
        struct epoll_event* _epoll_buffer = nullptr;
    };
}