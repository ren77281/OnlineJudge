#include <string>
#include <stdlib.h>
#include "control.h"
#include "../comm/httplib.h"
using std::string;
using namespace ns_model;
using namespace ns_control;
using namespace httplib;

/**
* @file online_judge.cpp
* @brief 将MVC模块打包成http网络服务
* 根据命令行参数，打开相应的端口进行监听
* @author ren77281
* @date 2024-04-25
*/

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "usage:\n\t" << " port\n";
        return 1;
    }

    // TODO编译主机的上线处理
    httplib::Server svr;
    Control ctl;

    // 设置根网页
    svr.Get("/", [&](const Request &req, Response &resp){
        string html;
        ctl.LoadPuzzleList(html);
        resp.set_content(html, "text/html;charset=utf-8");
    });

    svr.Get("/puzzlelist", [&](const Request &req, Response &resp){
        string html;
        ctl.LoadPuzzleList(html);
        resp.set_content(html, "text/html;charset=utf-8");
    });

    svr.Get(R"(/puzzles/(\d+))", [&](const Request &req, Response &resp){
        string html;
        string id = req.matches[1];
        ctl.LoadOnePuzzle(atoi(id.c_str()), html);
        resp.set_content(html, "text/html;charset=utf-8");
    });

    svr.Post(R"(/judge/(\d+))", [&](const Request &req, Response &resp){
        string outJson;
        string id = req.matches[1];
        ctl.Judge(atoi(id.c_str()), req.body, outJson);
        resp.set_content(outJson, "application/json;charset=utf-8");
    });

    svr.listen("0.0.0.0", atoi(argv[1]));
    return 0;
}