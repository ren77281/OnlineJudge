#ifndef CODE_PROCESS_H
#define CODE_PROCESS_H

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <jsoncpp/json/json.h>
#include "run.h"
#include "compiler.h"
#include "../comm/log.hpp"
#include "../comm/util.hpp"

/**
* @file code_process.h
* @brief 编译并运行用户的代码
* @author ren77281
* @date 2024-04-11
*/

namespace ns_code_process {
    using namespace ns_compiler;
    using namespace ns_run;
    using namespace ns_util;
    using namespace ns_log;
    using std::string;
    using std::vector;
    /**
    * @class CodeProcess
    * @brief 具有编译并运行功能的类
    */
    class CodeProcess {
    public:
    /**
    * @brief 编译并运行程序
    * 解析inJson串，根据得到的数据运行程序，构造outJson串，返回本次运行结果
    * @param[in] inJson
    * inJson格式：
    *   code：用户提交的代码
    *   in：标准答案/用户自测的输入样例
    *   out：标准答案的输出样例
    *   memLimit：该题的内存限制
    *   timeLimit：该题的时间限制
    * @param[out] outJson
    * outJsosn格式：
    *   status：程序的状态码
    *   reason：请求结果（对于状态码的解释）
    *   userOut：用户的输出结果
    *   comerr：用户代码的编译错误
    *   runerr：用户代码的运行时错误
    */
        static void CompileRun(const string& inJson, string& outJson);
    private:
    /**
    * @brief 清理编译运行产生的临时文件
    * 将根据文件名删除可能存在的延申文件
    * @param[in] fileName 不带路径的文件名
    */
        static void CleanTempFile(const string& fileName);

    /**
    * @brief 解释outJson中的状态码
    * @param[in] status 状态码
    * @return 状态码对应的解释原因
    */
        static string StatusToReason(int status);
    };
}

#endif