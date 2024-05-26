#include <string>
#include "../comm/httplib.h"
#include "code_process.h"

using std::string;
using namespace ns_code_process;
using namespace httplib;

/**
* @file code_process_server.cpp
* @brief 将编译运行服务打包成http接口
*
* 根据命令行参数开启对应的端口，监听网络中的请求
*
* @author ren77281
* @date 2024-04-11
*/

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "usage:\n\t" << " port\n";
        return 1;
    }
    httplib::Server svr;
    svr.Post("/code_process", [](const Request &req, Response &resp){
        string inJson, outJson;
        inJson = req.body;
        if (inJson.size()) {
            CodeProcess::CompileRun(inJson, outJson);
            // 设置字符集，防止中文乱码
            resp.set_content(outJson, "application/json;charset=utf-8");
        }
    });

    svr.listen("0.0.0.0", atoi(argv[1]));
    return 0;
}