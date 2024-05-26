#ifndef MODEL_H
#define MODEL_H

#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "../comm/log.hpp"
#include "../comm/util.hpp"

/**
* @file model.h 
* @brief 数据交互模块
* 题目数据以文件形式存储在online_judge/puzzles目录下
* @author ren77281
* @date 2024-04-21
*/

namespace ns_model {
    using std::string;
    using std::vector;
    namespace fs = std::filesystem;
    // 描述所有题目的文件
    const static string puzzleListPath = "./puzzles/puzzle.list";
    // 该路径下，每个题目拥有单独的文件夹，存储着样例，以及正式的输入输出
    const static string puzzlePath = "./puzzles/";

    /**
    * @struct BriefPuzzle
    * @brief 描述题目的简要信息
    * 题号，时间/空间限制，题目，难度
    */
    struct BriefPuzzle {
        int id, timeLimit, memLimit;
        string title, hard;
    };
    
    /**
    * @struct Puzzle
    * @brief 描述题目的所有信息
    * 题号，时间/空间限制，题目，题目描述
    * 输入用例，输出用例
    */
    struct Puzzle {
        int id, timeLimit, memLimit;
        string title, desc;
        vector<string> in, out;
    };

    /**
    * @class Model
    * @brief 数据交互器
    */
    class Model {
    public:
        // 初始化时调用LoadPuzzleList()
        Model();

        /**
        * @brief 加载题目列表
        * 打开puzzle.list文件，解析文件内容，初始化类成员变量briefPuzzles
        * @return 加载是否成功
        */
        bool LoadPuzzleList();

        /** 
        * @brief 加载具体的题目
        * 根据题号id加载题目的详细信息，以懒汉的形式构建成员变量puzzles
        * @return 加载是否成功
        */
        bool LoadOnePuzzle(int id);
        
        /**
        * @brief 获取题目列表
        * @param[out] out 获取到的所有题目信息
        * 获取私有成员briefPuzzles的数据
        * @return 获取是否成功
        */
        bool GetPuzzleList(vector<struct BriefPuzzle> &out);

        /**
        * @brief 获取具体题目
        * @param[in] id 题号
        * @param[out] getPuzzle 获取到的题目信息
        * 根据题号id将一道题的详细信息保存到getPuzzle中
        * @return 获取是否成功
        */
        bool GetOnePullze(int id, Puzzle &getPuzzle);

    private:
        // 用哈希表存储题号与题目的所有信息
        std::unordered_map<int, Puzzle> puzzles;
        // 用哈希表存储题号与题目的简要信息
        std::map<int, BriefPuzzle> briefPuzzles;
    };
}

#endif
