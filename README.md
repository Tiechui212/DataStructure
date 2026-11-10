# DataStructure

## 编译与运行

在项目目录执行（需要 g++，C++11）：

```powershell
g++ -std=c++11 -Wall -Wextra -pedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp paper.cpp loader.cpp editor.cpp storage.cpp hash.cpp bst.cpp search_service.cpp -o app.exe
.\app.exe
```

菜单 1 从当前目录加载 `papers.txt`。菜单 6 按整数 ID 查找，菜单 7 按完整标题精确查找，两者均显示论文详情。
标题查找区分大小写，重复标题返回顺序表中的第一篇。
加载、添加、删除成功后自动重建 ID 哈希索引和标题 BST。菜单 5 保存到 `papers_saved.txt`。

### Windows 控制台中文乱码

程序输出 UTF-8。如果菜单中的中文显示为乱码，先退出当前程序，再在包含 `app.exe` 和 `papers.txt` 的项目目录中执行下面的命令。VS Code 的 PowerShell 终端也可以直接执行。

```powershell
cmd.exe /d /c "chcp 65001 >nul && .\app.exe"
```

这条命令先将当前控制台的代码页切换为 UTF-8（65001），成功后再启动程序。已有的 `app.exe` 可以直接使用，无需重新编译。

## ID 查找流程测试

```powershell
python tests/id_flow_test.py .\app.exe
```

测试使用临时目录和数据副本，覆盖未加载、命中、未命中、非法输入、增删、
哈希冲突、删除后的下标移动、重载、空数据和 EOF，不修改原始数据。
