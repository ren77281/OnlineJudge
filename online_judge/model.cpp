#include "model.h"

namespace ns_model {
    Model::Model() {
        assert (LoadPuzzleList());
    }
    
    bool Model::LoadPuzzleList() {
        std::ifstream in(puzzleListPath);
        if (!in.is_open()) {
            LOG(FATAL) << "加载题目列表失败，无法启动系统！\n";
            return false;
        }
        string line;
        while (getline(in, line)) {
            vector<string> tokens;
            ns_util::StringUtil::SplitString(line, tokens, " ");
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

    bool Model::GetPuzzleList(vector<struct BriefPuzzle> &out) {
        if (briefPuzzles.size() == 0) {
            LOG(ERROR) << "获取题库失败，预载题目数量为0！\n";
            return false;
        } 
        for (auto p : briefPuzzles) {
            out.push_back(p.second);
        }
        return true;
    }

    // 根据题号将题目加载到puzzles中
    bool Model::LoadOnePuzzle(int id) {
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
        for (const auto &entry : fs::directory_iterator(path + "in")) {
            if (entry.is_regular_file()) {
                std::ifstream puzzleIn(entry.path());
                if (puzzleIn.is_open()) {
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
                std::ifstream puzzleOut(entry.path());
                if (puzzleOut.is_open()) {
                    string puzzleLine, allOut;
                    while (getline(puzzleOut, puzzleLine)) {
                        allOut += puzzleLine;
                    }
                    puzzle.out.push_back(allOut);
                    puzzleOut.close();
                }
            }
        }

        puzzles[id] = puzzle;
        return true;
    }

    bool Model::GetOnePullze(int id, Puzzle &getPuzzle) {
        if (puzzles.count(id) == 0) {
            if (!LoadOnePuzzle(id)) {
                return false;
            }
        }
        getPuzzle = puzzles[id];
        return true;
    }
}