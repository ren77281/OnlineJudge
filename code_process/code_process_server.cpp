#include <string>
#include "../comm/httplib.h"
#include "code_process.hpp"

using std::string;
using namespace ns_code_process;
using namespace httplib;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "usage:\n\t" << argv[1] << " port\n";
        return 1;
    }
    httplib::Server svr;
    svr.Post("/code_process", [](const Request &req, Response &resp){
        string inJson, outJson;
        inJson = req.body;
        if (inJson.size()) {
            CodeProcess::Start(inJson, outJson);
            // 设置字符集，防止中文乱码
            resp.set_content(outJson, "application/json;charset=utf-8");
        }
    });

    svr.listen("0.0.0.0", atoi(argv[1]));
    // string code = R"(#include <iostream>
    //     using namespace std;
    //     int main(){
    //         int t = 10 / 0;
    //         cout << "代码处理模块测试成功\n";
    //         return 0;
    //     }
    // )";
    // Json::Value inValue;
    // inValue["input"] = "";
    // inValue["code"] = code;
    // inValue["memLimit"] = 1024 * 1024 * 10;
    // inValue["cpuLimit"] = 1;
    // Json::StyledWriter writer;
    // string inJson;
    // inJson = writer.write(inValue);
    // std::cout << "这是输入的Json串\n" << inJson << "\n";

    // string outJson;
    // CodeProcess::Start(inJson, outJson);
    // std::cout << "这是返回的Json串\n" << outJson << "\n";
    return 0;
}