#include <string>
#include "../comm/httplib.h"
#include "code_process.hpp"

using std::string;
using namespace ns_code_process;
using namespace httplib;

/*
    将编译并运行模块打包成网络服务
*/

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
            CodeProcess::CompileRun(inJson, outJson);
            // 设置字符集，防止中文乱码
            resp.set_content(outJson, "application/json;charset=utf-8");
        }
    });

    svr.listen("0.0.0.0", atoi(argv[1]));
    return 0;
}