#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "model.hpp"
#include "view.hpp"
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "../comm/httplib.h"

/*
    class Machine：部署了编译服务的主机
        IncreaseLoad，DecreaseLoad与ResetLoad：增加，减少与重置主机的负载，函数都为原子操作
    class LoadBalancer：负载均衡器
        bool LoadConfig()：加载主机的配置文件，这些主机部署了系统的编译服务，在构造函数中调用该函数
        bool Choice(Machine &m, int &id)：选择一个负载最小的主机
        bool OfflineMachine(int id)与bool OnlineMachine()：离线指定主机与上线所有主机
    class Control：最重要的类，关联model.hpp与view.hpp，是MVC模块的控制器
        bool LoadPuzzleList(string &html)：加载题目列表，通过model模块获取briefPuzzles，再将数据通过view模块渲染网页
        bool LoadOnePuzzle(int id, string &html)：加载具体题目，通过model模块获取具体题目的信息Puzzle，再将数据通过view模块渲染网页
        void Judge(int id, const string &inJson, string &outJson)：判题功能，用户传入inJson字符串，函数返回outJson字符串
        inJson格式：
        code：用户提交的代码
        userIn：用户自测输入的数据
        outJson格式和code_process模块中的outJson相同
*/


namespace ns_control {
    using namespace ns_util;
    using namespace ns_view;
    using namespace ns_model;
    using std::string;
    using std::mutex;
    using std::vector;
    class Machine {
    public:
        string ip;
        int port;
        int load;
        mutex *mtx = nullptr;
    public:
        bool IncreaseLoad() {
            if (mtx == nullptr) {
                return false;
            }
            mtx->lock();
            load ++ ;
            mtx->unlock();
            return true;
        }

        bool DecreaseLoad() {
            if (mtx == nullptr) {
                return false;
            }
            mtx->lock();
            load -- ;
            mtx->unlock();
            return true;
        }

        bool ResetLoad() {
            if (mtx == nullptr) {
                return false;
            }
            mtx->lock();
            load = 0;
            mtx->unlock();
            return true;
        }
    };

    const string &configPath = "./conf/machine.conf";
    class LoadBalancer {
    private:
        // 部署了编译服务的主机地址，用下标表示主机的唯一性
        vector<Machine> machines;
        // 所有在线/离线的主机，保存主机的下标
        vector<int> online, offline;
    public:
        LoadBalancer() {
            assert(LoadConfig());
        }

        bool LoadConfig() {
            std::ifstream in(configPath);
            if (!in.is_open()) {
                return false;
            }
            string line;
            while (getline(in, line)) {
                vector<string> tokens;
                StringUtil::SplitString(line, tokens, ":");
                if (tokens.size() != 2) {
                    LOG(FATAL) << "machine.conf存在格式错误\n";
                    continue;
                }
                Machine m;
                m.ip = tokens[0];
                m.port = atoi(tokens[1].c_str());
                m.mtx = new mutex();
                m.load = 0;
                online.push_back(machines.size());
                machines.push_back(m);
            }
            in.close();
            LOG(INFO) << "machine.conf配置文件载入完成\n";
            return true;
        }

        bool Choice(Machine &m, int &id) {
            if (online.size() == 0) {
                LOG(ERROR) << "所有主机已经下线，无法进行编译\n";
                return false;
            }
            
            id = online[0], m = machines[online[0]];
            for (int i = 1; i < online.size(); ++ i) {
                if (machines[online[i]].load < machines[online[id]].load) {
                    id = online[i], m = machines[online[id]];
                }
            }
            return true;
        }

        bool OfflineMachine(int id) {
            for (auto it = online.begin(); it != online.end(); it ++) {
                if (*it == id) {
                    online.erase(it);
                    offline.push_back(*it);
                    return true;
                }
            }
            return false;
        }

        // 上线所有主机
        void OnlineMachine() {
            for (int i = 0; i < offline.size(); ++ i) {
                online.push_back(offline[i]);
            }
            offline.clear();
        }
    };

    class Control {
    private:
        Model model;
        View view;
        LoadBalancer loadBalancer;
    public:
        // 加载题目列表
        bool LoadPuzzleList(string &html) {
            vector<BriefPuzzle> puzzles;
            if (model.GetPuzzleList(puzzles)) {
                view.ExpandList(puzzles, html);
            }
            return false;
        }
        // 加载具体题目
        bool LoadOnePuzzle(int id, string &html) {
            struct Puzzle puzzle;
            if (model.GetOnePullze(id, puzzle)) {
                view.ExpandOnePuzzle(id, puzzle.title, html);
            }
            return false;
        }

        // 核心判题功能，根据题号获取题目的具体信息，调用code_process服务编译运行代码
        // inJson
        // code：用户提交的代码
        // userIn：用户自测输入的数据
        void Judge(int id, const string &inJson, string &outJson) {
            // 反序列化用户提交的Json串
            Json::Value inValue;
            Json::CharReaderBuilder readerBuilder;
            string errors;
            std::istringstream iss(inJson);
            if (!Json::parseFromStream(readerBuilder, iss, &inValue, &errors)) {
                LOG(FATAL) << "反序列化Json串失败\n";
                return;
            }
            string code = inValue["code"].asString();
            string userIn = inValue["userIn"].asString();

            // 获取题目的具体信息
            struct Puzzle p;
            model.GetOnePullze(id, p);

            // 构建Json串，以提交给code_process服务
            string compileString;
            Json::Value compileValue;
            compileValue["code"] = code;
            compileValue["memLimit"] = p.memLimit;
            compileValue["timeLimit"] = p.timeLimit;
            // 此次提交为判题
            if (userIn.empty()) {
                for (auto t : p.in) {
                    compileValue["in"].append(t);
                }
                for (auto t : p.out) {
                    compileValue["out"].append(t);
                }
            }
            // 此次提交为自测
            else {
                if (userIn.back() != '\n') userIn += '\n';
                compileValue["in"].append(userIn);
            }
            // 序列化complieValue为compileJson
            Json::StreamWriterBuilder writerBuilder;
            string compileJson = Json::writeString(writerBuilder, compileValue);
            // 使用负载均衡选择主机，再将序列化完成的Json串提交给code_process服务
            while (true) {
                Machine m; int mid;
                if (!loadBalancer.Choice(m, mid)) {
                    break;
                }
                // 选择负载最低的主机后，提交Json串
                m.IncreaseLoad();
                httplib::Client cli(m.ip, m.port);
                LOG(INFO) << "选择的主机ip和端口分别为:" << m.ip << ' ' << m.port << "主机负载为:" << m.load << "\n";
                if (auto res = cli.Post("/code_process", compileJson, "application/json; charset=utf-8")) {
                    if (res->status == 200) {
                        outJson = res->body;
                        m.DecreaseLoad();
                        LOG(INFO) << "代码成功编译并运行，服务ip和端口分别为：" << m.ip << ' ' << m.port << "\n";
                        return; // 编译运行完成，直接返回
                    }
                    else {
                        LOG(INFO) << "请求发送成功，但服务器出现错误，服务ip，端口与错误码分别为：" << m.ip << ' ' << m.port << ' ' << res->status << "\n";
                        m.DecreaseLoad();
                    }
                }
                else {
                    // 请求发送失败，将主机离线
                    LOG(ERROR) << "主机请求失败，IP和端口分别为:" << m.ip << ' ' << m.port << "\n";
                    loadBalancer.OfflineMachine(mid);
                }
            }
        }

    };
}