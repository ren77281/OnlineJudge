#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "../comm/log.hpp"
#include "../comm/util.hpp"

/*
    model.hpp：负责数据交互模块，提供加载题目列表的方法
    题目以文件的方式存储在online_judge/puzzles
    puzzles目录下存在puzzle.list文件，存储着所有题目的简要信息
    ：题号 题目 难度分值 时间要求 空间要求
    在puzzles目录下存在与题号相同的文件夹，存储in、out、desc，分别为输入、输出、题目描述（包括提示样例）
*/

namespace ns_model {
    using std::string;
    using std::vector;
    // 描述所有题目的文件
    const string puzzleList = "./puzzles/puzzle.list";
    // 该路径下，每个题目拥有单独的文件夹，存储着样例，以及正式的输入输出
    const string puzzlePath = "./puzzles/";

    struct Puzzle {
        int id, hard;
        int timeLimit, memLimit;
        string title, desc;
        vector<string> in, out;
    }

    class Model {
    public:
        Model() {
            assert (LoadPuzzleList());
        }
        bool LoadPuzzleList() {
            ifstream in(puzzleList);
            if (!in.open()) {
                LOG(FATAL) << "加载题目列表失败，无法启动系统！\n";
                return false;
            }
            string line;
            while (getline(in, line)) {
                vector<string> tokens;
                // TODO分割字符串
                StringUtil::SplitString(line, tokens, " ");
                struct puzzle;
                puzzle.id = atoi(tokens[0].c_str());
                puzzle.title = tokens[1];
                puzzle.hard = atoi(tokens[2].c_str());
                puzzle.timeLimit = atoi(tokens[3].c_str());
                puzzle.memLimit = atoi(tokens[4].c_str());

                string path = puzzlePath + tokens[0] + "/";
                // 读取题目的输入文件
                for (const auto &entry : fs::directory_iterator(path + "in")) {
                    if (entry.is_regular_file()) {
                        ifstream puzzleIn(entry.path());
                        if (puzzleIn.open()) {
                            string puzzleLine, allIn;
                            while (getline(puzzleIn, puzzleLine)) {
                                allIn += puzzleLine;
                            }
                            puzzle.in.push_back(allIn);
                            puzzleIn.close();
                        }
                    }
                }
                // 读取题目的输出文件
                for (const auto &entry : fs::directory_iterator(path + "out")) {
                    if (entry.is_regular_file()) {
                        ifstream puzzleOut(entry.path());
                        if (puzzleOut.open()) {
                            string puzzleLine, allOut;
                            while (getline(puzzleOut, puzzleLine)) {
                                allOut += puzzleLine;
                            }
                            puzzle.in.push_back(allOut);
                            puzzleOut.close();
                        }
                    }
                }
                puzzles[puzzle.id] = puzzle;
            }
        }
        bool GetAllPuzzles(vector<Puzzle> &getPuzzles) {
            if (getPuzzles.size() == 0) {
                LOG(WARNING) << "用户获取题目列表失败！\n";
                return false;
            }
            for (auto &puzzle : puzzles) {
                getPuzzles.push_back(puzzle.second);
            }
            return true;
        }
        bool GetOnePullze(int id, Puzzle &getPuzzle) {
            if (puzzles.count(id) == 0) {
                return false;
            }
            getPuzzle = puzzles[id];
            return true;
        }
    private:
        unordered_map<int, Puzzle> puzzles;
    };
}