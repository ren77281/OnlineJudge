#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include "util.hpp"

/**
* @file log.hpp
* @brief 日志模块
*
* 简单地将日志打印到标准输出中
*
* @author ren77281
* @date 2024-04-10
*/

namespace ns_log {
    using std::string;
    using namespace ns_util;
    enum {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    /**
    * @class Log
    * @brief 日志输出器
    */
    class Log {
    public:
        /**
        * @brief 根据日志等级输出日志
        * @param[in] level 日志等级
        * @param[in] fileName 输出日志的文件名
        * @param[in] line 所在文件的行数
        * @return 为支持'<<'的链式调用，返回输出流对象
        * 如：log("INFO", "main.cpp", 10) << "This is an info message." << std::endl;
        */
        static std::ostream& log(const string& level, const string& fileName, int line) {
            string str;
            // 等级
            str += "[", str += level, str += "]";
            // 文件名
            str += "[", str += fileName, str += "]";
            // 时间戳
            str += "[", str += TimeUtil::GetTimeStamp(), str += "]";
            // 行数
            str += "[", str += std::to_string(line), str += "]";
            
            std::cout << str;
            return std::cout;
        }
    };
    /**
    * @brief 对log函数的宏定义
    * @param[in] level 日志等级
    * 相比于直接使用log函数，LOG宏定义只需要传入日志等级
    */
    #define LOG(level) ns_log::Log::log(#level, __FILE__, __LINE__)
}

#endif