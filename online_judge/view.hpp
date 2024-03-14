#pragma once
#include <string>
#include <vector>
#include "model.hpp"

namespace ns_view {
    using std::string;
    using std::vector;
    const string templatePath = "./template_html/";
    class View {
    public:
        // 获取题目的简要信息BriefPuzzle，据此渲染题目列表
        void ExpandList(vector<struct BriefPuzzle> &puzzles, string &outHtml) {
            const string& TemplatePath = templatePath + "puzzles.html";
            ctemplate::TemplateDictionary root("allPuzzle");
            for (auto &puzzle : puzzles) {
                ctemplate::TemplateDictionary *sub = root.AddIncludeDictionary("puzzle");
                sub->SetValue("id", puzzle.id);
                sub->SetValue("title", puzzle.title);
                sub->SetValue("hard", puzzle.hard);

                ctemplate::Template *tql = ctemplate::Template::GetTemplate(templatePath, ctemplate::DO_NOT_STRIP);
                tql->Expand(&outHtml, &root);
            }
        }

        // 两个部分
        // 左边为题目描述，以markdown的格式显示题目
        // 右边为代码部分，后续可以提供语言选项
        void ExpandOnePuzzle(int id, string &outHtml) {
            const string oneTemplatePath = templatePath + "onePuzzle.html";
            ctemplate::TemplateDictionary root("onePuzzle");
            // TODO

        }
    };
}