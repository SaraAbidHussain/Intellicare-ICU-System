#include <iostream>
#include <chrono>
#include <cassert>
#include <cstdio>
#include <vector>
#include "btree.h"

using namespace std;

// Helper to create timestamps
long createTimestamp(int hour, int minute, int second = 0) {
    long baseTime = 1733270400; // Dec 4, 2024, 00:00:00
    return baseTime + (hour * 3600) + (minute * 60) + second;
}

// Helper to clean up test files
void cleanupFiles(const string& basePath) {
    remove((basePath + "_index.dat").c_str());
    remove((basePath + "_data.dat").c_str());
    remove((basePath + "_meta.dat").c_str());
}

// ==================== TEST 1: Basic Persistence ====================
void test1_BasicPersistence() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║  TEST 1: Basic Disk Persistence               ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    string testPath = "test1_persist";
    cleanupFiles(testPath);
    
    // Phase 1: Create and insert
    {
        cout << "\n[PHASE 1] Creating new tree and inserting data..." << endl;
        DiskBTree tree(3, testPath);
        
        VitalRecord r1(101, createTimestamp(10, 30), 75, 120, 80, 98, 37.2);
        VitalRecord r2(101, createTimestamp(10, 35), 78, 125, 82, 97, 37.3);
        VitalRecord r3(101, createTimestamp(10, 40), 72, 118, 79, 99, 37.1);
        
        tree.insert(createTimestamp(10, 30), r1);
        tree.insert(createTimestamp(10, 35), r2);
        tree.insert(createTimestamp(10, 40), r3);
        
        cout << "✓ Inserted 3 records" << endl;
        cout << "✓ Tree object going out of scope..." << endl;
    }
    
    // Simulate power off
    cout << "\n[SIMULATING POWER OFF]" << endl;
    cout << "Memory cleared. Only disk files remain.\n" << endl;
    
    // Phase 2: Load from disk
    {
        cout << "[PHASE 2] Power back on - Loading from disk..." << endl;
        DiskBTree tree(3, testPath);
        
        cout << "✓ Tree loaded from disk!" << endl;
        cout << "\nSearching for record at 10:35..." << endl;
        
        VitalRecord* found = tree.search(createTimestamp(10, 35));
        if (found) {
            cout << "✅ FOUND: ";
            found->display();
            delete found; // Clean up
        } else {
            cout << "FAILED: Record not found!" << endl;
            return;
        }
        
        cout << "\nTotal records in tree: " << tree.getRecordCount() << endl;
        assert(tree.getRecordCount() == 3);
    }
    
    cout << "\n✅ TEST 1 PASSED: Data persisted correctly!" << endl;
}

// ==================== TEST 2: Append to Existing Tree ====================
void test2_AppendData() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║  TEST 2: Append to Existing Tree              ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    string testPath = "test1_persist"; // Reuse from test1
    
    {
        DiskBTree tree(3, testPath);
        cout << "\nCurrent record count: " << tree.getRecordCount() << endl;
        
        cout << "\nAdding 2 more records..." << endl;
        VitalRecord r4(101, createTimestamp(10, 45), 74, 122, 81, 98, 37.2);
        VitalRecord r5(101, createTimestamp(10, 50), 76, 121, 80, 97, 37.4);
        
        tree.insert(createTimestamp(10, 45), r4);
        tree.insert(createTimestamp(10, 50), r5);
        
        cout << "✓ Added 2 records" << endl;
        cout << "New record count: " << tree.getRecordCount() << endl;
        assert(tree.getRecordCount() == 5);
    }
    
    // Reload and verify
    {
        cout << "\nReloading to verify persistence..." << endl;
        DiskBTree tree(3, testPath);
        assert(tree.getRecordCount() == 5);
        
        VitalRecord* found = tree.search(createTimestamp(10, 50));
        assert(found != nullptr);
        cout << "✅ Latest record found: ";
        found->display();
        delete found;
    }
    
    cout << "\n✅ TEST 2 PASSED: Append works correctly!" << endl;
}

// ==================== TEST 3: Range Query ====================
void test3_RangeQuery() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║  TEST 3: Range Query                          ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    string testPath = "test3_range";
    cleanupFiles(testPath);
    
    {
        DiskBTree tree(5, testPath);
        
        cout << "\nInserting 10 records at 5-minute intervals..." << endl;
        for (int i = 0; i < 10; i++) {
            VitalRecord r(101, createTimestamp(10, i * 5), 70 + i, 120 + i, 80, 98, 37.0);
            tree.insert(createTimestamp(10, i * 5), r);
        }
        cout << "✓ Inserted 10 records" << endl;
    }
    
    {
        DiskBTree tree(5, testPath);
        
        cout << "\nQuerying range: 10:10 to 10:30" << endl;
        auto results = tree.rangeQuery(createTimestamp(10, 10), createTimestamp(10, 30));
        
        cout << "Found " << results.size() << " records:" << endl;
        for (auto& r : results) {
            r.display();
        }
        
        assert(results.size() == 5); // 10:10, 10:15, 10:20, 10:25, 10:30
        cout << "\n✓ Range query returned correct count" << endl;
    }
    
    cout << "\n✅ TEST 3 PASSED: Range query works!" << endl;
}

// ==================== TEST 4: Multiple Patients ====================
void test4_MultiplePatients() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║  TEST 4: Multiple Patients                    ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    string testPath = "test4_multi";
    cleanupFiles(testPath);
    
    {
        DiskBTree tree(5, testPath);
        
        cout << "\nInserting data for 3 patients..." << endl;
        
        // Patient 101 - timestamps at 10:00
        tree.insert(createTimestamp(10, 0, 0), VitalRecord(101, createTimestamp(10, 0), 75, 120, 80, 98, 37.0));
        tree.insert(createTimestamp(10, 5, 0), VitalRecord(101, createTimestamp(10, 5), 76, 121, 81, 98, 37.1));
        
        // Patient 102 - timestamps at 10:00 + small offset
        tree.insert(createTimestamp(10, 0, 30), VitalRecord(102, createTimestamp(10, 0), 80, 130, 85, 96, 37.5));
        tree.insert(createTimestamp(10, 5, 30), VitalRecord(102, createTimestamp(10, 5), 82, 132, 86, 95, 37.6));
        
        // Patient 103 - timestamps at 10:00 + different offset
        tree.insert(createTimestamp(10, 0, 45), VitalRecord(103, createTimestamp(10, 0), 70, 115, 75, 99, 36.8));
        tree.insert(createTimestamp(10, 5, 45), VitalRecord(103, createTimestamp(10, 5), 71, 116, 76, 99, 36.9));
        
        cout << "✓ Inserted data for 3 patients (6 total records)" << endl;
    }
    
    {
        DiskBTree tree(5, testPath);
        
        cout << "\nReloaded tree - Searching for Patient 102's first reading..." << endl;
        VitalRecord* found = tree.search(createTimestamp(10, 0, 30));
        
        if (found) {
            cout << "✅ FOUND: ";
            found->display();
            assert(found->patientID == 102);
            delete found;
        } else {
            cout << "❌ FAILED!" << endl;
            return;
        }
    }
    
    cout << "\n✅ TEST 4 PASSED: Multiple patients handled correctly!" << endl;
}

// ==================== TEST 5: Large Dataset & Performance ====================
void test5_LargeDataset() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║  TEST 5: Large Dataset (1000 records)        ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    string testPath = "test5_large";
    cleanupFiles(testPath);
    
    const int RECORD_COUNT = 1000;
    
    // Phase 1: Insertion
    {
        cout << "\n[PHASE 1] Inserting " << RECORD_COUNT << " records..." << endl;
        DiskBTree tree(50, testPath); // Higher degree for better performance
        
        auto start = chrono::high_resolution_clock::now();
        
        for (int i = 0; i < RECORD_COUNT; i++) {
            VitalRecord r(101, 1733270400 + i * 60, 70 + (i % 30), 120 + (i % 20), 80, 98, 37.0);
            tree.insert(1733270400 + i * 60, r);
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        cout << "✓ Insertion completed" << endl;
        cout << "⏱  Time: " << duration.count() << " ms" << endl;
        cout << "⏱  Average: " << (double)duration.count() / RECORD_COUNT << " ms/record" << endl;
    }
    
    cout << "\n[SIMULATING POWER OFF]\n" << endl;
    
    // Phase 2: Load and search
    {
        cout << "[PHASE 2] Loading tree from disk..." << endl;
        auto start = chrono::high_resolution_clock::now();
        DiskBTree tree(50, testPath);
        auto end = chrono::high_resolution_clock::now();
        auto loadTime = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        cout << "✓ Tree loaded" << endl;
        cout << "⏱  Load time: " << loadTime.count() << " ms" << endl;
        cout << "📊 Record count: " << tree.getRecordCount() << endl;
        
        assert(tree.getRecordCount() == RECORD_COUNT);
        
        // Search test
        cout << "\nSearching for middle record (#500)..." << endl;
        start = chrono::high_resolution_clock::now();
        VitalRecord* found = tree.search(1733270400 + 500 * 60);
        end = chrono::high_resolution_clock::now();
        auto searchTime = chrono::duration_cast<chrono::microseconds>(end - start);
        
        if (found) {
            cout << "✅ FOUND: ";
            found->display();
            cout << "⏱  Search time: " << searchTime.count() << " μs" << endl;
            delete found;
        } else {
            cout << "❌ FAILED!" << endl;
            return;
        }
        
        // Range query test
        cout << "\nRange query: 100 records..." << endl;
        start = chrono::high_resolution_clock::now();
        auto results = tree.rangeQuery(1733270400 + 400 * 60, 1733270400 + 499 * 60);
        end = chrono::high_resolution_clock::now();
        auto rangeTime = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        cout << "✓ Found " << results.size() << " records" << endl;
        cout << "⏱  Range query time: " << rangeTime.count() << " ms" << endl;
        assert(results.size() == 100);
    }
    
    cout << "\n✅ TEST 5 PASSED: Large dataset handled efficiently!" << endl;
}

// ==================== TEST 6: B-Tree Node Splitting ====================
void test6_NodeSplitting() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║  TEST 6: B-Tree Node Splitting               ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    string testPath = "test6_split";
    cleanupFiles(testPath);
    
    {
        cout << "\nCreating tree with degree 3 (max 5 keys)..." << endl;
        DiskBTree tree(3, testPath);
        
        cout << "Inserting 10 records to force splits..." << endl;
        for (int i = 0; i < 10; i++) {
            VitalRecord r(101, createTimestamp(10, i), 70 + i, 120, 80, 98, 37.0);
            tree.insert(createTimestamp(10, i), r);
            cout << "  Inserted record #" << (i + 1) << endl;
        }
        
        cout << "\n✓ All records inserted (tree structure adapted via splits)" << endl;
    }
    
    {
        cout << "\nReloading and verifying all records..." << endl;
        DiskBTree tree(3, testPath);
        
        int foundCount = 0;
        for (int i = 0; i < 10; i++) {
            VitalRecord* found = tree.search(createTimestamp(10, i));
            if (found) {
                foundCount++;
                delete found;
            }
        }
        
        cout << "✓ Found " << foundCount << "/10 records" << endl;
        assert(foundCount == 10);
    }
    
    cout << "\n✅ TEST 6 PASSED: Node splitting works correctly!" << endl;
}

// ==================== TEST 7: Edge Cases ====================
void test7_EdgeCases() {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║  TEST 7: Edge Cases                           ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    
    string testPath = "test7_edge";
    cleanupFiles(testPath);
    
    {
        DiskBTree tree(5, testPath);
        
        // Test 1: Empty tree search
        cout << "\n[Test 7.1] Searching in empty tree..." << endl;
        VitalRecord* found = tree.search(createTimestamp(10, 0));
        assert(found == nullptr);
        cout << "✓ Returns nullptr for empty tree" << endl;
        
        // Test 2: Single record
        cout << "\n[Test 7.2] Single record..." << endl;
        VitalRecord r1(101, createTimestamp(10, 0), 75, 120, 80, 98, 37.0);
        tree.insert(createTimestamp(10, 0), r1);
        found = tree.search(createTimestamp(10, 0));
        assert(found != nullptr);
        cout << "✓ Single record insertion and retrieval works" << endl;
        delete found;
        
        // Test 3: Non-existent key
        cout << "\n[Test 7.3] Searching for non-existent key..." << endl;
        found = tree.search(createTimestamp(11, 0));
        assert(found == nullptr);
        cout << "✓ Returns nullptr for non-existent key" << endl;
        
        // Test 4: Empty range query
        cout << "\n[Test 7.4] Range query with no matches..." << endl;
        auto results = tree.rangeQuery(createTimestamp(12, 0), createTimestamp(13, 0));
        assert(results.size() == 0);
        cout << "✓ Returns empty vector for range with no matches" << endl;
    }
    
    cout << "\n✅ TEST 7 PASSED: Edge cases handled correctly!" << endl;
}

// ==================== MAIN ====================
int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                      ║" << endl;
    cout << "║     DISK-BASED B-TREE COMPREHENSIVE TEST SUITE      ║" << endl;
    cout << "║           IntelliCare ICU Monitoring System          ║" << endl;
    cout << "║                                                      ║" << endl;
    cout << "╚══════════════════════════════════════════════════════╝" << endl;
    
    try {
        test1_BasicPersistence();
        test2_AppendData();
        test3_RangeQuery();
        test4_MultiplePatients();
        test5_LargeDataset();
        test6_NodeSplitting();
        test7_EdgeCases();
        
        cout << "\n\n";
        cout << "╔══════════════════════════════════════════════════════╗" << endl;
        cout << "║                                                      ║" << endl;
        cout << "║              ✅ ALL TESTS PASSED! ✅                 ║" << endl;
        cout << "║                                                      ║" << endl;
        cout << "║  Test files created:                                 ║" << endl;
        cout << "║  • test1_persist_*.dat                               ║" << endl;
        cout << "║  • test3_range_*.dat                                 ║" << endl;
        cout << "║  • test4_multi_*.dat                                 ║" << endl;
        cout << "║  • test5_large_*.dat                                 ║" << endl;
        cout << "║  • test6_split_*.dat                                 ║" << endl;
        cout << "║  • test7_edge_*.dat                                  ║" << endl;
        cout << "║                                                      ║" << endl;
        cout << "║  Your disk-based B-tree is working correctly!       ║" << endl;
        cout << "║                                                      ║" << endl;
        cout << "╚══════════════════════════════════════════════════════╝" << endl;
        
    } catch (const exception& e) {
        cout << "\n❌ TEST FAILED WITH EXCEPTION: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}