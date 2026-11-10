#include <iostream>
#include <string>
#include <limits> // For numeric_limits
#include <sstream>

#include "bst.h"
#include "editor.h"
#include "loader.h"
#include "paper.h"
#include "storage.h"
#include "hash.h"
#include "search_service.h"


//使用模块化编程的思想，每个功能都应被视为一个独立的模块，
//实现在不同的源文件和头文件中，并通过 include 语句引入

// 在这里引入你实现的功能模块头文件，如：
//#include "Paper.h"


// 可在这里自由添加全局常量\变量
// #define MAX_PAPERS 1000   // 全局顺序表的最大论文数量

void displayMenu() {
    std::cout << "\n--- 学术文献管理与关联分析系统 ---" << std::endl;
    std::cout << "1. 加载论文数据" << std::endl;
    std::cout << "2. 显示所有论文 (简要信息)" << std::endl;
    std::cout << "3. 添加新论文" << std::endl;
    std::cout << "4. 删除论文" << std::endl;
    std::cout << "5. 保存当前数据到文件" << std::endl;
    std::cout << "6. 按ID查找论文" << std::endl;
    std::cout << "7. 按标题查找论文 (BST)" << std::endl;
    std::cout << "8. 按年份降序排序并显示" << std::endl;
    std::cout << "9. 按引用次数降序排序并显示" << std::endl;
    std::cout << "10. 按标题字母顺序排序并显示" << std::endl;
    std::cout << "11. 合并两个有序论文列表" << std::endl;
    std::cout << "12. 查找相关论文 (BFS)" << std::endl;

    std::cout << "0. 退出" << std::endl;
    std::cout << "请输入您的选择: ";
}

int main() {//主函数中可以自由添加局部变量、交互输入信息
    int choice;
    bool dataLoaded = false;

    do {
        displayMenu();
        std::cin >> choice;

        // --- 输入验证和错误处理 ---
        if (std::cin.fail()) { // 如果输入失败（例如输入了非数字字符）
            if (std::cin.eof()) return 0;
            std::cout << "无效的输入，请输入一个数字选项。" << std::endl;
            std::cin.clear(); // 清除错误标志
            // 忽略并丢弃当前行中所有剩余的无效输入，直到遇到换行符
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1; // 将 choice 设置为一个无效值，以便循环继续
            continue;    // 跳过本次循环的剩余部分，重新显示菜单
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: { // 加载论文数据，数据文件名默认为 papers.txt
                if (loadPapers("papers.txt")) {
                    rebuildSearchIndexes();
                    dataLoaded = true; // 仅在加载成功后设置标志
                    std::cout << "论文数据加载成功！当前共有 "
                              << g_papers.length << " 篇论文。" << std::endl;
                } else {
                    std::cout << "论文数据加载失败，请检查 papers.txt。"
                              << std::endl;
                }
                break;
            }
            case 2: {// 显示所有论文 (简要信息)
                if (dataLoaded) {
                    displayPapers();
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 3: { // 添加新论文
                if (dataLoaded) {
                    if (addPaperInteractive()) {
                        rebuildSearchIndexes();
                        std::cout << "当前共有 " << g_papers.length
                                  << " 篇论文。" << std::endl;
                    } else {
                        std::cout << "添加操作未完成，论文数量不变。"
                                  << std::endl;
                    }
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 4: { // 删除论文
                if (dataLoaded) {
                    int id;
                    std::cout << "请输入要删除的论文 ID: ";
                    std::cin >> id;
                    if (std::cin.fail()) {
                        std::cout << "无效的 ID，删除操作已取消。" << std::endl;
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        break;
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (deletePaperById(id)) {
                        rebuildSearchIndexes();
                        std::cout << "当前共有 " << g_papers.length
                                  << " 篇论文。" << std::endl;
                    } else {
                        std::cout << "未删除任何论文，论文数量不变。"
                                  << std::endl;
                    }
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 5: { // 保存当前数据到文件
                if (dataLoaded) {
                    const std::string outputFile = "papers_saved.txt";
                    if (savePapers(outputFile)) {
                        std::cout << "当前数据已保存到 " << outputFile << "。"
                                  << std::endl;
                    } else {
                        std::cout << "保存数据失败。" << std::endl;
                    }
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 6: { // 按ID查找论文
                if (dataLoaded) {

                    std::cout << "请输入要查找的论文 ID: ";
                    std::string line;
                    if (!std::getline(std::cin, line)) return 0;
                    std::istringstream input(line);
                    int id;
                    if (!(input >> id) || !(input >> std::ws).eof()) {
                        std::cout << "无效的 ID，请输入整数。" << std::endl;
                        break;
                    }
                    const int paperIndex = searchHash(id);
                    if (paperIndex == -1) {
                        std::cout << "未找到 ID 为 " << id << " 的论文。" << std::endl;
                    } else {
                        displayPaperDetail(paperIndex);
                    }

                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 7: { // 按标题查找论文 (BST)
                if (dataLoaded) {
                    std::cout << "请输入要查找的完整标题: ";
                    std::string title;
                    if (!std::getline(std::cin, title)) return 0;
                    const int paperIndex = searchBST(title.c_str());
                    if (paperIndex == -1) {
                        std::cout << "未找到标题为 " << title << " 的论文。" << std::endl;
                    } else {
                        displayPaperDetail(paperIndex);
                    }
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 8: // 按年份降序排序并显示
                if (dataLoaded) {
                    //ToDo: 在此处调用按年份降序排序并显示的功能函数
                    std::cout << "按年份降序排序并显示功能待实现。" << std::endl;
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            case 9: // 按引用次数降序排序并显示
                if (dataLoaded) {
                   //ToDo: 在此处调用按引用次数排序并显示的功能函数
                   std::cout << "按引用次数降序排序并显示功能待实现。" << std::endl;
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            case 10: // 按标题字母顺序排序并显示
                if (dataLoaded) {
                    //ToDo: 在此处调用按标题字母顺序排序并显示的功能函数
                    std::cout << "按标题字母顺序排序并显示功能待实现。" << std::endl;
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            case 11: { // 合并两个有序论文列表
                if (dataLoaded) {
                    //ToDo: 在此处调用合并两个有序论文列表的功能函数
                    std::cout << "合并两个有序论文列表功能待实现。" << std::endl;
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 12: { // 查找相关论文 (BFS)
                if (dataLoaded) {
                    //ToDo: 在此处调用查找相关论文的功能函数
                    std::cout << "查找相关论文 (BFS) 功能待实现。" << std::endl;
                } else {
                    std::cout << "请先加载论文数据。" << std::endl;
                }
                break;
            }
            case 0:
                std::cout << "感谢使用，再见！" << std::endl;
                break;
            default:
                std::cout << "无效的选择，请重新输入。" << std::endl;
                break;
        }
    } while (choice != 0);

    return 0;
}
