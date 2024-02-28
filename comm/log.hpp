#pragma once

#include <iostream>
#include <string>
#include "util.hpp"

using std::string;
using namespace ns_util;

namespace ns_log {
    enum {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    class Log {
    public:
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
    #define LOG(level) ns_log::Log::log(#level, __FILE__, __LINE__)
}