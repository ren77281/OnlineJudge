#include "view.h"

namespace ns_view {
    // 获取题目的简要信息BriefPuzzle，据此渲染题目列表
    void View::ExpandList(vector<struct BriefPuzzle> &puzzles, string &outHtml) {
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
    void View::ExpandOnePuzzle(int id, const string& title, string &outHtml) {
        ctemplate::TemplateDictionary root("Puzzle");
        string desc = FileUtil::ReadFile("./puzzles/" + to_string(id) + "/desc.txt", true);
        root.SetValue("markdownText", desc);
        root.SetValue("puzzleId", to_string(id));
        root.SetValue("title", title);
        ctemplate::Template *tql = ctemplate::Template::GetTemplate(puzzlePath, ctemplate::DO_NOT_STRIP);
        tql->Expand(&outHtml, &root);
    }
}