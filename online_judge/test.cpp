#include <ctemplate/template.h>
#include <iostream>
using namespace std;

int main(){
    ctemplate::TemplateDictionary root("onePuzzle");
    string desc;
    std::istream in("./puzzles/1/desc.txt");
    if (!in.is_open()) {
        LOG(FATAL) << "获取题目信息失败，题号为：" << id << "\n";
        outHtml = "出错了，请重试\n";
        return;
    }
    string line;
    while (getline(in, line)) {
        desc += line;
    }
    root.SetValue("markdownText", desc);
    ctemplate::TemplateDictionary root("rootPuzzle");
    ctemplate::Template *tql = ctemplate::Template::GetTemplate(puzzleList, ctemplate::DO_NOT_STRIP);
    tql->Expand(&outHtml, &root);
    return 0;
}