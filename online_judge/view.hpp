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
        void ExpandList(vector<struct BriefPuzzle> &Puzzles, string &outHtml) {
            const string& TemplatePath = templatePath + "puzzles.html";
            ctemplate::TemplateDictionary root("allPuzzle");
            for (auto &puzzle : allPuzzles) {
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
        void ExpandOneHtml(string &outHtml) {
            const string oneTemplatePath = templatePath + "onePuzzle.html";
            ctemplate::TemplateDictionary root("onePuzzle");
            // TODO
        }
    };
}