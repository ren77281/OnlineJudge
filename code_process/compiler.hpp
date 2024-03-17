#pragma once

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "../comm/log.hpp"
#include "../comm/util.hpp"

/*
    compiler.hpp：负责源文件的编译
    static int Compile(const std::string& fileName)：根据传入的文件名，fork子进程以编译相应的文件
    返回负数时，表示系统自身发生错误导致无法编译。返回0表示编译完成，返回正数时，表示用户代码存在语法错误导致编译失败
*/

namespace ns_compiler {
    using namespace ns_util;
    using namespace ns_log;

    class Compiler {
        
    public:
        static int Compile(const std::string& fileName) {
            pid_t pid = fork();
            if (pid < 0) {
                LOG(FATAL) << "无法生成子进程\n";
                return -1;
            }
            else if (pid == 0) {
                umask(0);
                int fileError = open(PathUtil::CompileError(fileName).c_str(), O_CREAT | O_WRONLY, 0644);
                if (fileError < 0) {
                    LOG(FATAL) << "打开文件时发生错误\n";
                    return -1;
                }
                dup2(fileError, 2);
                execlp("g++", "g++", PathUtil::Cpp(fileName).c_str(), "-o", PathUtil::Exe(fileName).c_str(), "-std=c++11", NULL);
                LOG(FATAL) << "编译时，子进程无法进行程序替换\n";
                return -1;
            }
            else {
                waitpid(pid, NULL, 0);
                // 检查可执行文件是否存在
                if (FileUtil::IsExist(PathUtil::Exe(fileName))) {
                    return 0;
                }
                LOG(ERROR) << "没有生成可执行文件\n";
                return 1;
            }
        }
    };
}