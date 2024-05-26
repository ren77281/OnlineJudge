#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctemplate/template.h>
#include "model.h"

/**
* @file view.h
* @brief 负责前端页面的渲染
* @author ren77281
* @date 2024-04-20
*/

namespace ns_view {
    using std::string;
    using std::vector;
    using std::to_string;
    using namespace ns_model;
    using namespace ns_util;
    const static string listPath = "./template_html/puzzlelist.html";
    const static string puzzlePath = "./template_html/puzzle.html";
    /**
    * @class View 
    * @brief 渲染器
    */
    class View {
    public:
        /**
        * @brief 渲染题目列表
        * @param[in] puzzles 所有题目的详细数据
        * @param[in,out] outHtml 渲染后的网页数据
        * outHtml作为原始网页数据，将被渲染并返回
        */
        void ExpandList(vector<struct BriefPuzzle> &puzzles, string &outHtml);
        /**
        * @brief 渲染答题网页
        * @param[in] id 需要渲染的题号id
        * @param[in] title 题目标题
        * @param[in,out] outHtml 渲染后的网页数据
        * outHtml作为原始网页数据，将被渲染并返回
        */
        void ExpandOnePuzzle(int id, const string& title, string &outHtml);
       
    };
}

#endif