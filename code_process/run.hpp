#pragma once

#include <string>
#include <unistd.h>
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

namespace ns_run {
    using std::string;
    class Runer {
    public:
        // 限制程序资源：时长（秒），内存（MB）
        static void SetRLimit(int cpuLimit, uint64_t memLimit) {
            // 内存的单位转换
            memLimit = memLimit * 1024 * 1024;
            // 设置时间（cpu时长）限制
            struct rlimit cpuRLimit;
            cpuRLimit.rlim_cur = cpuLimit, cpuRLimit.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_CPU, &cpuRLimit);
            // 设置虚拟内存限制
            struct rlimit memRLimit;
            memRLimit.rlim_cur = memLimit, memRLimit.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_AS, &memRLimit);
        }
        static int Run(const string& fileName, int cpuLimit, int memLimit) {
            pid_t pid = fork();
            if (pid < 0) {
                LOG(ERROR) << "准备运行程序时，无法生成子进程\n";
                return -1;
            }
            else if (pid == 0) {
                string exeFile = PathUtil::Exe(fileName);
                string inFile = PathUtil::In(fileName);
                string outFile = PathUtil::Out(fileName);
                string errFile = PathUtil::RunError(fileName);

                umask(0);
                int inFileFd = open(inFile.c_str(), O_CREAT | O_WRONLY, 0644);
                int outFileFd = open(outFile.c_str(), O_CREAT | O_WRONLY, 0644);
                int errFileFd = open(errFile.c_str(), O_CREAT | O_WRONLY, 0644);
                if (inFileFd < 0 || outFileFd < 0 || errFileFd < 0) {
                    close(inFileFd), close(outFileFd), close(errFileFd);
                    LOG(ERROR) << "准备运行程序时，重定向流文件打开失败\n";
                    return -2;
                }
                // 将标准流重定向到文件
                dup2(inFileFd, 0), dup2(outFileFd, 1), dup2(errFileFd, 2);
                // 设置程序的资源限制
                SetRLimit(cpuLimit, memLimit);
                // 替换子进程以执行可执行文件
                execlp(exeFile.c_str(), exeFile.c_str(), NULL);
                // 若替换失败，关闭文件
                close(inFileFd), close(outFileFd), close(errFileFd);
                LOG(ERROR) << "准备运行程序时，子进程替换失败\n";
                return -3;
            }
            else {
                int status = 0;
                waitpid(pid, &status, 0);
                if (status & 0xff) {
                    LOG(ERROR) << "子进程异常退出，退出码为:" << (status & 0xff) << "\n";
                }
                return (status & 0xff);
            }
        }
    };
}