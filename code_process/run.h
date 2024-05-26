#ifndef RUN_H
#define RUN_H

#include <fcntl.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../comm/util.hpp"
#include "../comm/log.hpp"

/**
* @file run.h
* @brief 运行可执行文件
* @author ren77281
* @date 2024-04-09
*/

namespace ns_run {
    using std::string;
    using std::vector;
    using namespace ns_util;
    /**
    * @class Runer
    * @brief 具有运行功能的类
    */
    class Runer {
    public:
        /**
        * @brief 限制可执行程序的时间与空间
        * @param[in] timeLimit 时间限制（秒）
        * @param[in] memLimit 空间限制（MB）
        */
        static void SetRLimit(int timeLimit, int memLimit);
        
        /**
        * @brief 运行可执行文件
        * 
        * 根据测试用例的数量创建子进程以运行程序
        * 每个子进程的标准输入都是测试用例，输出结果将保存到userOut中
        * 
        * @param[in] fileName 不带路径的文件名
        * @param[in] in 输入用例
        * @param[out] userOut 输出结果
        * @param[in] timeLimit 时间限制
        * @param[in] memLimit 空间限制 
        * @return >0表示子进程异常退出，=0表示顺利运行，<0表示发生测评机错误
        */
        static int Run(const string& fileName, vector<string> &in,\
            vector<string> &userOut, int timeLimit, int memLimit);
    };
}

#endif