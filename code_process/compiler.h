#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "../comm/log.hpp"
#include "../comm/util.hpp"

/**
* @file compiler.h
* @brief 编译cpp源文件
* @author ren77281
* @date 2024-04-08
*/

namespace ns_compiler {
    using namespace ns_util;
    using namespace ns_log;
    /**
    * @class Compiler
    * @brief 具有编译功能的类
    */
    class Compiler {
    public:
        /**
        * @brief 编译cpp文件
        * @param[in] fileName 不需要路径的cpp文件名，（默认该文件在当前目录下的code目录中）
        * @return <0表示因为测评机原因导致的失败，=0表示成功编译，>0表示因为语法错误导致的失败
        */
        static int Compile(const std::string& fileName);
    };
}

#endif