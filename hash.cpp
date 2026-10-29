#include "hash.h"

#include "paper.h"

#include <algorithm>
#include <iostream>

namespace {

// 选用大于阶段一数据量的质数作为桶数。不同 ID 仍可能落入同一桶，
// 冲突通过节点中的 next 下标组成单链表解决。
const int HASH_BUCKET_COUNT = 101;

struct HashNode {
    int id = 0;
    int paperIndex = -1;
    int next = -1;
};

int bucketHeads[HASH_BUCKET_COUNT];
HashNode nodePool[MAX_PAPERS];
int nodeCount = 0;
bool hashBuilt = false;

int bucketIndexFor(int id) {
    int index = id % HASH_BUCKET_COUNT;
    if (index < 0) {
        index += HASH_BUCKET_COUNT;
    }
    return index;
}

int findInBucket(int id) {
    const int bucket = bucketIndexFor(id);
    int nodeIndex = bucketHeads[bucket];

    while (nodeIndex != -1) {
        if (nodePool[nodeIndex].id == id) {
            return nodePool[nodeIndex].paperIndex;
        }
        nodeIndex = nodePool[nodeIndex].next;
    }
    return -1;
}

bool insertHashNode(int id, int paperIndex) {
    if (nodeCount >= MAX_PAPERS) {
        std::cerr << "哈希索引构建失败：节点池容量不足。\n";
        return false;
    }

    // ID 按阶段一约定应保持唯一。若输入文件中出现重复 ID，保留
    // 顺序表中第一次出现的记录，使查找结果稳定。
    if (findInBucket(id) != -1) {
        std::cerr << "警告：论文 ID " << id
                  << " 重复，哈希索引保留第一条记录。\n";
        return true;
    }

    const int bucket = bucketIndexFor(id);
    HashNode& node = nodePool[nodeCount];
    node.id = id;
    node.paperIndex = paperIndex;
    node.next = bucketHeads[bucket];
    bucketHeads[bucket] = nodeCount;
    ++nodeCount;
    return true;
}

}  // namespace

void buildHashFromPapers() {
    std::fill(bucketHeads, bucketHeads + HASH_BUCKET_COUNT, -1);
    nodeCount = 0;
    hashBuilt = true;

    if (g_papers.length < 0 || g_papers.length > MAX_PAPERS) {
        std::cerr << "哈希索引构建失败：顺序表长度无效。\n";
        hashBuilt = false;
        return;
    }

    for (int i = 0; i < g_papers.length; ++i) {
        if (!insertHashNode(g_papers.data[i].id, i)) {
            hashBuilt = false;
            return;
        }
    }
}

int searchHash(int id) {
    if (!hashBuilt) {
        return -1;
    }
    return findInBucket(id);
}
