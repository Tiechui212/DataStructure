#pragma once

#include <string>

// 从指定文件加载论文数据。文件格式为 12 个由 '|' 分隔的字段，首行为表头。
// 成功返回 true；失败返回 false，并保留加载前的 g_papers 内容。
bool loadPapers(const std::string& filename);
