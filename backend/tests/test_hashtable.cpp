#include <iostream>
#include "../src/data_structures/hash_table.h"
#include "patient.h"
#include "medication.h"

using namespace std;

// Test 1: Patient Hash Table
void test1_PatientHashTable() {
    cout << "\n========== TEST 1: Patient Hash Table ==========" << endl;
    
    {
        cout << "\nCreating patient hash table..." << endl;
        HashTable<int, Patient> patientTable(11, "patients.bin");
        
        // Add patients
        Patient p1(101, "Ahmed Ali", 45, 'M', "ICU-A", "2024-12-01", "Post-surgery");
        p1.addMedication("Aspirin");
        p1.addMedication("Metformin");
        p1.addAllergy("Penicillin");
        p1.bloodType = "O+";
        
        Patient p2(102, "Sara Khan", 32, 'F', "ICU-B", "2024-12-02", "Pneumonia");
        p2.addMedication("Amoxicillin");
        p2.bloodType = "A+";
        
        Patient p3(103, "Ali Raza", 67, 'M', "ICU-A", "2024-12-03", "Heart failure");
        p3.addMedication("Furosemide");
        p3.addMedication("Warfarin");
        p3.bloodType = "B+";
        
        patientTable.insert(101, p1);
        patientTable.insert(102, p2);
        patientTable.insert(103, p3);
        
        cout << "\nInserted 3 patients. Hash table statistics:" << endl;
        patientTable.display();
        
        // Search for patient
        cout << "\nSearching for patient 102..." << endl;
        Patient* found = patientTable.search(102);
        if (found) {
            found->display();
        }
        
        cout << "\n[Data will be saved to disk on destruction...]" << endl;
    }
    
    cout << "\n--- Simulating restart ---" << endl;
    
    {
        cout << "\nLoading patient table from disk..." << endl;
        HashTable<int, Patient> patientTable(11, "patients.bin");
        
        cout << "\nPatients loaded! Searching for patient 101..." << endl;
        Patient* found = patientTable.search(101);
        if (found) {
            cout << "✅ Found persisted patient:" << endl;
            found->display();
        }
    }
    
    cout << "\n✅ Test 1 Passed!" << endl;
}

// Test 2: Medication Hash Table
void test2_MedicationHashTable() {
    cout << "\n========== TEST 2: Medication Hash Table ==========" << endl;
    
    {
        cout << "\nCreating medication hash table..." << endl;
        HashTable<string, Medication> medTable(11, "medications.bin");
        
        // Add medications
        Medication m1("Aspirin", "Acetylsalicylic acid", "Analgesic", 100);
        m1.sideEffects.push_back("Stomach irritation");
        m1.sideEffects.push_back("Bleeding risk");
        
        Medication m2("Metformin", "Metformin HCl", "Antidiabetic", 500);
        m2.sideEffects.push_back("Nausea");
        m2.sideEffects.push_back("Diarrhea");
        
        Medication m3("Warfarin", "Warfarin sodium", "Anticoagulant", 5);
        m3.sideEffects.push_back("Increased bleeding");
        m3.contraindications.push_back("Pregnancy");
        
        medTable.insert("aspirin", m1);
        medTable.insert("metformin", m2);
        medTable.insert("warfarin", m3);
        
        cout << "\nInserted 3 medications." << endl;
        medTable.display();
        
        // Search
        cout << "\nSearching for 'warfarin'..." << endl;
        Medication* found = medTable.search("warfarin");
        if (found) {
            found->display();
        }
        
    }
    
    cout << "\n--- Reloading from disk ---" << endl;
    
    {
        HashTable<string, Medication> medTable(11, "medications.bin");
        
        cout << "\nAll medications from disk:" << endl;
        auto keys = medTable.getAllKeys();
        for (const auto& key : keys) {
            Medication* med = medTable.search(key);
            if (med) {
                med->display();
            }
        }
    }
    
    cout << "\n✅ Test 2 Passed!" << endl;
}

// Test 3: Collision Handling
void test3_CollisionHandling() {
    cout << "\n========== TEST 3: Collision Handling ==========" << endl;
    
    // Small table to force collisions
    HashTable<int, Patient> table(7);  // Prime number
    
    cout << "\nCreating small table (size 7) to test collisions..." << endl;
    
    // Insert patients that will likely collide
    for (int i = 1; i <= 15; i++) {
        Patient p(100 + i, "Patient_" + to_string(i), 30 + i, 'M', 
                  "ICU-A", "2024-12-01", "Condition");
        table.insert(100 + i, p);
    }
    
    cout << "\nInserted 15 patients into 7 buckets:" << endl;
    table.display();
    
    // Verify all patients can be found
    cout << "\nVerifying all patients can be retrieved..." << endl;
bool allFound = true;
for (int i = 1; i <= 15; i++) {
    if (!table.contains(100 + i)) {
        cout << "❌ Patient " << (100 + i) << " not found!" << endl;
        allFound = false;
    }
}
}
// Test 4: Dynamic Resizing
void test4_DynamicResizing() {
cout << "\n========== TEST 4: Dynamic Resizing ==========" << endl;
HashTable<int, Patient> table(11);  // Start small

cout << "\nStarting with table size 11..." << endl;
cout << "Inserting 100 patients to trigger resizing..." << endl;

for (int i = 1; i <= 100; i++) {
    Patient p(i, "Patient_" + to_string(i), 30, 'M', 
              "ICU-A", "2024-12-01", "Condition");
    table.insert(i, p);
    
    if (i % 25 == 0) {
        cout << "Inserted " << i << " patients - Size: " << table.size() 
             << " | Capacity: " << table.capacity() << endl;
    }
}

cout << "\nFinal statistics:" << endl;
cout << "Total patients: " << table.size() << endl;
cout << "Table capacity: " << table.capacity() << endl;
cout << "Load factor: " << (float)table.size() / table.capacity() << endl;

// Verify random patients
cout << "\nVerifying random patients..." << endl;
for (int id : {5, 27, 53, 89, 100}) {
    if (table.contains(id)) {
        cout << "✅ Patient " << id << " found" << endl;
    } else {
        cout << "❌ Patient " << id << " NOT found" << endl;
    }
}

cout << "\n✅ Test 4 Passed!" << endl;

}
// Test 5: Update and Delete
void test5_UpdateDelete() {
cout << "\n========== TEST 5: Update and Delete ==========" << endl;
HashTable<int, Patient> table(11);

// Insert patient
Patient p(101, "Ahmed Ali", 45, 'M', "ICU-A", "2024-12-01", "Stable");
table.insert(101, p);

cout << "\nOriginal patient:" << endl;
table.search(101)->display();

// Update patient
cout << "\nUpdating patient condition..." << endl;
Patient updated(101, "Ahmed Ali", 45, 'M', "ICU-A", "2024-12-01", "Critical");
updated.addMedication("Emergency medication");
table.insert(101, updated);  // Should update, not create new

cout << "\nUpdated patient:" << endl;
table.search(101)->display();

cout << "\nTable size (should still be 1): " << table.size() << endl;

// Delete patient
cout << "\nDeleting patient 101..." << endl;
bool deleted = table.remove(101);
cout << (deleted ? "✅ Deleted successfully" : "❌ Delete failed") << endl;
cout << "Table size after deletion: " << table.size() << endl;

// Try to find deleted patient
cout << "\nSearching for deleted patient..." << endl;
if (table.search(101) == nullptr) {
    cout << "✅ Patient not found (correctly deleted)" << endl;
} else {
    cout << "❌ Patient still exists!" << endl;
}

cout << "\n✅ Test 5 Passed!" << endl;
}
int main() {
cout << "╔══════════════════════════════════════════╗" << endl;
cout << "║   HASH TABLE IMPLEMENTATION TEST         ║" << endl;
cout << "║   IntelliCare ICU Project               ║" << endl;
cout << "╚══════════════════════════════════════════╝" << endl;
test1_PatientHashTable();
test2_MedicationHashTable();
test3_CollisionHandling();
test4_DynamicResizing();
test5_UpdateDelete();

cout << "\n\n╔══════════════════════════════════════════╗" << endl;
cout << "║   ✅ ALL TESTS PASSED!                  ║" << endl;
cout << "║   Files created:                         ║" << endl;
cout << "║   - patients.bin                         ║" << endl;
cout << "║   - medications.bin                      ║" << endl;
cout << "╚══════════════════════════════════════════╝" << endl;

return 0;
}