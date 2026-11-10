#include "search_service.h"
#include "bst.h"
#include "hash.h"
#include "paper.h"

#include <iostream>

void rebuildSearchIndexes() {
    buildHashFromPapers();
    buildBSTFromPapers();
}

void displayPaperDetail(int paperIndex) {
    if (g_papers.length < 0 || g_papers.length > MAX_PAPERS ||
        paperIndex < 0 || paperIndex >= g_papers.length) {
        std::cerr << "无法显示论文：下标无效。\n";
        return;
    }
    const Paper& paper = g_papers.data[paperIndex];
    std::cout << "ID: " << paper.id << '\n'
              << "标题: " << paper.title << '\n'
              << "作者: " << paper.authors << '\n'
              << "关键词: ";
    bool first = true;
    for (int i = 0; i < MAX_KEYWORDS; ++i) {
        if (paper.keywords[i][0] != '\0') {
            if (!first) std::cout << "; ";
            std::cout << paper.keywords[i];
            first = false;
        }
    }
    if (first) std::cout << "无";
    std::cout << '\n' << "年份: " << paper.year << '\n'
              << "引用次数: " << paper.citations << '\n'
              << "摘要: " << paper.abstractSnippet << '\n';
}
