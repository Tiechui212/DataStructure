#pragma once

// 根据当前 g_papers 重建 ID 哈希索引。
// 构建前会清空旧桶和旧节点，不会累积上一次的数据。
void buildHashFromPapers();

// 按论文 ID 精确查找，找到时返回论文在 g_papers.data 中的下标；
// 未找到、索引尚未构建或索引为空时返回 -1。
int searchHash(int id);
