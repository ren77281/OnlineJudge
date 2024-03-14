#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <../comm/util.hpp>

namespace ns_control {
    using namespace ns_util;
    using namespace ns_view;
    using namespace ns_model;
    using std::string;
    using std::mutex;
    using std::vector;
    class Machine {
    private:
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
        mutex mtx;
    public:
        LoadBalancer() {
            mtx = new mutex();
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
                m.ip = toekns[0];
                m.port = atoi(tokens[1].c_str());
                m.mtx = new mutex();
                m.load = 0;
                inline.push_back(machines.size());
                machines.push_back(m);
            }
            in.close();
            LOG(INFO) << "machine.conf配置文件载入完成\n";
            return true;
        }

        bool Choice(Machine &m, int &id) {
            if (online.size() == 0) {
                LOG(ERROR) << "所有主机已经下线\n";
                return false;
            }
            
            mtx.lock();
            id = online[0], m = machines[online[0]];
            for (int i = 1; i < online.size(); ++ i) {
                if (machines[online[i]].load < machines[online[id]].load) {
                    id = online[i], m = machines[online[id]];
                }
            }
            mtx.unlock();
            return true;
        }

        bool OfflineMachine(int id) {
            for (int i = 0; i < online.size(); ++ i) {
                if (online[i] == id) {
                    online.erase(id);
                    offline.push_back(id);
                    return true;
                }
            }
            return false;
        }

        // 上线所有主机
        bool OnlineMachine() {
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
    public:
        bool LoadPuzzleList(string &html) {
            vector<BriefPuzzle> puzzles;
            if (model.GetPuzzleList(puzzles)) {
                view.ExpandList(puzzles, html);
            }
            return false;
        }

        bool LoadOnePuzzle(int id, string &html) {
            struct Puzzle puzzle;
            if (model.GetOnePullze(id, puzzle)) {
                view.ExpandOnePuzzle(id, html);
            }
            return false;
        }

        // 核心判题功能
        void Judge()
    };
}