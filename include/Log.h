
#pragma once
// 定义日志的文件
#include <iostream>
#include <chrono>
#include <sstream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <memory>

namespace reactor
{
    namespace fs = std::filesystem;

    // 定义配置信息

    const fs::path gpath = "./log";

    const fs::path gfilename = "log.txt";



    enum class LogLevel
    {
        DEBUG,
        WARNING,
        CRITIC,
        ERROR,
        FATAL,
        INFO,
    };

    class LevelToStringHelper
    {
    public:
        static std::string LevelToString(LogLevel level)
        {
            switch (level) 
            {
                case LogLevel::DEBUG : return "DEBUG"; break;
                case LogLevel::WARNING : return "WARNING"; break;
                case LogLevel::CRITIC : return "CRITIC"; break;

                case LogLevel::ERROR : return "ERROR"; break;
                case LogLevel::FATAL : return "FATAL"; break;
                case LogLevel::INFO : return "INFO"; break;
                default : return "NOT_KOWN"; break;
            }    
        }

    };
    class Timer
    {
    public:
        static std::string getCurTime()
        {
            auto now = std::chrono::system_clock::now();
    
            // 转换为时间戳（自 1970-01-01 的秒数）
            auto timestamp = std::chrono::system_clock::to_time_t(now);
            
            std::stringstream ssm;
            // 转换为本地时间字符串
            ssm << std::ctime(&timestamp);
            return ssm.str().substr(0,ssm.str().size() - 1);
        } 
    };
    // 日期的协议
    // [日期][日志级别][filename][line]: 错误信息
    // 指定公共的标准的接口
    class Log
    {
    public:
        
        virtual void Write(const std::string&) = 0;

    };

    class LogFile : public Log
    {
    public:
        LogFile(const std::string& filename = gfilename) : _filename(filename)
        {
            // 初始化fstream
            if(!fs::exists(gpath))
            {
                if(fs::create_directory(gpath) == false) 
                {
                    std::cout << gpath << "创建失败" << std::endl;
                    ::exit(1);
                }
            }
            const std::string path = gpath / gfilename;
            _ofs.open(path,std::ios::trunc);

            if(!_ofs.is_open())
            {
                std::cerr << "open " << path << " fail!" << std::endl;
                ::exit(1); 
            }
        }
        
        virtual void Write(const std::string& msg) override 
        {
            std::unique_lock<std::mutex> lock(_mtx);
            
            _ofs << msg << std::endl;
        }
        

    private:
        const std::string& _filename;
        std::ofstream _ofs;
        std::mutex _mtx;
    };

    class LogScreen : public Log
    {
    public:
        virtual void Write(const std::string& msg) override 
        {
            std::unique_lock<std::mutex> lock(_mtx);

            std::cout << msg << std::endl;
        }
    private:
        std::mutex _mtx;
    };

    enum class DstFile
    {
        SCREEN,
        FILE,
    };
    // 针对上面类型的使用
    // 通过单例模式来构架LogTool
    class LogTool
    {
    private:
        LogTool(const LogTool&) = delete;


    public:
        LogTool() = default;

        static void setDstFile(DstFile df) { _desfile = df; }
        
        static std::unique_ptr<LogTool>& getInstance()
        {
            if(_self.get() == nullptr)
            {
                _self = std::make_unique<LogTool>();
                if(_desfile == DstFile::SCREEN) _self->ChgToScreen();
                else _self->ChgToFile();
            }

            return _self;
        }

        void ChgToScreen()
        {
            _write_tool = std::make_unique<LogScreen>();
        }

        void ChgToFile()
        {
            _write_tool = std::make_unique<LogFile>();
        }

        void WriteMsg(const std::string& msg)
        {
            _write_tool->Write(msg);
        }

    private:
        static inline DstFile _desfile = DstFile::SCREEN; // 默认是打印在显示屏失眠

        std::unique_ptr<Log> _write_tool; // 写入的工具

        static inline std::unique_ptr<LogTool> _self = nullptr;
    };


    // 组织想要写入的信息

    class LogInfo
    {
    public:
        LogInfo(LogLevel level,const std::string& filename,int line)
        {
            _ssm << "[" << Timer::getCurTime() << "]" \
                << "[" << LevelToStringHelper::LevelToString(level) << "]" \
                << "[" << filename << "]" \
                << "[" << line << "]:";
        }

        template <class T>
        LogInfo& operator<<(T&& msg)
        {
            _ssm << msg;
            return *this;
        }

        ~LogInfo()
        {
            LogTool::getInstance()->WriteMsg(_ssm.str());
        }
    private:

        std::stringstream _ssm;
    };


#define log(level) LogInfo(level,__FILE__,__LINE__)


}

#define ENABLE_LOG_FILE() reactor::LogTool::setDstFile(reactor::DstFile::FILE)
#define ENABLE_LOG_SCREEN() reactor::LogTool::setDstFile(reactor::DstFile::SCREEN)
