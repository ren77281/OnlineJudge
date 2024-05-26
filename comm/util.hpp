#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <boost/algorithm/string.hpp>

/**
* @file util.hpp
* @brief 将一些常用操作封装为接口
*/

namespace ns_util {
    using std::string;
    using std::vector;
    // 静态定义hpp文件中的变量，否则可能导致重复定义
    static string code_path = "./code/";
    /**
    * @class PathUtil
    * @brief 文件路径相关的类
    */
    class PathUtil {
    public:
        /**
        * @brief 为字符串添加后缀
        *
        * 先添加前缀，再添加后缀。前缀的值为code_path变量
        *
        * @param[in] fileName 需要添加后缀的字符串
        * @param[in] suffix 后缀
        * @return 添加了后缀的字符串
        */
        static string AddSuffix(const string& fileName, const string& suffix) {
            string res = code_path;
            res += fileName;
            res += suffix;
            return res;
        }
        /**
        * 以下函数都是AddSuffix的复用，区别在于每个函数的suffix不同
        */
        static string Cpp(const string& fileName) {
            return AddSuffix(fileName, ".cpp");
        }
        static string Exe(const string& fileName) {
            return AddSuffix(fileName, ".exe");
        }
        static string CompileError(const string& fileName) { 
            return AddSuffix(fileName, ".comerr");
        }
        static string In(const string& fileName) { 
            return AddSuffix(fileName, ".in");
        }
        static string Out(const string& fileName) { 
            return AddSuffix(fileName, ".out");
        }
        static string RunError(const string& fileName) { 
            return AddSuffix(fileName, ".runerr");
        }
    };
    
    /**
    * @class TimeUtil
    * @brief 与时间相关的类
    */
    class TimeUtil {
    public:
        /**
        * @brief 获取毫秒级别的精度
        * @return 表示时间的字符串
        */
        static string GetMsTime() {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return std::to_string(tv.tv_sec * 1000l + tv.tv_usec / 1000);
        }
        /**
        * @brief 获取时间戳
        * @return 表示时间戳的字符串
        */
        static string GetTimeStamp() {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return std::to_string(tv.tv_sec);
        }
    };

    /**
    * @class FileUtil
    * @brief 文件操作相关的类
    */
    class FileUtil {
    public:
        /**
        * @brief 判断文件是否存在
        * @param[in] fileName 带路径的文件名
        * @return true/false分别表示文件存在/不存在
        */
        static bool IsExist(const string& fileName) {
            struct stat statbuf;
            if (stat(fileName.c_str(), &statbuf) < 0) {
                return false;
            }
            return true;
        }
        /**
        * @brief 读取文件内容
        * @param[in] fileName 带路径的文件名
        * @param[in] newline 是否要添加换行符
        * @return 表示文件内容的字符串
        */
        static string ReadFile(const string& fileName, bool newline) {
            std::ifstream in(fileName);
            string content;
            if (in.is_open()) {
                string line;
                while (std::getline(in, line)) {
                    content += line;
                    if (newline) content += "\n";
                }
            }
            in.close();
            return content;
        }
        /**
        * @brief 写入文件
        * @param[in] fileName 带路径的文件名
        * @param[in] content 要写入的内容
        * @return true/false分别表示写入成功/失败
        */
        static bool WriteFile(const string& fileName, const string& content) {
            std::ofstream out(fileName, std::ios::trunc);
            if (out.is_open()) {
                out.write(content.c_str(), content.size());
                out.close();
                return true;
            }
            else return false;
        }
        /**
        * @brief 生成唯一的文件名
        *
        * 将当前时间（毫秒级）与计数器的值拼接得到唯一文件名
        *
        * @return 表示文件名的字符串
        */
        static string UniqFileName() {
            static std::atomic_uint count(0);
            count ++ ;
            unsigned int value = count.load();
            return TimeUtil::GetMsTime() + "_" + std::to_string(value);
        }
    };
    /**
    * @class StringUtil
    * @brief 字符串操作相关的类
    */
    class StringUtil {
    public:
        /**
        * @brief 根据指定字符分割字符串
        * @param[in] src 原字符串
        * @param[out] tokens 分割后的子字符串
        * @param[in] sep 分隔符
        */
        static void SplitString(const string &src, vector<string> &tokens, const string &sep) {
            boost::split(tokens, src, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
}