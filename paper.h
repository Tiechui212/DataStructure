#pragma once

// 公共论文数据结构：所有模块统一包含此头文件。

const int MAX_PAPERS = 1000;
const int MAX_KEYWORDS = 6;

struct Paper {
    int id = 0;
    char title[256] = {};
    char authors[512] = {};
    // 固定保留 6 个关键词位置；空关键词也保留为空字符串。
    char keywords[MAX_KEYWORDS][128] = {};
    int keywordCount = 0;  // 实际非空关键词数量
    int year = 0;
    int citations = 0;
    char abstractSnippet[1024] = {};
};

struct PaperList {
    Paper data[MAX_PAPERS];
    int length = 0;
};

// 全局顺序表只在 paper.cpp 中定义一次，其他模块通过此声明访问。
extern PaperList g_papers;
