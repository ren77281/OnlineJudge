#ifndef CONTROL_H
#define CONTROL_H

#include <mutex>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "model.h"
#include "view.h"
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "../comm/httplib.h"

/**
* @file control.h
* @brief MVC结构中的控制器，负责调用model与view
* @author ren77281
* @date 2024-04-20
*/

namespace ns_control {
    using namespace ns_util;
    using namespace ns_view;
    using namespace ns_model;
    using std::string;
    using std::mutex;
    using std::vector;
    
   /**
   * @class Machine 
   * @brief 表示测评机的信息
   * ip, port, 负载数
   */
    class Machine {
    public:
        string ip;
        int port;
        int load;
        mutex *mtx = nullptr;
    public:
        /**
        * 以下三个函数分别为：增加负载、减少负载、重载负载
        * 修改load对象的值，使用互斥锁mutex保证操作的原子性
        */
        bool IncreaseLoad();
        bool DecreaseLoad();
        bool ResetLoad();
    };

    const static std::string &configPath = "./conf/machine.conf";
    /**
    * @class LoadBalancer
    * @brief 负载均衡器，搭载负载均衡策略，选择负载最少的主机
    */
    class LoadBalancer {
    private:
        // 部署了编译服务的主评测机信息，用下标表示评测机的唯一性
        vector<Machine> machines;
        // 所有在线/离线的评测机，保存评测机的下标
        vector<int> online, offline;
    public:
        LoadBalancer();
        /**
        * @brief 加载评测机的配置文件，初始化成员变量machines与online
        * 被构造函数调用，若加载失败，则不会构建LoadBalancer对象
        * @return 加载成功/失败
        */
        bool LoadConfig();

        /**
        * @brief 负载均衡地选择一个评测机
        * @return 选择成功/失败
        */
        bool Choice(Machine &m, int &id);

        /**
        * 以下两个函数分别是：离线所有评测机与上线所有测评机
        * 是对online与offline容器的修改
        */
        bool OfflineMachine(int id);
        void OnlineMachine();
    };

    /**
    * @class Control 
    * @brief 控制器
    * 在内部耦合model和view两个模块，并暴露出接口供外部调用
    */
    class Control {
    private:
        Model model;
        View view;
        LoadBalancer loadBalancer;
    public:
        /**
        * @brief 加载题目列表
        * @param[out] html 加载完成的html网页数据
        * @return 加载成功/失败
        */
        bool LoadPuzzleList(string &html);

        /**
        * @brief 加载具体题目
        * @param[in] id 要加载的题号
        * @param[out] html 加载完成的html网页数据
        * @return 加载成功/失败
        */
        bool LoadOnePuzzle(int id, string &html);

        /**
        * @brief 核心判题功能
        * 根据id获取题目信息，构建inJson串，负载均衡地选择一台评测机
        * 同时评测机返回的outJson串
        * @param[in] id 要加载的题号
        * @param[in] inJson 题目信息与用户数据
        * inJson格式：
        *   code：用户提交的代码
        *   userIn：用户自测输入的数据
        *   action：自测/提交
        * @param[out] outJson 评测机返回的Json数据
        * outJson格式：
        *   status：程序的状态码
        *   reason：请求结果（对于状态码的解释）
        *   userOut：用户的输出结果
        *   comerr：用户代码的编译错误
        *   runerr：用户代码的运行时错误
        */
        void Judge(int id, const string &inJson, string &outJson);
    };
}

#endif