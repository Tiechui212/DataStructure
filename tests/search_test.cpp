#include <cstring>
#include <iostream>

#include "bst.h"
#include "hash.h"
#include "loader.h"
#include "paper.h"

namespace {

int passed = 0;
int failed = 0;

void report(const char* name, bool success) {
    std::cout << '[' << (success ? "PASS" : "FAIL") << "] " << name << '\n';
    success ? ++passed : ++failed;
}

void clearPapersForTest() {
    g_papers.length = 0;
}

bool addPaperForTest(const Paper& paper) {
    if (g_papers.length < 0 || g_papers.length >= MAX_PAPERS) {
        return false;
    }
    g_papers.data[g_papers.length++] = paper;
    return true;
}

bool deletePaperByIdForTest(int id) {
    for (int i = 0; i < g_papers.length; ++i) {
        if (g_papers.data[i].id != id) {
            continue;
        }
        for (int j = i + 1; j < g_papers.length; ++j) {
            g_papers.data[j - 1] = g_papers.data[j];
        }
        --g_papers.length;
        return true;
    }
    return false;
}

void rebuildBothIndexes() {
    buildBSTFromPapers();
    buildHashFromPapers();
}

int linearSearchByTitle(const char* title) {
    if (title == nullptr) {
        return -1;
    }
    for (int i = 0; i < g_papers.length; ++i) {
        if (std::strcmp(g_papers.data[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int linearSearchById(int id) {
    for (int i = 0; i < g_papers.length; ++i) {
        if (g_papers.data[i].id == id) {
            return i;
        }
    }
    return -1;
}

bool allIndexedResultsMatchLinearSearch() {
    for (int i = 0; i < g_papers.length; ++i) {
        const Paper& paper = g_papers.data[i];
        if (searchBST(paper.title) != linearSearchByTitle(paper.title) ||
            searchHash(paper.id) != linearSearchById(paper.id)) {
            std::cout << "Mismatch at index " << i << ": id=" << paper.id
                      << ", title=" << paper.title << '\n';
            return false;
        }
    }
    return true;
}

}  // namespace

int main() {
    std::cout << "=== BST title search + hash ID search tests ===\n";

    clearPapersForTest();
    rebuildBothIndexes();
    report("empty table returns not found",
           searchBST("any title") == -1 && searchHash(9999) == -1);

    if (!loadPapers("papers.txt")) {
        std::cerr << "[FATAL] Cannot load papers.txt from the working directory.\n";
        return 2;
    }
    rebuildBothIndexes();
    const int originalCount = g_papers.length;
    report("loaded data matches linear search", allIndexedResultsMatchLinearSearch());
    report("missing title and ID return not found",
           searchBST("This Title Does Not Exist 123456") == -1 &&
           searchHash(-9999) == -1);

    Paper collisionA{};
    collisionA.id = 70001;
    std::memcpy(collisionA.title, "Hash Collision Paper A",
                sizeof("Hash Collision Paper A"));
    Paper collisionB{};
    collisionB.id = 70102;  // 70102 - 70001 == 101, so both use the same bucket.
    std::memcpy(collisionB.title, "Hash Collision Paper B",
                sizeof("Hash Collision Paper B"));
    const bool collisionDataAdded = addPaperForTest(collisionA) &&
                                    addPaperForTest(collisionB);
    rebuildBothIndexes();
    report("hash collision keeps both IDs searchable",
           collisionDataAdded &&
           searchHash(collisionA.id) == linearSearchById(collisionA.id) &&
           searchHash(collisionB.id) == linearSearchById(collisionB.id));
    deletePaperByIdForTest(collisionA.id);
    deletePaperByIdForTest(collisionB.id);
    rebuildBothIndexes();

    Paper duplicateTitle{};
    duplicateTitle.id = 77777;
    std::memcpy(duplicateTitle.title, g_papers.data[0].title,
                sizeof(duplicateTitle.title));
    const bool duplicateAdded = addPaperForTest(duplicateTitle);
    rebuildBothIndexes();
    report("duplicate title keeps the first paper index",
           duplicateAdded && searchBST(duplicateTitle.title) == 0 &&
           searchBST(duplicateTitle.title) == linearSearchByTitle(duplicateTitle.title) &&
           searchHash(duplicateTitle.id) == linearSearchById(duplicateTitle.id));
    deletePaperByIdForTest(duplicateTitle.id);
    rebuildBothIndexes();

    Paper added{};
    added.id = 88888;
    std::memcpy(added.title, "Test New Paper For Search Tester",
                sizeof("Test New Paper For Search Tester"));
    std::memcpy(added.authors, "Tester", sizeof("Tester"));
    const bool addedOk = addPaperForTest(added);
    rebuildBothIndexes();
    report("search is correct after add",
           addedOk && searchBST(added.title) == linearSearchByTitle(added.title) &&
           searchHash(added.id) == linearSearchById(added.id));

    const bool deletedOk = deletePaperByIdForTest(added.id);
    rebuildBothIndexes();
    report("search is correct after delete",
           deletedOk && searchBST(added.title) == -1 && searchHash(added.id) == -1 &&
           g_papers.length == originalCount && allIndexedResultsMatchLinearSearch());

    Paper transient{};
    transient.id = 99999;
    std::memcpy(transient.title, "Transient Paper Removed By Reload",
                sizeof("Transient Paper Removed By Reload"));
    addPaperForTest(transient);
    rebuildBothIndexes();
    const bool transientWasIndexed = searchBST(transient.title) != -1 &&
                                     searchHash(transient.id) != -1;

    const bool reloaded = loadPapers("papers.txt");
    rebuildBothIndexes();
    report("search is correct after reload",
           transientWasIndexed && reloaded && g_papers.length == originalCount &&
           searchBST(transient.title) == -1 && searchHash(transient.id) == -1 &&
           allIndexedResultsMatchLinearSearch());

    std::cout << "=== Result: " << passed << " passed, " << failed << " failed ===\n";
    return failed == 0 ? 0 : 1;
}
