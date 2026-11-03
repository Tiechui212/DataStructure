# DataStructure

## 编译与运行

在项目目录执行（需要 g++，C++11）：

```powershell
g++ -std=c++11 -Wall -Wextra -pedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp paper.cpp loader.cpp editor.cpp storage.cpp hash.cpp search_service.cpp -o app.exe
.\app.exe
```

菜单 1 从当前目录加载 `papers.txt`，菜单 6 按整数 ID 精确查找并显示全部详情。
加载、添加、删除成功后自动重建 ID 哈希索引；菜单 5 保存到 `papers_saved.txt`。
标题 BST 和菜单 7 尚待接入。

## ID 查找流程测试

```powershell
python tests/id_flow_test.py .\app.exe
```

测试使用临时目录和数据副本，覆盖未加载、命中、未命中、非法输入、增删、
哈希冲突、删除后的下标移动、重载、空数据和 EOF，不修改原始数据。
