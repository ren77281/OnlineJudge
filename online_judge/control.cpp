#include "control.h"

namespace ns_control {
    bool Machine::IncreaseLoad() {
        if (mtx == nullptr) {
            return false;
        }
        mtx->lock();
        load ++ ;
        mtx->unlock();
        return true;
    }

    bool Machine::DecreaseLoad() {
        if (mtx == nullptr) {
            return false;
        }
        mtx->lock();
        load -- ;
        mtx->unlock();
        return true;
    }

    bool Machine::ResetLoad() {
        if (mtx == nullptr) {
            return false;
        }
        mtx->lock();
        load = 0;
        mtx->unlock();
        return true;
    }

    LoadBalancer::LoadBalancer() {
        assert(LoadConfig());
    }

    bool LoadBalancer::LoadConfig() {
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

    bool LoadBalancer::Choice(Machine &m, int &id) {
        if (online.size() == 0) {
            LOG(ERROR) << "所有主机已经下线，无法进行编译\n";
            return false;
        }
        
        id = online[0], m = machines[online[0]];
        for (int i = 0; i < online.size(); ++ i) {
            if (machines[online[i]].load < machines[online[id]].load) {
                id = online[i], m = machines[online[id]];
            }
        }
        return true;
    }

    bool LoadBalancer::OfflineMachine(int id) {
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
    void LoadBalancer::OnlineMachine() {
        for (int i = 0; i < offline.size(); ++ i) {
            online.push_back(offline[i]);
        }
        offline.clear();
    }


    bool Control::LoadPuzzleList(string &html) {
        vector<BriefPuzzle> puzzles;
        if (model.GetPuzzleList(puzzles)) {
            view.ExpandList(puzzles, html);
        }
        return false;
    } 
    // 渲染具体题目
    bool Control::LoadOnePuzzle(int id, string &html) {
        struct Puzzle puzzle;
        if (model.GetOnePullze(id, puzzle)) {
            view.ExpandOnePuzzle(id, puzzle.title, html);
        }
        return false;
    }

    void Control::Judge(int id, const string &inJson, string &outJson) {
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
        string action = inValue["action"].asString();

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
        if (action == "submit") {
            for (auto t : p.in) {
                compileValue["in"].append(t);
            }
            for (auto t : p.out) {
                compileValue["out"].append(t);
            }
        }
        // 此次提交为自测
        else {
            if (!userIn.empty() && userIn.back() != '\n') userIn += '\n';
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
}