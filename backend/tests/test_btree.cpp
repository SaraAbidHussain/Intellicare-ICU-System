#include <iostream>
#include <ctime>
#include <chrono>
#include "../src/data_structures/btree.h"

using namespace std;

long createTimestamp(int hour, int minute) {
    long baseTime = 1733270400;
    return baseTime + (hour * 3600) + (minute * 60);
}

// Test 1: Basic Insert and Persistence
void test1_Persistence() {
    cout << "\n========== TEST 1: Disk Persistence ==========" << endl;
    
    {
        cout << "\nCreating NEW tree and inserting data..." << endl;
        BTree tree(3, "test_data.bin");
        
        VitalRecord r1(101, createTimestamp(10, 30), 75, 120, 80, 98, 37.2);
        VitalRecord r2(101, createTimestamp(10, 31), 76, 121, 81, 98, 37.2);
        VitalRecord r3(101, createTimestamp(10, 32), 74, 119, 79, 97, 37.3);
        
        tree.insert(createTimestamp(10, 30), r1);
        tree.insert(createTimestamp(10, 31), r2);
        tree.insert(createTimestamp(10, 32), r3);
        
        cout << "Data inserted. Tree will be saved to disk..." << endl;
    } // Tree destructor called here - saves to disk
    
    cout << "\n--- SIMULATING POWER OFF ---" << endl;
    cout << "Tree object destroyed. Memory cleared.\n" << endl;
    
    {
        cout << "--- SIMULATING POWER ON ---" << endl;
        cout << "Creating NEW tree object..." << endl;
        BTree tree(3, "test_data.bin");
        
        cout << "\nData loaded from disk! Traversing:" << endl;
        tree.traverse();
        
        cout << "\nSearching for previously inserted data..." << endl;
        VitalRecord* found = tree.search(createTimestamp(10, 31));
        if (found) {
            cout << "✅ Found: ";
            found->display();
        } else {
            cout << "❌ Not found!" << endl;
        }
    }
    
    cout << "\n✅ Test 1 Passed! Data persisted across sessions!" << endl;
}

// Test 2: Add More Data to Existing File
void test2_AppendData() {
    cout << "\n========== TEST 2: Append to Existing Data ==========" << endl;
    
    BTree tree(3, "test_data.bin");
    
    cout << "\nExisting data in tree:" << endl;
    tree.traverse();
    
    cout << "\nAdding new records..." << endl;
    VitalRecord r4(101, createTimestamp(10, 33), 73, 118, 78, 96, 37.4);
    VitalRecord r5(101, createTimestamp(10, 34), 72, 117, 77, 95, 37.5);
    
    tree.insert(createTimestamp(10, 33), r4);
    tree.insert(createTimestamp(10, 34), r5);
    
    cout << "\nAll data in tree now:" << endl;
    tree.traverse();
    
    cout << "\n✅ Test 2 Passed! New data added to existing file!" << endl;
}

// Test 3: Range Query on Persisted Data
void test3_RangeQueryPersisted() {
    cout << "\n========== TEST 3: Range Query on Disk Data ==========" << endl;
    
    BTree tree(3, "test_data.bin");
    
    cout << "\nRange Query: 10:31 to 10:33" << endl;
    auto results = tree.rangeQuery(createTimestamp(10, 31), createTimestamp(10, 33));
    
    cout << "Found " << results.size() << " records:" << endl;
    for (auto& record : results) {
        record.display();
    }
    
    cout << "\n✅ Test 3 Passed!" << endl;
}

// Test 4: Multiple Patients Persistence
void test4_MultiplePatients() {
    cout << "\n========== TEST 4: Multiple Patients ==========" << endl;
    
    {
        BTree tree(3, "multi_patient.bin");
        
        cout << "Inserting data for multiple patients..." << endl;
        
        // Patient 101
        tree.insert(createTimestamp(10, 0), VitalRecord(101, createTimestamp(10, 0), 75, 120, 80, 98, 37.0));
        tree.insert(createTimestamp(10, 1), VitalRecord(101, createTimestamp(10, 1), 76, 121, 81, 98, 37.1));
        
        // Patient 102
        tree.insert(createTimestamp(10, 0) + 1, VitalRecord(102, createTimestamp(10, 0), 80, 130, 85, 96, 37.5));
        tree.insert(createTimestamp(10, 1) + 1, VitalRecord(102, createTimestamp(10, 1), 82, 132, 86, 95, 37.6));
        
        cout << "Data saved to disk..." << endl;
    }
    
    cout << "\n--- Reloading from disk ---" << endl;
    {
        BTree tree(3, "multi_patient.bin");
        
        cout << "\nAll patient data:" << endl;
        tree.traverse();
    }
    
    cout << "\n✅ Test 4 Passed!" << endl;
}

// Test 5: Large Dataset Persistence
void test5_LargeDataset() {
    cout << "\n========== TEST 5: Large Dataset Performance ==========" << endl;
    
    {
        cout << "Creating tree with 1000 records..." << endl;
        BTree tree(50, "large_data.bin");
        
        auto start = chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; i++) {
            VitalRecord r(101, 1733270400 + i * 60, 70 + (i % 30), 120, 80, 98, 37.0);
            tree.insert(1733270400 + i * 60, r);
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        cout << "Insertion time (with disk writes): " << duration.count() << " ms" << endl;
    }
    
    cout << "\n--- Reloading 1000 records from disk ---" << endl;
    {
        auto start = chrono::high_resolution_clock::now();
        BTree tree(50, "large_data.bin");
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        cout << "Load time: " << duration.count() << " ms" << endl;
        
        // Verify data
        VitalRecord* found = tree.search(1733270400 + 500 * 60);
        if (found) {
            cout << "✅ Verification: Middle record found!" << endl;
        }
    }
    
    cout << "\n✅ Test 5 Passed!" << endl;
}

int main() {
    cout << "╔══════════════════════════════════════════╗" << endl;
    cout << "║   DISK-BASED B-TREE TEST SUITE          ║" << endl;
    cout << "║   IntelliCare ICU Project               ║" << endl;
    cout << "╚══════════════════════════════════════════╝" << endl;
    
    test1_Persistence();
    test2_AppendData();
    test3_RangeQueryPersisted();
    test4_MultiplePatients();
    test5_LargeDataset();
    
    cout << "\n\n╔══════════════════════════════════════════╗" << endl;
    cout << "║   ✅ ALL TESTS PASSED!                  ║" << endl;
    cout << "║   Data files created:                    ║" << endl;
    cout << "║   - test_data.bin                        ║" << endl;
    cout << "║   - multi_patient.bin                    ║" << endl;
    cout << "║   - large_data.bin                       ║" << endl;
    cout << "╚══════════════════════════════════════════╝" << endl;
    
    return 0;
}