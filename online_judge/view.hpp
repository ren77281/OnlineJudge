#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctemplate/template.h>
#include "model.hpp"

/*
    view.hpp：负责将数据渲染成网页，使用ctemplate进行模板替换
    void ExpandList(vector<struct BriefPuzzle> &puzzles, string &outHtml)：根据简要的题目信息puzzles，渲染原模板outHtml
    void ExpandOnePuzzle(int id, string &outHtml)：根据题号id获取题目的详细信息，渲染原模板outHtml
*/

namespace ns_view {
    using std::string;
    using std::vector;
    using std::to_string;
    using namespace ns_model;
    using namespace ns_util;
    const string listPath = "./template_html/puzzlelist.html";
    const string puzzlePath = "./template_html/puzzle.html";
    class View {
    public:
        // 获取题目的简要信息BriefPuzzle，据此渲染题目列表
        void ExpandList(vector<struct BriefPuzzle> &puzzles, string &outHtml) {
            ctemplate::TemplateDictionary root("rootPuzzleList");
            for (auto &puzzle : puzzles) {
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("puzzleList");
                sub->SetValue("id", to_string(puzzle.id));
                sub->SetValue("title", puzzle.title);
                sub->SetValue("hard", puzzle.hard);
            }
            ctemplate::Template *tql = ctemplate::Template::GetTemplate(listPath, ctemplate::DO_NOT_STRIP);
            tql->Expand(&outHtml, &root);
        }

        void ExpandOnePuzzle(int id, const string& title, string &outHtml) {
            ctemplate::TemplateDictionary root("Puzzle");
            string desc = FileUtil::ReadFile("./puzzles/" + to_string(id) + "/desc.txt", true);
            root.SetValue("markdownText", desc);
            root.SetValue("puzzleId", to_string(id));
            root.SetValue("title", title);
            ctemplate::Template *tql = ctemplate::Template::GetTemplate(puzzlePath, ctemplate::DO_NOT_STRIP);
            tql->Expand(&outHtml, &root);
        }
    };
}