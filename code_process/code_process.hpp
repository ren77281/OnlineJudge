#pragma once

#include <jsoncpp/json/json.h>
#include <unistd.h>
#include "compiler.hpp"
#include "run.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"

/*
    code_process.hpp：连结compiler.hpp和run.hpp，负责编译并运行代码
    static void CleanTempFile(const string& fileName)：清理运行源文件时产生的临时文件，包括源文件
    static string StatusToReason(int status)：将编译运行代码产生的错误码（自己规定），转换成文字描述
    static void CompileRun(const string& inJson, string& outJson)：编译并运行代码，输入为inJson字符串，输出为outJson字符串
    Json格式将在下方说明
*/

namespace ns_code_process {
    using namespace ns_compiler;
    using namespace ns_run;
    using namespace ns_util;
    using namespace ns_log;
    using std::string;
    using std::vector;

    class CodeProcess {
    public:
        /*
        将用户自测和判题功能耦合在一起，样例数量为1，此时为用户自测
        输入型参数inJson：
            code：用户提交的代码
            in：标准答案/用户自测的输入样例
            out：标准答案的输出样例，当in为用户自测时为空
            memLimit：该题的内存限制
            timeLimit：该题的时间限制
        输出型参数outJson:
            status：处理程序的状态码
            reason：请求结果（对于状态码的解释）
            userOut：用户的输出结果
            comerr：用户代码的编译错误
            runerr：用户代码的运行时错误
        */
        static void CompileRun(const string& inJson, string& outJson) {
            // 反序列化String串为Json串
            vector<string> in, out, userOut;

            Json::Value inValue;
            Json::CharReaderBuilder readerBuilder;
            string errors;
            std::istringstream iss(inJson);
            if (!Json::parseFromStream(readerBuilder, iss, &inValue, &errors)) {
                LOG(FATAL) << "反序列化Json串失败\n";
                return;
            }
            // 读取Json串中的内容
            int count = 1;
            for (const auto& str : inValue["in"]) {
                in.push_back(str.asString());
            }
            count = 1;
            for (const auto& str : inValue["out"]) {
                out.push_back(str.asString());
            }
            string code = inValue["code"].asString();
            int memLimit = inValue["memLimit"].asInt();
            int timeLimit = inValue["timeLimit"].asInt();
            int status = 0;
            // 生成唯一文件名
            string fileName = FileUtil::UniqFileName(); 
            Json::Value outValue;

            do {
                // 代码为空
                if (code.size() == 0) { status = -1; continue; } 
                // 写入用户所写代码
                if (!FileUtil::WriteFile(PathUtil::Cpp(fileName), code)) { status = -2; continue; } 
                // 编译用户代码
                int compileResult = Compiler::Compile(fileName);
                if (compileResult > 0) { 
                    status = -3; // 用户代码存在语法错误
                    string fileComerr = FileUtil::ReadFile(PathUtil::CompileError(fileName), true);
                    outValue["comerr"] = fileComerr;
                    continue; 
                }
                else if (compileResult < 0) {
                    status = -2;
                    continue;
                }
                // 运行用户代码
                int runResult = Runer::Run(fileName, in, userOut, timeLimit, memLimit);
                // 系统自身出错
                if (runResult < 0) {status = -2; continue; }
                // 用户代码发生运行时错误
                else if (runResult > 0) {
                    status = runResult; 
                    string fileRunerr = FileUtil::ReadFile(PathUtil::RunError(fileName), true);
                    outValue["runerr"] = fileRunerr; 
                    continue; 
                }
                /*  用户代码正确运行且此时的请求为判题，将正确结果和用户的输出结果比较
                *   程序的测试用例大于1，可以视为此次提交为判题
                *   用例数为0或者1，可以视为自测
                */
                else if (in.size() > 1) {
                    if (out.size() != userOut.size()) {
                        status = -4;
                        continue;
                    }
                    for (int i = 0; i < out.size(); ++ i) {
                        if (userOut[i].back() == '\n') userOut[i].pop_back();
                        if (out[i] != userOut[i]) {
                            status = -4;
                            break;
                        }
                    }
                }
            } while (false);

            // 序列化Json串
            for (auto &uout : userOut) {
                outValue["userOut"].append(uout);
            }
            
            outValue["status"] = status;
            outValue["reason"] = StatusToReason(status);
            Json::StreamWriterBuilder writerBuilder;
            outJson = Json::writeString(writerBuilder, outValue);
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
            if (status > 128) status -= 128;
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
                reason = "结果不正确，请重新检查代码\n";
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
                reason = "程序被未知信号: " + std::to_string(status) + " 终止，请重试或等待管理员处理\n";
                break;
            }
            return reason;
        }
    };
}