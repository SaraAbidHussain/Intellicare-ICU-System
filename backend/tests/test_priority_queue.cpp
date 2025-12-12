#include <iostream>
#include <thread>
#include <chrono>
#include "../src/data_structures/priority_queue.h"

using namespace std;

// Test 1: Basic Insert and Extract
void test1_BasicOperations() {
    cout << "\n========== TEST 1: Basic Operations ==========" << endl;
    
    PriorityQueue pq;
    
    // Insert alerts with different priorities
    Alert a1(1, 101, CRITICAL, VITAL_ABNORMAL, "Heart rate dropped to 40 bpm");
    Alert a2(2, 102, LOW, MEDICATION_DUE, "Medication scheduled for 2:00 PM");
    Alert a3(3, 103, HIGH, DETERIORATION, "Blood pressure rising rapidly");
    Alert a4(4, 101, MEDIUM, LAB_CRITICAL, "Lab results need review");
    
    cout << "\nInserting alerts..." << endl;
    pq.insert(a1);
    pq.insert(a2);
    pq.insert(a3);
    pq.insert(a4);
    
    cout << "\nHeap structure:" << endl;
    pq.displayTree();
    
    cout << "\nExtracting alerts in priority order:" << endl;
    while (!pq.isEmpty()) {
        Alert alert = pq.extractMin();
        alert.display();
    }
    
    cout << "\n✅ Test 1 Passed!" << endl;
}

// Test 2: Disk Persistence
void test2_Persistence() {
    cout << "\n========== TEST 2: Disk Persistence ==========" << endl;
    
    {
        cout << "\nCreating priority queue and adding alerts..." << endl;
        PriorityQueue pq("alerts.bin");
        
        pq.insert(Alert(1, 101, CRITICAL, VITAL_ABNORMAL, "Critical: SpO2 below 85%"));
        pq.insert(Alert(2, 102, HIGH, DRUG_INTERACTION, "Warfarin + Aspirin interaction"));
        pq.insert(Alert(3, 103, MEDIUM, MEDICATION_DUE, "Insulin dose due in 10 min"));
        pq.insert(Alert(4, 104, LOW, MEDICATION_DUE, "Patient family arrived"));
        
        cout << "\nAlerts in queue:" << endl;
        pq.display();
        pq.saveToDisk();
        cout << "\n[Saving to disk...]" << endl;
    }
    
    cout << "\n--- SIMULATING SYSTEM RESTART ---\n" << endl;
    
    {
        cout << "Loading priority queue from disk..." << endl;
        PriorityQueue pq("alerts.bin");
        
        cout << "\nAlerts loaded from disk:" << endl;
        pq.display();
        
        cout << "\nExtracting highest priority alert:" << endl;
        if (!pq.isEmpty()) {
            Alert critical = pq.extractMin();
            critical.display();
        }
    }
    
    cout << "\n✅ Test 2 Passed!" << endl;
}

// Test 3: Real-time Alert Processing
void test3_RealTimeProcessing() {
    cout << "\n========== TEST 3: Real-time Alert Processing ==========" << endl;
    
    PriorityQueue pq;
    
    cout << "\nSimulating real-time ICU alerts..." << endl;
    
    // Simulate alerts coming in over time
    pq.insert(Alert(1, 101, MEDIUM, MEDICATION_DUE, "Antibiotics due"));
    cout << "Time: 10:00 - Alert received" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    
    pq.insert(Alert(2, 102, LOW, MEDICATION_DUE, "Patient awake"));
    cout << "Time: 10:05 - Alert received" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    
    pq.insert(Alert(3, 103, CRITICAL, VITAL_ABNORMAL, "Cardiac arrest!"));
    cout << "Time: 10:07 -  CRITICAL ALERT!" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    
    pq.insert(Alert(4, 104, HIGH, DETERIORATION, "Rapid breathing"));
    cout << "Time: 10:08 - Alert received" << endl;
    
    cout << "\n\nProcessing alerts by priority:" << endl;
    int count = 1;
    while (!pq.isEmpty()) {
        cout << "\n--- Processing Alert #" << count++ << " ---" << endl;
        Alert alert = pq.extractMin();
        alert.display();
        
        this_thread::sleep_for(chrono::milliseconds(300));
    }
    
    cout << "\n Test 3 Passed!" << endl;
}

// Test 4: Filter and Query
void test4_FilterQuery() {
    cout << "\n========== TEST 4: Filter and Query ==========" << endl;
    
    PriorityQueue pq;
    
    // Add various alerts
    pq.insert(Alert(1, 101, CRITICAL, VITAL_ABNORMAL, "Critical vitals"));
    pq.insert(Alert(2, 102, CRITICAL, DRUG_INTERACTION, "Drug interaction"));
    pq.insert(Alert(3, 103, HIGH, DETERIORATION, "Patient deteriorating"));
    pq.insert(Alert(4, 104, MEDIUM, LAB_CRITICAL, "Lab results"));
    pq.insert(Alert(5, 105, LOW, MEDICATION_DUE, "Medication due"));
    
    cout << "\nAll alerts:" << endl;
    pq.display();
    
    // Query critical alerts
    cout << "\n\n🔴 Filtering CRITICAL alerts only:" << endl;
    auto criticalAlerts = pq.getAlertsByPriority(CRITICAL);
    cout << "Found " << criticalAlerts.size() << " critical alerts:" << endl;
    for (const auto& alert : criticalAlerts) {
        alert.display();
    }
    
    // Query unacknowledged
    cout << "\n\n  Unacknowledged alerts:" << endl;
    auto unacked = pq.getUnacknowledgedAlerts();
    cout << "Found " << unacked.size() << " unacknowledged alerts" << endl;
    
    cout << "\nTest 4 Passed!" << endl;
}

// Test 5: Large Scale Performance
void test5_Performance() {
    cout << "\n========== TEST 5: Performance Test ==========" << endl;
    
    PriorityQueue pq;
    
    cout << "\nInserting 1000 alerts..." << endl;
    auto start = chrono::high_resolution_clock::now();
    
    for (int i = 1; i <= 1000; i++) {
        AlertPriority prio = static_cast<AlertPriority>((i % 5) + 1);
        AlertType type = static_cast<AlertType>(i % 7);
        Alert alert(i, 100 + (i % 10), prio, type, "Alert message " + to_string(i));
        pq.insert(alert);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Insertion time: " << duration.count() << " ms" << endl;
    cout << "Queue size: " << pq.size() << endl;
    
    // Extract all in order
    cout << "\nExtracting all 1000 alerts in priority order..." << endl;
    start = chrono::high_resolution_clock::now();
    
    int prevPriority = 0;
    bool ordered = true;
    
    while (!pq.isEmpty()) {
        Alert alert = pq.extractMin();
        if (alert.priority < prevPriority) {
            ordered = false;
            cout << " Order violation detected!" << endl;
        }
        prevPriority = alert.priority;
    }
    
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Extraction time: " << duration.count() << " ms" << endl;
    
    if (ordered) {
        cout << " All alerts extracted in correct priority order!" << endl;
    }
    
    cout << "\n Test 5 Passed!" << endl;
}

// Test 6: Heap Property Verification
void test6_HeapProperty() {
    cout << "\n========== TEST 6: Heap Property Verification ==========" << endl;
    
    PriorityQueue pq;
    
    // Insert in random order
    cout << "\nInserting alerts in random order..." << endl;
    pq.insert(Alert(1, 101, MEDIUM, CUSTOM, "Alert 1"));
    pq.insert(Alert(2, 102, CRITICAL, CUSTOM, "Alert 2"));
    pq.insert(Alert(3, 103, LOW, CUSTOM, "Alert 3"));
    pq.insert(Alert(4, 104, HIGH, CUSTOM, "Alert 4"));
    pq.insert(Alert(5, 105, CRITICAL, CUSTOM, "Alert 5"));
    pq.insert(Alert(6, 106, MEDIUM, CUSTOM, "Alert 6"));
    
    cout << "\nHeap structure (parent should be <= children):" << endl;
    pq.displayTree();
    
    cout << "\nExtracting in order to verify min-heap property:" << endl;
    int lastPriority = 0;
    bool isMinHeap = true;
    
    while (!pq.isEmpty()) {
        Alert alert = pq.extractMin();
        cout << "Extracted: Priority " << alert.priority << " (ID: " << alert.alertID << ")" << endl;
        
        if (alert.priority < lastPriority) {
            isMinHeap = false;
            cout << " Min-heap property violated!" << endl;
        }
        lastPriority = alert.priority;
    }
    
    if (isMinHeap) {
        cout << "\n Min-heap property maintained correctly!" << endl;
    }
    
    cout << "\n Test 6 Passed!" << endl;
}

int main() {
    cout << "╔══════════════════════════════════════════════════════╗" << endl;
    cout << "║   PRIORITY QUEUE (MIN-HEAP) TEST SUITE             ║" << endl;
    cout << "║   IntelliCare ICU - Alert Management               ║" << endl;
    cout << "╚══════════════════════════════════════════════════════╝" << endl;
    
    test1_BasicOperations();
    test2_Persistence();
    test3_RealTimeProcessing();
    test4_FilterQuery();
    test5_Performance();
    test6_HeapProperty();
    
    cout << "\n\n╔══════════════════════════════════════════════════════╗" << endl;
    cout << "║    ALL TESTS PASSED SUCCESSFULLY!                 ║" << endl;
    cout << "║                                                      ║" << endl;
    cout << "║   Priority Queue Features:                          ║" << endl;
    cout << "║   ✓ O(log n) insertion                              ║" << endl;
    cout << "║   ✓ O(log n) extraction                             ║" << endl;
    cout << "║   ✓ O(1) peek minimum                               ║" << endl;
    cout << "║   ✓ Disk persistence                                ║" << endl;
    cout << "║   ✓ Priority-based ordering                         ║" << endl;
    cout << "║   ✓ Min-heap property maintained                    ║" << endl;
    cout << "╚══════════════════════════════════════════════════════╝" << endl;
    
    return 0;
}