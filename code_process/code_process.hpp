#pragma once

#include <jsoncpp/json/json.h>
#include <unistd.h>
#include "compiler.hpp"
#include "run.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace ns_code_process {
    using namespace ns_compiler;
    using namespace ns_run;
    using namespace ns_util;
    using namespace ns_log;
    using std::string;

    class CodeProcess {
    public:
        /*
        输入型参数inJson：
            code：用户提交的代码
            input：用户的标准输入
            memLimit：该题的内存限制
            cpuLimit：该题的时间限制
        输出型参数outJson:
            status：处理程序的状态码
            reason：请求结果（对于状态码的解释）
            stdout：用户代码的标准输出
            comerr：用户代码的编译错误
            runerr：用户代码的运行时错误
        */
        static void Start(const string& inJson, string& outJson) {
            // 反序列化String串为Json串
            Json::Value inValue;
            Json::Reader reader;
            reader.parse(inJson, inValue); 
            // 读取Json串中的内容
            string code = inValue["code"].asString();
            string input = inValue["input"].asString();
            int memLimit = inValue["memLimit"].asInt();
            int cpuLimit = inValue["cpuLimit"].asInt();
            
            int status = 0;
            string fileName = FileUtil::UniqFileName(); 
            Json::Value outValue;

            do {
                // 代码为空
                if (code.size() == 0) { status = -1; continue; } 
                // 写入用户所写代码
                if (!FileUtil::WriteFile(PathUtil::Cpp(fileName), code)) { status = -2; continue; } 
                // 编译用户代码
                if (!Compiler::Compile(fileName)) { 
                    status = -3; // 无法编译用户代码
                    string fileComerr = FileUtil::ReadFile(PathUtil::CompileError(fileName), true);
                    outValue["comerr"] = fileComerr;
                    continue; 
                }
                // 写入input文件，若Json串中存在input，则此次提交为用户自测
                // 若不存在input，则input为所有的用例
                if (input.size()) {
                    if (!FileUtil::WriteFile(PathUtil::In(fileName), input)) { status = -4; continue; } 
                }

                // 运行用户代码
                int runResult = Runer::Run(fileName, cpuLimit, memLimit);
                if (runResult < 0) {status = -5; continue; }
                // 用户代码发生运行时错误
                else if (runResult > 0) {
                    status = runResult; 
                    string fileRunerr = FileUtil::ReadFile(PathUtil::RunError(fileName), true);
                    outValue["stderr"] = fileRunerr; 
                    continue; 
                }
                // 用户代码正确运行
                else { 
                    string fileStdout = FileUtil::ReadFile(PathUtil::Out(fileName), true);
                    outValue["stdout"] = fileStdout; 
                    continue;
                }
            } while (false);

            outValue["status"] = status;
            outValue["reason"] = StatusToReason(status);
            Json::StyledWriter writer;
            outJson = writer.write(outValue);

            // 清理临时文件
            CleanTempFile(fileName);
        }
    private:
        static void CleanTempFile(const string& fileName) {
            if (FileUtil::IsExist(PathUtil::Exe(fileName))) unlink(PathUtil::Exe(fileName).c_str());
            if (FileUtil::IsExist(PathUtil::Out(fileName))) unlink(PathUtil::Out(fileName).c_str());
            if (FileUtil::IsExist(PathUtil::CompileError(fileName))) unlink(PathUtil::CompileError(fileName).c_str());
            if (FileUtil::IsExist(PathUtil::RunError(fileName))) unlink(PathUtil::RunError(fileName).c_str());
            if (FileUtil::IsExist(PathUtil::In(fileName))) unlink(PathUtil::In(fileName).c_str());
            if (FileUtil::IsExist(PathUtil::Cpp(fileName))) unlink(PathUtil::Cpp(fileName).c_str());
        }

        static string StatusToReason(int status) {
            string reason;
            switch (status) {
            case -1:
                reason = "用户代码为空\n";
                break;
            case -2:
                reason = "未知错误，请重试\n";
                break;
            case -3:
                reason = "编译失败，请检查代码语法\n";
                break;
            case -4:
                reason = "未知错误，请重试\n";
                break;
            case SIGXCPU:
                reason = "程序超时，请检查时间复杂度\n";
                break;
            case SIGFPE:
                reason = "浮点数溢出\n";
                break;
            case SIGABRT:
                reason = "内存超限，请检查空间复杂度\n";
                break;
            case SIGSEGV:
                reason = "发生段错误，请检查可能的越界访问\n";
                break;
            case 0:
                reason = "运行成功！\n";
                break;
            default:
                reason = "程序被未知信号: " + std::to_string(status) + " 终止，请通知网站管理员\n";
                break;
            }
            return reason;
        }
    };
}