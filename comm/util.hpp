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

/*
FileUtil：
static bool WriteFile(const string& fileName, const string& content)：将content写入fileName文件中
*/

namespace ns_util {
    using std::string;
    using std::vector;
    string code_path = "./code/";
    
    class PathUtil {
    public:
        static string AddSuffix(const string& fileName, const string& suffix) {
            string res = code_path;
            res += fileName;
            res += suffix;
            return res;
        }
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
    
    class TimeUtil {
    public:
        static string GetMsTime() {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return std::to_string(tv.tv_sec * 1000l + tv.tv_usec / 1000);
        }
        static string GetTimeStamp() {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return std::to_string(tv.tv_sec);
        }
    };

    class FileUtil {
    public:
        static bool IsExist(const string& fileName) {
            struct stat statbuf;
            if (stat(fileName.c_str(), &statbuf) < 0) {
                return false;
            }
            return true;
        }
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
        static bool WriteFile(const string& fileName, const string& content) {
            std::ofstream out(fileName, std::ios::trunc);
            if (out.is_open()) {
                out.write(content.c_str(), content.size());
                out.close();
                return true;
            }
            else return false;
        }
        static string UniqFileName() {
            static std::atomic_uint count(0);
            count ++ ;
            unsigned int value = count.load();
            return TimeUtil::GetMsTime() + "_" + std::to_string(value);
        }
    };
    class StringUtil {
    public:
        static void SplitString(const string &src, vector<string> &tokens, const string &sep) {
            boost::split(tokens, src, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
}