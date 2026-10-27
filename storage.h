#pragma once

#include <string>

// 显示所有论文的 ID、标题、年份和引用次数。
void displayPapers();

// 按原文件格式保存论文，成功返回 true，失败返回 false。
bool savePapers(const std::string& filename);
