#include "loader.h"
#include "paper.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {

// 按分隔符拆分，并保留相邻分隔符、行尾分隔符所表示的空字段。
std::vector<std::string> splitPreservingEmpty(const std::string& line, char delimiter) {
    std::vector<std::string> fields;
    std::size_t start = 0;

    while (true) {
        const std::size_t pos = line.find(delimiter, start);
        if (pos == std::string::npos) {
            fields.push_back(line.substr(start));
            break;
        }
        fields.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
    return fields;
}

// 将文本安全复制到固定长度字符数组；超长时返回 false，不截断数据。
template <std::size_t N>
bool copyField(char (&destination)[N], const std::string& source) {
    if (source.size() >= N) {
        return false;  // 需要为结尾的 '\0' 留出一个字节
    }
    std::memcpy(destination, source.c_str(), source.size() + 1);
    return true;
}

bool parseInteger(const std::string& text, int& value) {
    if (text.empty()) {
        return false;
    }

    std::istringstream input(text);
    input >> value;
    if (input.fail()) {
        return false;
    }

    // 不允许数字后面再出现其他非空白字符。
    input >> std::ws;
    return input.eof();
}

bool parsePaper(const std::string& line, Paper& paper, std::string& error) {
    const std::vector<std::string> fields = splitPreservingEmpty(line, '|');
    if (fields.size() != 12) {
        error = "字段数量应为 12，实际为 " + std::to_string(fields.size());
        return false;
    }

    Paper parsed;
    if (!parseInteger(fields[0], parsed.id)) {
        error = "ID 不是有效整数";
        return false;
    }
    if (!copyField(parsed.title, fields[1])) {
        error = "标题过长（最多 255 字节）";
        return false;
    }
    if (!copyField(parsed.authors, fields[2])) {
        error = "作者字段过长（最多 511 字节）";
        return false;
    }

    parsed.keywordCount = 0;
    for (int i = 0; i < MAX_KEYWORDS; ++i) {
        const std::string& keyword = fields[3 + i];
        if (!copyField(parsed.keywords[i], keyword)) {
            error = "第 " + std::to_string(i + 1) + " 个关键词过长（最多 127 字节）";
            return false;
        }
        if (!keyword.empty()) {
            ++parsed.keywordCount;
        }
    }

    if (!parseInteger(fields[9], parsed.year)) {
        error = "Year 不是有效整数";
        return false;
    }
    if (!parseInteger(fields[10], parsed.citations)) {
        error = "Citations 不是有效整数";
        return false;
    }
    if (!copyField(parsed.abstractSnippet, fields[11])) {
        error = "摘要字段过长（最多 1023 字节）";
        return false;
    }

    paper = parsed;
    return true;
}

}  // namespace

bool loadPapers(const std::string& filename) {
    std::ifstream input(filename.c_str());
    if (!input.is_open()) {
        std::cerr << "加载失败：无法打开文件 \"" << filename << "\"。请检查文件路径。\n";
        return false;
    }

    std::string header;
    if (!std::getline(input, header)) {
        std::cerr << "加载失败：文件为空，缺少表头。\n";
        return false;
    }
    // 兼容 Windows CRLF 文件的行尾回车符。
    if (!header.empty() && header.back() == '\r') {
        header.pop_back();
    }

    // PaperList 的固定数组约占数 MB。放在堆上可避免 Windows 默认线程栈
    // 较小时发生栈溢出，同时仍能保证完整解析成功后再替换全局数据。
    std::unique_ptr<PaperList> loaded(new PaperList);
    std::string line;
    int lineNumber = 1;

    while (std::getline(input, line)) {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // 忽略完全空白的行，但不忽略含有分隔符的记录。
        if (line.empty()) {
            continue;
        }

        if (loaded->length >= MAX_PAPERS) {
            std::cerr << "加载失败：论文数量超过上限 " << MAX_PAPERS << "。\n";
            return false;
        }

        Paper paper;
        std::string error;
        if (!parsePaper(line, paper, error)) {
            std::cerr << "加载失败：第 " << lineNumber << " 行解析错误：" << error << "。\n";
            return false;
        }

        loaded->data[loaded->length] = paper;
        ++loaded->length;
    }

    if (input.bad()) {
        std::cerr << "加载失败：读取文件时发生 I/O 错误。\n";
        return false;
    }

    // 仅在整份文件全部解析成功后替换全局数据，避免失败时留下半份数据。
    g_papers = *loaded;
    std::cout << "成功加载 " << g_papers.length << " 条论文记录。\n";
    return true;
}
