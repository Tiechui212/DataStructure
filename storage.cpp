#include "storage.h"
#include "paper.h"

#include <fstream>
#include <iostream>
#include <locale>

namespace {

// 文本须以 '\0' 结尾，且不能含有分隔符或换行。
template <std::size_t N>
bool validText(const char (&text)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        if (text[i] == '\0') {
            return true;
        }
        if (text[i] == '|' || text[i] == '\n' || text[i] == '\r') {
            return false;
        }
    }
    return false;
}

bool validPaper(const Paper& paper) {
    if (!validText(paper.title) || !validText(paper.authors) ||
        !validText(paper.abstractSnippet)) {
        return false;
    }

    int actualKeywordCount = 0;
    for (int i = 0; i < MAX_KEYWORDS; ++i) {
        if (!validText(paper.keywords[i])) {
            return false;
        }
        if (paper.keywords[i][0] != '\0') {
            ++actualKeywordCount;
        }
    }
    // 关键词数量在重载时重新计算，因此须与实际内容一致。
    return paper.keywordCount == actualKeywordCount;
}

}

void displayPapers() {
    if (g_papers.length < 0 || g_papers.length > MAX_PAPERS) {
        std::cerr << "显示失败：顺序表长度无效。\n";
        return;
    }
    if (g_papers.length == 0) {
        std::cout << "暂无论文数据。\n";
        return;
    }

    std::cout << "共有 " << g_papers.length << " 篇论文：\n";
    for (int i = 0; i < g_papers.length; ++i) {
        const Paper& paper = g_papers.data[i];
        if (!validText(paper.title)) {
            std::cerr << "显示失败：第 " << i + 1 << " 篇论文的标题格式无效。\n";
            return;
        }
        std::cout << "ID: " << paper.id << '\n'
                  << "标题: " << paper.title << '\n'
                  << "年份: " << paper.year << '\n'
                  << "引用次数: " << paper.citations << "\n\n";
    }
}

bool savePapers(const std::string& filename) {
    if (g_papers.length < 0 || g_papers.length > MAX_PAPERS) {
        std::cerr << "保存失败：顺序表长度无效。\n";
        return false;
    }

    // 先检查数据，避免打开文件后才发现错误，导致原内容丢失。
    for (int i = 0; i < g_papers.length; ++i) {
        if (!validPaper(g_papers.data[i])) {
            std::cerr << "保存失败：第 " << i + 1
                      << " 篇论文的文本格式或关键词数量无效。\n";
            return false;
        }
    }

    std::ofstream output(filename.c_str(), std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "保存失败：无法打开文件 \"" << filename << "\"。\n";
        return false;
    }
    // 使用固定格式，避免数字受系统区域设置影响。
    output.imbue(std::locale::classic());
    output << "ID|Title|AuthorsString|Keyword1|Keyword2|Keyword3|Keyword4|"
              "Keyword5|Keyword6|Year|Citations|AbstractSnippet\n";

    for (int i = 0; i < g_papers.length; ++i) {
        const Paper& paper = g_papers.data[i];
        output << paper.id << '|' << paper.title << '|' << paper.authors;
        // 空关键词也要保留位置，防止后续字段错位。
        for (int j = 0; j < MAX_KEYWORDS; ++j) {
            output << '|' << paper.keywords[j];
        }
        output << '|' << paper.year << '|' << paper.citations
               << '|' << paper.abstractSnippet << '\n';
    }

    // 关闭文件后再检查，确保缓冲区中的数据也已写入。
    output.close();
    if (output.fail()) {
        std::cerr << "保存失败：写入或关闭文件时发生 I/O 错误。\n";
        return false;
    }
    return true;
}
