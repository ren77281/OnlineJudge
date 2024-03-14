#pragma once

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
    namespace fs = std::filesystem;
    // 描述所有题目的文件
    const string puzzleListPath = "./puzzles/puzzle.list";
    // 该路径下，每个题目拥有单独的文件夹，存储着样例，以及正式的输入输出
    const string puzzlePath = "./puzzles/";

    struct BriefPuzzle {
        uint64_t id, timeLimit, memLimit;
        string title, hard;
    };

    struct Puzzle {
        uint64_t id, timeLimit, memLimit;
        string title, desc;
        vector<string> in, out;
    };

    class Model {
    public:
        Model() {
            assert (LoadPuzzleList());
        }
        bool LoadPuzzleList() {
            std::ifstream in(puzzleListPath);
            if (!in.is_open()) {
                LOG(FATAL) << "加载题目列表失败，无法启动系统！\n";
                return false;
            }
            string line;
            while (getline(in, line)) {
                vector<string> tokens;
                StringUtil::SplitString(line, tokens, " ");
                // 题号 标题 难度 时间 空间
                if (tokens.size() != 5) {
                    LOG(WARNING) << "puzzle.list中存在无法识别的题目！\n";
                    continue;
                }
                // 将puzzle.list中的简要信息写入BriefPuzzles中
                struct BriefPuzzle bp;
                bp.id = atoi(tokens[0].c_str());
                bp.title = tokens[1], 
                bp.hard = tokens[2];
                bp.timeLimit = atoi(tokens[3].c_str());
                bp.memLimit = atoi(tokens[4].c_str());

                briefPuzzles[bp.id] = bp;
            }
            return true;
        }

        bool GetPuzzleList(vector<struct BriefPuzzle> &out) {
            if (puzzles.size() == 0) {
                LOG(ERROR) << "获取题库失败，预载题目数量为0！\n";
                return false;
            } 
            for (auto p : puzzles) {
                out.push_back(p);
            }
            return true;
        }

        // 根据题号将题目加载到puzzles中
        bool LoadOnePuzzle(int id) {
            if (briefPuzzles.count(id) == 0) {
                LOG(ERROR) << "用户请求的问题不存在，前端题目列表与后端题目列表不一致！\n";
                return false;
            }
            struct Puzzle puzzle;
            struct BriefPuzzle bp = briefPuzzles[id];
            
            puzzle.id = id;
            puzzle.timeLimit = bp.timeLimit;
            puzzle.memLimit = bp.memLimit;
            puzzle.title = bp.title;

            // 获取题目的全部输入与输出
            string path = puzzlePath + std::to_string(id) + "/";
            // 读取题目的输入文件
            std::cout << "in:\n";
            for (const auto &entry : fs::directory_iterator(path + "in")) {
                if (entry.is_regular_file()) {
                    std::ifstream puzzleIn(entry.path());
                    if (puzzleIn.is_open()) {
                        string puzzleLine, allIn;
                        while (getline(puzzleIn, puzzleLine)) {
                            allIn += puzzleLine;
                        }
                        // debug
                        std::cout << allIn << "\n";
                        puzzle.in.push_back(allIn);
                        puzzleIn.close();
                    }
                }
            }
            std::cout << "out:\n";
            // 读取题目的输出文件
            for (const auto &entry : fs::directory_iterator(path + "out")) {
                if (entry.is_regular_file()) {
                    std::ifstream puzzleOut(entry.path());
                    if (puzzleOut.is_open()) {
                        string puzzleLine, allOut;
                        while (getline(puzzleOut, puzzleLine)) {
                            allOut += puzzleLine;
                        }
                        // debug
                        std::cout << allOut << "\n";
                        puzzle.out.push_back(allOut);
                        puzzleOut.close();
                    }
                }
            }
            puzzles[id] = puzzle;
            return true;
        }

        bool GetOnePullze(uint64_t id, Puzzle &getPuzzle) {
            if (puzzles.count(id) == 0) {
                if (!LoadOnePuzzle(id)) {
                    return false;
                }
            }
            getPuzzle = puzzles[id];
            return true;
        }

    private:
        std::unordered_map<uint64_t, Puzzle> puzzles;
        std::map<uint64_t, BriefPuzzle> briefPuzzles;
    };
}

