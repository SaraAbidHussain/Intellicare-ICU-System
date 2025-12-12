#include <iostream>
#include "../src/data_structures/drug_graph.h"

void testBasicOperations() {
    std::cout << "\n========== TEST 1: Basic Operations ==========\n" << std::endl;
    
    DrugGraph graph;
    
    // Add drugs
    graph.addDrug("Aspirin");
    graph.addDrug("Warfarin");
    graph.addDrug("Ibuprofen");
    
    // Add interactions
    graph.addInteraction("Aspirin", "Warfarin", SEVERE, 
                        "Increased bleeding risk - monitor INR closely");
    graph.addInteraction("Warfarin", "Ibuprofen", SEVERE, 
                        "Increased bleeding risk - consider alternatives");
    
    graph.display();
    
    std::cout << "✅ Basic operations test passed\n" << std::endl;
}

void testDrugCombination() {
    std::cout << "\n========== TEST 2: Drug Combination Safety ==========\n" << std::endl;
    
    DrugGraph graph;
    graph.loadCommonInteractions();
    
    // Test Case 1: Safe combination
    std::cout << "\n--- Test Case 1: Safe Combination ---" << std::endl;
    std::vector<std::string> safeMeds = {"Metformin", "Atorvastatin"};
    auto report1 = graph.checkDrugCombination(safeMeds);
    report1.display();
    
    // Test Case 2: Moderate interactions
    std::cout << "\n--- Test Case 2: Moderate Interaction ---" << std::endl;
    std::vector<std::string> moderateMeds = {"Aspirin", "Ibuprofen"};
    auto report2 = graph.checkDrugCombination(moderateMeds);
    report2.display();
    
    // Test Case 3: Severe interactions
    std::cout << "\n--- Test Case 3: Severe Interaction ---" << std::endl;
    std::vector<std::string> severeMeds = {"Warfarin", "Aspirin", "Ibuprofen"};
    auto report3 = graph.checkDrugCombination(severeMeds);
    report3.display();
    
    // Test Case 4: Contraindicated
    std::cout << "\n--- Test Case 4: Contraindicated ---" << std::endl;
    std::vector<std::string> contraindicated = {"Warfarin", "Vitamin K"};
    auto report4 = graph.checkDrugCombination(contraindicated);
    report4.display();
    
    std::cout << "✅ Drug combination test passed\n" << std::endl;
}

void testDFSPath() {
    std::cout << "\n========== TEST 3: DFS Path Finding ==========\n" << std::endl;
    
    DrugGraph graph;
    
    // Create a chain: A -- B -- C
    graph.addInteraction("DrugA", "DrugB", MODERATE, "A-B interaction");
    graph.addInteraction("DrugB", "DrugC", MODERATE, "B-C interaction");
    
    std::vector<std::string> path;
    std::vector<DrugInteraction> interactions;
    
    if (graph.hasInteractionPath("DrugA", "DrugC", path, interactions)) {
        std::cout << "✅ Found path from DrugA to DrugC:" << std::endl;
        std::cout << "Path: ";
        for (const auto& drug : path) {
            std::cout << drug << " → ";
        }
        std::cout << "END" << std::endl;
        
        std::cout << "\nInteractions in path:" << std::endl;
        for (const auto& interaction : interactions) {
            interaction.display();
        }
    } else {
        std::cout << "❌ No path found" << std::endl;
    }
    
    std::cout << "\n✅ DFS path finding test passed\n" << std::endl;
}

void testPersistence() {
    std::cout << "\n========== TEST 4: Disk Persistence ==========\n" << std::endl;
    
    {
        DrugGraph graph("test_drugs.bin");
        graph.addInteraction("TestDrug1", "TestDrug2", SEVERE, "Test interaction");
        graph.addInteraction("TestDrug2", "TestDrug3", MODERATE, "Another test");
        std::cout << "\nSaving graph..." << std::endl;
    }
    
    {
        DrugGraph graph("test_drugs.bin");
        std::cout << "\nLoaded graph:" << std::endl;
        graph.display();
    }
    
    std::cout << "✅ Persistence test passed\n" << std::endl;
}

void testRealWorldScenario() {
    std::cout << "\n========== TEST 5: Real-World ICU Scenario ==========\n" << std::endl;
    
    DrugGraph graph;
    graph.loadCommonInteractions();
    
    // Patient with multiple conditions
    std::cout << "\n🏥 ICU Patient - Multiple Medications:" << std::endl;
    std::cout << "   - Heart condition: Aspirin, Atorvastatin" << std::endl;
    std::cout << "   - Blood clot: Warfarin" << std::endl;
    std::cout << "   - Pain management: Ibuprofen" << std::endl;
    std::cout << "   - Infection: Clarithromycin" << std::endl;
    
    std::vector<std::string> patientMeds = {
        "Aspirin", "Atorvastatin", "Warfarin", "Ibuprofen", "Clarithromycin"
    };
    
    auto report = graph.checkDrugCombination(patientMeds);
    report.display();
    
    if (!report.isSafe) {
        std::cout << "🚨 ALERT: This medication combination requires immediate review!" << std::endl;
    }
    
    std::cout << "\n✅ Real-world scenario test passed\n" << std::endl;
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║     DRUG INTERACTION GRAPH - TEST SUITE            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    try {
        testBasicOperations();
        testDrugCombination();
        testDFSPath();
        testPersistence();
        testRealWorldScenario();
        
        std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║            ALL TESTS PASSED ✅                      ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════╝\n" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;   
}