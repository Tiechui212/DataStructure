#pragma once

// 数据加载、增删成功后调用；当前重建 ID 哈希索引。
void rebuildSearchIndexes();

// 显示顺序表中指定下标的论文详情，非法下标会提示错误。
void displayPaperDetail(int paperIndex);
