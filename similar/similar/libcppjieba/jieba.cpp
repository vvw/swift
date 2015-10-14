//
//  jieba.cpp
//  similar
//
//  Created by vvw on 10/14/15.
//  Copyright (c) 2015 vvw. All rights reserved.
//

#include "include/Application.hpp"
#include "stdlib.h"

using namespace CppJieba;

///Users/vvw/Desktop/dict/
extern "C" char* testjieba() {
    CppJieba::Application app("/Users/vvw/Desktop/dict/jieba.dict.utf8",
                              "/Users/vvw/Desktop/dict/hmm_model.utf8",
                              "/Users/vvw/Desktop/dict/user.dict.utf8",
                              "/Users/vvw/Desktop/dict/idf.utf8",
                              "/Users/vvw/Desktop/dict/stop_words.utf8");
    vector<string> words;
    string result;
    //string s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。";
    string s = "我";

    string results;
    const char *p = 0;
    char *buf = 0;
    
    app.cut(s, words, METHOD_MP);
    results = join(words.begin(), words.end(), "--|--");
    p = results.c_str();
    int len = (int)strlen (p);
    buf = (char*)malloc (len+1);
    memset(buf, 0, len+1);
    memcpy(buf, p, len+1);
    
    //cout << results << endl;
    return buf;
}