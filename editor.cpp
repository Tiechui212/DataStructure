#include "editor.h"

#include <iostream>
#include <cstring>
#include <string>

bool addPaperInteractive() {
    if (g_papers.length >= MAX_PAPERS) {
        std::cout << "论文数量已达上限，无法添加。\n";
        return false;
    }

    Paper newPaper = {};

    std::cout << "请输入论文ID：";
    std::cin >> newPaper.id;
    std::cin.ignore();

    for (int i = 0; i < g_papers.length; ++i) {
        if (g_papers.data[i].id == newPaper.id) {
            std::cout << "论文ID已存在，添加失败。\n";
            return false;
        }
    }

    std::string input;

    std::cout << "请输入标题：";
    std::getline(std::cin, input);
    std::strncpy(newPaper.title, input.c_str(),
                 sizeof(newPaper.title) - 1);
    newPaper.title[sizeof(newPaper.title) - 1] = '\0';

    std::cout << "请输入作者（多个作者用;分隔）：";
    std::getline(std::cin, input);
    std::strncpy(newPaper.authors, input.c_str(),
                 sizeof(newPaper.authors) - 1);
    newPaper.authors[sizeof(newPaper.authors) - 1] = '\0';

    newPaper.keywordCount = 0;
    for (int i = 0; i < MAX_KEYWORDS; ++i) {
        std::cout << "请输入第" << i + 1
                  << "个关键词（不需要请直接回车）：";
        std::getline(std::cin, input);

        std::strncpy(newPaper.keywords[i], input.c_str(),
                     sizeof(newPaper.keywords[i]) - 1);
        newPaper.keywords[i][sizeof(newPaper.keywords[i]) - 1] = '\0';

        if (!input.empty()) {
            ++newPaper.keywordCount;
        }
    }

    std::cout << "请输入发表年份：";
    std::cin >> newPaper.year;
    std::cin.ignore();

    std::cout << "请输入引用次数：";
    std::cin >> newPaper.citations;
    std::cin.ignore();

    std::cout << "请输入摘要片段：";
    std::getline(std::cin, input);
    std::strncpy(newPaper.abstractSnippet, input.c_str(),
                 sizeof(newPaper.abstractSnippet) - 1);
    newPaper.abstractSnippet[sizeof(newPaper.abstractSnippet) - 1] = '\0';

    g_papers.data[g_papers.length] = newPaper;
    ++g_papers.length;

    std::cout << "论文添加成功。\n";
    return true;
}

bool deletePaperById(int id) {
    int pos = -1;

    for (int i = 0; i < g_papers.length; ++i) {
        if (g_papers.data[i].id == id) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        std::cout << "未找到指定ID的论文，删除失败。\n";
        return false;
    }

    for (int i = pos; i < g_papers.length - 1; ++i) {
        g_papers.data[i] = g_papers.data[i + 1];
    }

    --g_papers.length;

    std::cout << "论文删除成功。\n";
    return true;
}
