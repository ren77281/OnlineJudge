#include "compiler.h"

namespace ns_compiler {
    using namespace ns_util;
    using namespace ns_log;
    int Compiler::Compile(const std::string& fileName) {
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
            // 不存在说明存在语法错误，编译失败
            return 1;
        }
    }
}