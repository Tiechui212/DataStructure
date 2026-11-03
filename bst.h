#pragma once

#include "paper.h"

// 节点仅保存标题和论文下标，不复制整篇论文。
struct BSTNode {
    char title[sizeof(Paper{}.title)] = {};
    int paper_index = -1;
    int left_child_idx = -1;
    int right_child_idx = -1;
};

// 固定数组节点池；-1 表示空树或空子节点。
extern BSTNode BST_NODES[MAX_PAPERS];
extern int bst_root_index;
extern int next_free_bst_node_index;

void resetBST();

// title 必须是以 '\0' 结尾的字符串，paperIndex 是 g_papers 中的有效下标。
// 成功插入返回 true；重复标题（保留原节点）、容量不足或参数无效返回 false。
bool insertPaperIntoBST(const char* title, int paperIndex);

// 先清空旧索引，再按顺序表顺序构建。重复标题保留第一篇并提示。
// 无效顺序表长度或缺少结束符的标题会使构建终止，索引保持为空。
void buildBSTFromPapers();

// 完整标题、区分大小写的精确匹配；返回论文下标，不是 BST 节点下标。
// 未找到、空树或 nullptr 均返回 -1。
int searchBST(const char* title);
