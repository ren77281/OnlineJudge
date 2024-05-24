#include <string>
#include <stdlib.h>
#include "control.hpp"
#include "../comm/httplib.h"
using std::string;
using namespace ns_model;
using namespace ns_control;
using namespace httplib;

int main() {
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

    svr.listen("0.0.0.0", 8000);
    return 0;
}