#include "bst.h"

#include <cstring>
#include <iostream>

BSTNode BST_NODES[MAX_PAPERS];
int bst_root_index = -1;
int next_free_bst_node_index = 0;

namespace {

// 节点只由本模块分配，避免调用方分配后未连接到树。
int getNewBSTNodeIndex() {
    if (next_free_bst_node_index >= MAX_PAPERS) {
        return -1;
    }
    const int index = next_free_bst_node_index++;
    BST_NODES[index] = BSTNode{};
    return index;
}

}  // namespace

void resetBST() {
    // 旧节点不再可达；下次分配时重新初始化，不必逐个清空数组。
    bst_root_index = -1;
    next_free_bst_node_index = 0;
}

bool insertPaperIntoBST(const char* title, int paperIndex) {
    if (title == nullptr || g_papers.length < 0 || g_papers.length > MAX_PAPERS ||
        paperIndex < 0 || paperIndex >= g_papers.length) {
        std::cerr << "BST 插入失败：标题或论文下标无效。\n";
        return false;
    }
    if (std::strlen(title) >= sizeof(BST_NODES[0].title)) {
        std::cerr << "BST 插入失败：标题超过 255 字节。\n";
        return false;
    }

    int parent = -1;
    int current = bst_root_index;
    int comparison = 0;
    while (current != -1) {
        comparison = std::strcmp(title, BST_NODES[current].title);
        if (comparison == 0) {
            std::cerr << "BST：重复标题，保留第一条索引：" << title << '\n';
            return false;
        }
        parent = current;
        current = comparison < 0 ? BST_NODES[current].left_child_idx
                                 : BST_NODES[current].right_child_idx;
    }

    const int nodeIndex = getNewBSTNodeIndex();
    if (nodeIndex == -1) {
        std::cerr << "BST 插入失败：节点池已满。\n";
        return false;
    }
    BSTNode& node = BST_NODES[nodeIndex];
    std::strcpy(node.title, title);  // 上面已检查容量，包括结尾的 '\0'。
    node.paper_index = paperIndex;

    if (parent == -1) {
        bst_root_index = nodeIndex;
    } else if (comparison < 0) {
        BST_NODES[parent].left_child_idx = nodeIndex;
    } else {
        BST_NODES[parent].right_child_idx = nodeIndex;
    }
    return true;
}

void buildBSTFromPapers() {
    resetBST();
    if (g_papers.length < 0 || g_papers.length > MAX_PAPERS) {
        std::cerr << "BST 构建失败：顺序表长度无效。\n";
        return;
    }
    // 先检查全部标题，防止异常数据产生半棵树或越界读取。
    for (int i = 0; i < g_papers.length; ++i) {
        if (std::memchr(g_papers.data[i].title, '\0', sizeof(g_papers.data[i].title)) == nullptr) {
            std::cerr << "BST 构建失败：下标 " << i << " 的标题缺少字符串结束符。\n";
            return;
        }
    }
    for (int i = 0; i < g_papers.length; ++i) {
        insertPaperIntoBST(g_papers.data[i].title, i);
    }
}

int searchBST(const char* title) {
    if (title == nullptr) {
        return -1;
    }
    int current = bst_root_index;
    while (current != -1) {
        const BSTNode& node = BST_NODES[current];
        const int comparison = std::strcmp(title, node.title);
        if (comparison == 0) {
            return node.paper_index;
        }
        current = comparison < 0 ? node.left_child_idx : node.right_child_idx;
    }
    return -1;
}
