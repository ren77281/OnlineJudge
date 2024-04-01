#pragma once

#include <string>
#include <unistd.h>
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

/*
    run.hpp：负责源文件的运行
    static void SetRLimit(int timeLimit, int memLimit)：设置进程能使用的资源上限，也就是设置程序可使用的时间与空间
    static int Run(const string& fileName, vector<string> &in, vector<string> &userOut, 
    int timeLimit, int memLimit)：根据测试用例in运行可执行程序，保存结果到userOut中。有几个用例就创建几个子进程运行用例
    返回值为int，若>0表示子进程异常，被系统信号终止。=0表示运行完毕，无异常发生。<0表示遇到系统自身的错误
*/

namespace ns_run {
    using std::string;
    class Runer {
    public:
        // 限制程序资源：时长（秒），内存（MB）
        static void SetRLimit(int timeLimit, long long memLimit) {
            // 内存的单位转换
            memLimit = memLimit * 1024 * 1024;
            // 设置时间（cpu时长）限制
            struct rlimit cpuRLimit;
            cpuRLimit.rlim_cur = timeLimit, cpuRLimit.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_CPU, &cpuRLimit);
            // 设置虚拟内存限制
            struct rlimit memRLimit;
            memRLimit.rlim_cur = memLimit, memRLimit.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_AS, &memRLimit);
        }
        // 文件名 标准答案的输入 用户的输出 时间限制 空间限制
        // 指定了程序的标准输入为in，创建多个子进程分别执行每个用例
        // 保存用户的输出到userOut中
        static int Run(const string& fileName, 
            vector<string> &in, vector<string> &userOut,
            int timeLimit, int memLimit) {
            string exeFile = PathUtil::Exe(fileName);
            string inFile = PathUtil::In(fileName);
            string outFile = PathUtil::Out(fileName);
            string errFile = PathUtil::RunError(fileName);
            
            // 根据测试用例的数量创建子进程
            for (int i = 0; i < in.size(); ++ i) {
                pid_t pid = fork();
                if (pid < 0) {
                    LOG(ERROR) << "准备运行程序时，无法生成子进程\n";
                    return -1;
                }
                else if (pid == 0) {
                    umask(0);
                    int inFileFd = open(inFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644);
                    int outFileFd = open(outFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644);
                    int errFileFd = open(errFile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644);
                    if (inFileFd < 0 || outFileFd < 0 || errFileFd < 0) {
                        close(inFileFd), close(outFileFd), close(errFileFd);
                        LOG(ERROR) << "准备运行程序时，重定向流文件打开失败\n";
                        return -2;
                    }
                    // 写入标准输入文件
                    if (!FileUtil::WriteFile(inFile, in[i])) {
                        LOG(FATAL) << "写入标准输入文件失败！\n";
                        return -3;
                    }
                    // 设置程序的资源限制
                    SetRLimit(timeLimit, memLimit);
                    // 将标准流重定向到文件
                    dup2(inFileFd, 0), dup2(outFileFd, 1), dup2(errFileFd, 2);
                    // 替换子进程以执行可执行文件
                    execlp(exeFile.c_str(), exeFile.c_str(), NULL);
                    // 若替换失败，关闭文件
                    LOG(ERROR) << "准备运行程序时，子进程替换失败\n";
                    return -4;
                }
                else {
                    int status = 0;
                    waitpid(pid, &status, 0);
                    if (status & 0xff) {
                        LOG(ERROR) << "子进程异常退出，退出码为:" << (status & 0xff) << "\n";
                        return status & 0xff;
                    }
                    // 保存用户的输出
                    userOut.push_back(FileUtil::ReadFile(PathUtil::Out(fileName), true));
                }
            }
            return 0;
        }
    };
}