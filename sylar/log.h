#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <fstream>
#include <list>
#include <vector>

namespace sylar{
    
//log's event
class LogEvent{
public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();
private:
        const char* m_file=nullptr; //file name
        uint32_t m_line=0;          //line number
        uint32_t m_elapse=0;        //The ms number of process run time 
        uint32_t m_threadId=0;      //thread id
        uint32_t m_fiberId=0;       //fiber id
        uint64_t m_time;            //timeval
        std::string m_content;
};

//日志级别
class LogLevel{
public:
    enum Level {
        DEBUG=1,
        INFO=2,
        WARN=3,
        ERROR=4,
        FATAL=5
    };
};

class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
        
        //
    std::string format(LogEvent::ptr event);
private:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem(){}
        virtual std::string format(LogEvent::ptr event)=0;
    };

    virtual void init();

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}

    virtual void log(LogLevel::Level level,LogEvent::ptr event)=0;
    void setFormatter(LogFormatter::ptr val) {m_formatter=val;}
    LogFormatter::ptr getFormatter(){return m_formatter;}
protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};

//日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name="root");
    void log(LogLevel::Level level,LogEvent::ptr event);
        
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel()const {return m_level;}
    void setLevel(LogLevel::Level val) {m_level=val;}
private:
    std::string m_name;                 //log name
    LogLevel::Level m_level;            //log level
    std::list<LogAppender::ptr> m_appenders;        //Set of Appender
};

//输出到控制台的appender
class StdoutLogAppender:public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(LogLevel::Level,LogEvent::ptr event) override;

};

//output log to file
class FileLogAppender:public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(LogLevel::Level level,LogEvent::ptr event) override;

    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}// namespace sylar

#endif