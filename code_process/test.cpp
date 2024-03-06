#include <iostream>
#include <sstream>
#include <jsoncpp/json/json.h>
#include <string>
using namespace std;

int main() { 
    // Json::Value root;
    // vector<string> strs = {{"hello"}, {"world"}, {"hhhhh"}};
    // for (auto t : strs) {
    //     root["strings"].append(t);
    // }
    // root["test"] = "test";
    // Json::StreamWriterBuilder builder;
    // string inJson = Json::writeString(builder, root);

    // // cout << inJson << "\n";

    // Json::Value out;
    // Json::CharReaderBuilder readerBuilder;
    // vector<string> outstrs;
    // string errors;
    // std::istringstream iss(inJson);
    // if (Json::parseFromStream(readerBuilder, iss, &out, &errors)) {
    //     for (const auto& str : out["strings"]) {
    //         outstrs.push_back(str.asString());
    //     }
    // }
    // cout << out["test"].asString() << "\n";
    // // for (auto t : outstrs) {
    // //     cout << t << "\n";

    // // }
        string code = R"(#include <bits/stdc++.h>
        using namespace std;

        typedef pair<int, int> PII;
        typedef long long LL;
        typedef unsigned long long ULL;
        const int inf = 2e9 + 10;
        const LL INF = 4e18 + 10;
        const int mod9 = 998244353;
        const int mod7 = 1e9 + 7;
        const int N = 2e5 + 10;

        void solve()
        {
            string s; cin >> s;
            int i;
            for (i = s.size() - 1; i >= 0; -- i) 
                if (s[i] == '.')
                    break;
            for (i ++; i < s.size(); ++ i) cout << s[i];
        }

        int main()
        {
            ios::sync_with_stdio(false), cin.tie(0),cout.tie(0);
            solve();
            return 0;
        }
    )";
    Json::Value inValue;
    inValue["input"] = "";
    inValue["code"] = code;
    inValue["memLimit"] = 1024 * 1024 * 10;
    inValue["cpuLimit"] = 1;
    Json::StyledWriter writer;
    string inJson;
    inJson = writer.write(inValue);
    std::cout << "这是输入的Json串\n" << inJson << "\n";

    string outJson;
    CodeProcess::Start(inJson, outJson);
    std::cout << "这是返回的Json串\n" << outJson << "\n";
    return 0;
}