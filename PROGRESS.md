# 项目进度

更新日期：2026-11-03。当前用户负责阶段二 D（菜单与集成）。

## 最新进展：D 的 ID 查找流程已完成

本节为本次实现后的状态，以下原同步快照保留用于追溯。

- 新增 search_service.h/.cpp：统一重建当前 ID 哈希索引，按合法下标显示全部论文详情（含作者、非空关键词、摘要）。
- main.cpp 菜单 6 已接通 searchHash；整行校验整数，拒绝空输入、非数字、尾随字符和超出 int 范围的 ID。
- 加载、添加、删除成功后均调用 rebuildSearchIndexes()；失败时不改变索引。
- 菜单输入和 ID 查找遇到 EOF 时安全退出。
- README.md 已补充编译、运行、测试说明。
- tests/id_flow_test.py 实际运行通过：未加载、已有/不存在 ID、非法输入及恢复、详情字段、添加后查找、哈希冲突（1001/1102）、删除后未命中、下标移动、重新加载清除旧索引、空数据、EOF。
- g++ C++11、Wall/Wextra/pedantic 编译通过，无诊断。测试使用临时数据副本，原始 papers.txt 未改动。
- 当前 D 下一步：等待/接收 A 的 bst.h/.cpp，在统一重建函数中加入 buildBSTFromPapers()，接通菜单 7，再与 C 完成双索引联调和报告。添加模块的数字输入校验仍为后续待处理项。
- 2026-11-03：用户要求将本次 ID 查找实现、测试和文档同步到小组 GitHub main 分支；推送结果以 Git 历史为准。

## 同步范围与依据

- 已重新读取 D:\Github\DataStructure 中全部 13 个项目文件，以及 D:\DataStructureA 中的阶段二分工与现有文件清单。
- 本次为本地文件状态同步，未执行远程 fetch/pull、提交或推送。
- 本地最新提交：d93f2db（2026-10-29，Add ID hash table index）；此前 bb0dbc4 完成阶段一菜单集成。
- 写入本记录前 Git 工作区干净。
- 分工文档保留了旧的“你是 A”描述；当前用户明确负责 D，以当前说明为准。

## 当前阶段

阶段一基础功能已接入主程序；阶段二已加入 ID 哈希模块，正在等待查找菜单集成及 BST 模块交付。

| 内容 | 文件状态 |
| --- | --- |
| 公共顺序表、加载、添加、删除、显示、保存 | 已实现，main.cpp 菜单 1～5 已调用 |
| 样例数据 | papers.txt 已加入，共 50 条记录 |
| B：ID 哈希模块 | hash.h/hash.cpp 已加入，链地址法、101 桶、固定数组节点池；提供 buildHashFromPapers()/searchHash(int) |
| A：标题 BST | 本地仓库尚无 bst.h/bst.cpp，无法据此判断成员在其他位置的开发情况 |
| D：查找集成 | 菜单 6、7 仍为占位；尚无 search_service.h/.cpp；加载、增删后尚未调用索引重建 |
| C：测试与性能分析 | 本地仓库未见 tests/search_test.cpp 或测试报告 |
| 后续功能 | 菜单 8～12 仍为占位 |

## 本次验证

- 使用 g++ -std=c++11 -Wall -Wextra -pedantic 编译 main.cpp、paper.cpp、loader.cpp、editor.cpp、storage.cpp、hash.cpp，成功且无诊断。
- 实际运行输入菜单 1、6、7、0：成功加载 50 条记录；确认两个查找菜单仍显示待实现；正常退出。
- 本次未执行增删保存全流程测试，也未独立验证哈希冲突、重复 ID 等行为；模块存在不等于已通过功能验收。
- 可执行文件输出到系统临时目录，未改动 papers.txt。

## D 下一步（按顺序）

1. 新建 search_service.h/.cpp，实现 displayPaperDetail(int paperIndex)，检查下标并显示论文全部字段。
2. 先接通菜单 6：读取并校验 ID，调用 searchHash(id)，对 -1 提示未找到，否则调用详情显示。
3. 建立统一 rebuildSearchIndexes()；当前先调用 buildHashFromPapers()，在加载、添加、删除成功后调用。A 交付后再加入 buildBSTFromPapers()，避免引用不存在的头文件导致工程无法编译。
4. 验证 ID 链路：加载后查找 1001；查找不存在 ID；添加后查找；删除后查找；删除前部记录后查找后续记录；重新加载后查找。预期删除记录不再命中、剩余记录下标正确、重载不残留旧索引。
5. A 交付 bst.h/.cpp 后接通菜单 7：使用 getline 读取含空格的完整标题，调用 searchBST(title.c_str())；其返回约定同样是顺序表下标或 -1。
6. 与 C 完成双索引联调，整理真实验收记录和报告。测试入口独立构建，避免多个 main()。

## 集成约定与待处理问题

- 菜单 6 为 ID 查找，7 为标题查找；标题采用精确匹配。
- 删除会使后续论文下标移动，必须重建全部已接入的索引；将来排序改变顺序表后也同样处理。
- 两种查找均返回 g_papers.data 下标，未找到返回 -1；只使用 paper.cpp 中唯一的 g_papers。
- editor.cpp 的数字输入失败处理尚不完整；main.cpp 对输入结束 EOF 的处理也需要完善。
- loader.cpp 尚未拒绝重复 ID；hash.cpp 对重复 ID 提示并保留第一条，后续需统一数据唯一性策略。
- README.md 仍仅有项目名，构建及运行说明尚待补充。
