#include "code_process.hpp"
#include <string>
using std::string;
using namespace ns_code_process;

int main() {
    string code = R"(#include <iostream>
        using namespace std;
        int main(){
            int t = 10 / 0;
            cout << "代码处理模块测试成功\n";
            return 0;
        }
    )";
    Json::Value inValue;
    inValue["input"] = "";
    inValue["code"] = code;
    inValue["memLimit"] = 1024 * 1024 * 10;
    inValue["cpuLimit"] = 1;
    Json::StyledWriter writer;
    string inJson;
    inJson = writer.write(inValue);
    std::cout << "这是输入的Json串\n" << inJson << "\n";

    string outJson;
    CodeProcess::Start(inJson, outJson);
    std::cout << "这是返回的Json串\n" << outJson << "\n";
    return 0;
}