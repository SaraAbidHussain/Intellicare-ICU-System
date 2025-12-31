#include <iostream>
#include <string>
#include <mutex>
#include "../include/httplib.h"
#include "../include/json.hpp"
#include "data_structures/btree.h"
#include "data_structures/priority_queue.h"
#include "data_structures/hash_table.h"
#include "data_structures/drug_graph.h"
#include "data_structures/sliding_window.h"
#include "data_structures/kdtree.h"
#include "models/vital_record.h"
#include "models/patient.h"
#include "models/alert.h"
#include "simulation/vital_simulator.h"
#include "analysis/vital_analyzer.h"


using namespace httplib;
using json = nlohmann::json;

// Global data structures
DiskBTree* vitalSignsDB;
HashTable<int, Patient>* patientDB;
PriorityQueue* alertQueue;
DrugGraph* drugInteractionGraph;
KDTree* patientClusteringTree;
SlidingWindow* continuousMonitoring;
VitalSimulator* vitalSimulator;
VitalAnalyzer* vitalAnalyzer;

std::mutex globalDataMutex;  // Protects all shared data structures

// Convert VitalRecord to JSON
json vitalToJson(const VitalRecord& v) {
    return {
        {"patientID", v.patientID},
        {"timestamp", v.timestamp},
        {"heart_rate", v.heart_rate},
        {"systolic_bp", v.systolic_bp},
        {"diastolic_bp", v.diastolic_bp},
        {"spo2", v.spo2},
        {"temperature", v.temperature}
    };
}

// Convert Alert to JSON
json alertToJson(const Alert& a) {
    return {
        {"alertID", a.alertID},
        {"patientID", a.patientID},
        {"priority", a.priority},
        {"priorityString", a.getPriorityString()},
        {"message", a.message},
        {"timestamp", a.timestamp}
    };
}

// Convert Patient to JSON
json patientToJson(const Patient& p) {
    json medications = json::array();
    for (const auto& med : p.medications) {
        medications.push_back(med);
    }
    
    return {
        {"patientID", p.patientID},
        {"name", p.name},
        {"age", p.age},
        {"gender", std::string(1, p.gender)},
        {"ward", p.ward},
        {"condition", p.condition},
        {"bloodType", p.bloodType},
        {"medications", medications}
    };
}

// void enableCORS(Response& res) {
//     res.set_header("Access-Control-Allow-Origin", "*");
//     res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
//     res.set_header("Access-Control-Allow-Headers", "Content-Type");
// }
void enableCORS(Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    res.set_header("Access-Control-Allow-Credentials", "false");
    res.set_header("Access-Control-Max-Age", "86400"); // 24 hours
}
void setupPhase1Systems() {
    vitalSimulator = new VitalSimulator();
    vitalAnalyzer = new VitalAnalyzer();
    
    // Set callback for simulator to auto-save vitals
    vitalSimulator->setCallback([](const VitalRecord& record) {
        // ADD MUTEX LOCK HERE:
        std::lock_guard<std::mutex> lock(globalDataMutex);
        
        // Auto-save simulated vitals to B-tree
        vitalSignsDB->insert(record.timestamp, record);
        
        // Add to sliding window
        continuousMonitoring->addReading(record.patientID, record);
        
        // Add to KD-tree
        patientClusteringTree->insert(record);
        
        // Check for abnormalities and create alerts
        std::vector<std::string> abnormalities;
        if (vitalAnalyzer->checkAbnormal(record, abnormalities)) {
            static int alertID = 1000;
            Alert alert(alertID++, record.patientID, HIGH, VITAL_ABNORMAL,
                       "Abnormal vitals detected: " + abnormalities[0]);
            alertQueue->insert(alert);
        }
        
        std::cout << "[SIM] Generated vitals for patient " << record.patientID << std::endl;
    });
    
    std::cout << "[PHASE1] Simulation and analysis systems initialized" << std::endl;
}

int main() {
    vitalSignsDB = new DiskBTree(50, "vitals");
    patientDB = new HashTable<int, Patient>(101, "patients.bin");
    alertQueue = new PriorityQueue("alerts.bin");
    drugInteractionGraph = new DrugGraph("drug_interactions.bin");
    drugInteractionGraph->loadCommonInteractions();
    patientClusteringTree = new KDTree(5, "patient_clustering.bin");
    continuousMonitoring = new SlidingWindow(100, "sliding_windows.bin");
    
    Server svr;
    
    // svr.Options(R"(.*)", [](const Request& req, Response& res) {
    //     res.set_header("Access-Control-Allow-Origin", "*");
    //     res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    //     res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    //     res.status = 204;
    // });

    // KEEP THIS ONE:
    svr.Options(R"(.*)", [](const Request& req, Response& res) {
        enableCORS(res);
        res.status = 204;
    });

    std::cout << "╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║   IntelliCare ICU - Backend API         ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    
    // Health check
    svr.Get("/", [](const Request& req, Response& res) {
        enableCORS(res);
        json response = {
            {"status", "online"},
            {"message", "IntelliCare ICU API"},
            {"version", "1.0.0"}
        };
        res.set_content(response.dump(), "application/json");
    });
    
        svr.Post("/api/vitals", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            auto jsonData = json::parse(req.body);
            VitalRecord record;
            record.patientID = jsonData["patientID"];
            record.timestamp = jsonData["timestamp"];
            record.heart_rate = jsonData["heart_rate"];
            record.systolic_bp = jsonData["systolic_bp"];
            record.diastolic_bp = jsonData["diastolic_bp"];
            record.spo2 = jsonData["spo2"];
            record.temperature = jsonData["temperature"];
            
            // ADD MUTEX LOCK:
            {
                std::lock_guard<std::mutex> lock(globalDataMutex);
                vitalSignsDB->insert(record.timestamp, record);
                continuousMonitoring->addReading(record.patientID, record);
                patientClusteringTree->insert(record);
            }
            
            json response = {{"status", "success"}, {"message", "Vitals recorded"}};
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
    
    // GET /api/vitals/:id
    svr.Get(R"(^/api/vitals/(\d+)$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            long startTime = 0;
            long endTime = time(nullptr);
            
            if (req.has_param("start")) startTime = std::stol(req.get_param_value("start"));
            if (req.has_param("end")) endTime = std::stol(req.get_param_value("end"));
            
            auto readings = vitalSignsDB->rangeQuery(startTime, endTime);
            json results = json::array();
            
            for (const auto& reading : readings) {
                if (reading.patientID == patientID) {
                    results.push_back(vitalToJson(reading));
                }
            }
            
            json response = {{"status", "success"}, {"count", results.size()}, {"readings", results}};
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
        // GET /api/vitals/timestamp/:timestamp - Get vital by exact timestamp
        svr.Get(R"(^/api/vitals/timestamp/(\d+)$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            long timestamp = std::stol(req.matches[1]);
            
            VitalRecord record;
            bool found;
            
            {
                std::lock_guard<std::mutex> lock(globalDataMutex);
                found = vitalSignsDB->search(timestamp, record);
            }
            
            if (found) {
                json response = {
                    {"status", "success"},
                    {"reading", vitalToJson(record)}
                };
                res.set_content(response.dump(), "application/json");
            } else {
                json error = {
                    {"status", "error"},
                    {"message", "No vital record found for this timestamp"}
                };
                res.status = 404;
                res.set_content(error.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
    svr.Post("/api/patient", [](const Request& req, Response& res) {
    enableCORS(res);
    try {
        auto jsonData = json::parse(req.body);
        Patient patient;
        patient.patientID = jsonData["patientID"];
        patient.name = jsonData["name"];
        patient.age = jsonData["age"];
        patient.gender = jsonData["gender"].get<std::string>()[0];
        patient.ward = jsonData["ward"];
        patient.condition = jsonData["condition"];
        patient.admissionDate = jsonData["admissionDate"];
        if (jsonData.contains("bloodType")) patient.bloodType = jsonData["bloodType"];
        
        // ADD THIS BLOCK - Load medications
        if (jsonData.contains("medications") && jsonData["medications"].is_array()) {
            for (const auto& med : jsonData["medications"]) {
                patient.medications.push_back(med.get<std::string>());
            }
        }
        
        patientDB->insert(patient.patientID, patient);
        
        json response = {{"status", "success"}, {"message", "Patient added"}};
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        json error = {{"status", "error"}, {"message", e.what()}};
        res.status = 400;
        res.set_content(error.dump(), "application/json");
    }
});
    
    // GET /api/patient/:id
    svr.Get(R"(/api/patient/(\d+))", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            Patient* patient = patientDB->search(patientID);
            
            if (patient) {
                json response = {{"status", "success"}, {"data", patientToJson(*patient)}};
                res.set_content(response.dump(), "application/json");
            } else {
                json error = {{"status", "error"}, {"message", "Patient not found"}};
                res.status = 404;
                res.set_content(error.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
    
    // GET /api/patients
    svr.Get("/api/patients", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            auto patientIDs = patientDB->getAllKeys();
            json patients = json::array();
            
            for (int id : patientIDs) {
                Patient* p = patientDB->search(id);
                if (p) patients.push_back(patientToJson(*p));
            }
            
            json response = {{"status", "success"}, {"count", patients.size()}, {"patients", patients}};
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 500;
            res.set_content(error.dump(), "application/json");
        }
    });
    
    // POST /api/alert
    svr.Post("/api/alert", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            auto jsonData = json::parse(req.body);
            static int alertIDCounter = 1;
            
            Alert alert;
            alert.alertID = alertIDCounter++;
            alert.patientID = jsonData["patientID"];
            alert.priority = static_cast<AlertPriority>(jsonData["priority"].get<int>());
            alert.type = static_cast<AlertType>(jsonData["type"].get<int>());
            alert.message = jsonData["message"];
            alert.timestamp = time(nullptr);
            
            alertQueue->insert(alert);
            
            json response = {{"status", "success"}, {"message", "Alert created"}};
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
    
        // GET /api/alerts - Get all alerts without removing them
    svr.Get("/api/alerts", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            json alerts = json::array();
            
            // Get all alerts without modifying the queue
            std::vector<Alert> allAlerts;
            {
                std::lock_guard<std::mutex> lock(globalDataMutex);
                allAlerts = alertQueue->getAllAlertsSorted();
            }
            
            // Convert to JSON
            for (const auto& alert : allAlerts) {
                alerts.push_back(alertToJson(alert));
            }
            
            json response = {
                {"status", "success"},
                {"count", alerts.size()},
                {"alerts", alerts}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 500;
            res.set_content(error.dump(), "application/json");
        }
    });
        // GET /api/alerts/next - Extract highest priority alert
    svr.Get("/api/alerts/next", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            Alert alert;
            bool hasAlert = false;
            
            {
                std::lock_guard<std::mutex> lock(globalDataMutex);
                if (!alertQueue->isEmpty()) {
                    alert = alertQueue->extractMin();
                    hasAlert = true;
                }
            }
            
            if (hasAlert) {
                json response = {
                    {"status", "success"},
                    {"alert", alertToJson(alert)}
                };
                res.set_content(response.dump(), "application/json");
            } else {
                json response = {
                    {"status", "success"},
                    {"message", "No alerts in queue"}
                };
                res.set_content(response.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 500;
            res.set_content(error.dump(), "application/json");
        }
    });

    // POST /api/alerts/:id/acknowledge - Mark alert as acknowledged
    svr.Post(R"(/api/alerts/(\d+)/acknowledge)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int alertID = std::stoi(req.matches[1]);
            auto jsonData = json::parse(req.body);
            std::string acknowledgedBy = jsonData.value("acknowledgedBy", "Unknown");
            
            // Get all alerts, find the one to acknowledge, update it
            std::lock_guard<std::mutex> lock(globalDataMutex);
            auto allAlerts = alertQueue->getAllAlertsSorted();
            
            bool found = false;
            for (auto& alert : allAlerts) {
                if (alert.alertID == alertID) {
                    alert.acknowledged = true;
                    alert.acknowledgedBy = acknowledgedBy;
                    alert.acknowledgedTime = time(nullptr);
                    found = true;
                    break;
                }
            }
            
            if (found) {
                // Rebuild queue with updated alerts
                alertQueue->clear();
                for (const auto& alert : allAlerts) {
                    alertQueue->insert(alert);
                }
                
                json response = {
                    {"status", "success"},
                    {"message", "Alert acknowledged"}
                };
                res.set_content(response.dump(), "application/json");
            } else {
                json error = {
                    {"status", "error"},
                    {"message", "Alert not found"}
                };
                res.status = 404;
                res.set_content(error.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    }); 

    // POST /api/drug-check
    svr.Post("/api/drug-check", [](const Request& req, Response& res) {
    enableCORS(res);
    try {
        auto jsonData = json::parse(req.body);
        std::vector<std::string> medications;
        
        for (const auto& med : jsonData["medications"]) {
            medications.push_back(med.get<std::string>());
        }
        
        auto report = drugInteractionGraph->checkDrugCombination(medications);
        
        json interactions = json::array();
        for (const auto& interaction : report.allInteractions) {
            interactions.push_back({
                {"drug1", interaction.drug1},
                {"drug2", interaction.drug2},
                {"severity", interaction.severity},
                {"severityString", interaction.getSeverityString()},
                {"description", interaction.description}
            });
        }
        
        json response = {
            {"status", "success"},
            {"isSafe", report.isSafe},
            {"totalInteractions", report.totalInteractions},
            {"maxSeverity", report.maxSeverity},
            {"interactions", interactions},
            {"criticalPairs", report.criticalPairs}
        };
        
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        json error = {{"status", "error"}, {"message", e.what()}};
        res.status = 400;
        res.set_content(error.dump(), "application/json");
    }
});

    // GET /api/drugs
    svr.Get("/api/drugs", [](const Request& req, Response& res) {
    enableCORS(res);
    try {
        auto drugs = drugInteractionGraph->getAllDrugs();
        json response = {
            {"status", "success"},
            {"count", drugs.size()},
            {"drugs", drugs}
        };
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        json error = {{"status", "error"}, {"message", e.what()}};
        res.status = 500;
        res.set_content(error.dump(), "application/json");
    }
    });
    
    // OPTIONS for CORS
    svr.Options(R"(.*)", [](const Request& req, Response& res) {
        enableCORS(res);
        res.status = 204;
    });

    
    // GET /api/vitals/:id/recent?count=N
    svr.Get(R"(^/api/vitals/(\d+)/recent$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            int count = 10;  // Default
            
            if (req.has_param("count")) {
                count = std::stoi(req.get_param_value("count"));
            }
            
            auto readings = continuousMonitoring->getLastReadings(patientID, count);
            json results = json::array();
            
            for (const auto& reading : readings) {
                results.push_back(vitalToJson(reading));
            }
            
            json response = {
                {"status", "success"},
                {"patientID", patientID},
                {"count", results.size()},
                {"readings", results}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // GET /api/vitals/:id/stats
    svr.Get(R"(^/api/vitals/(\d+)/stats$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            
            auto stats = continuousMonitoring->getStatistics(patientID);
            
            json response = {
                {"status", "success"},
                {"patientID", patientID},
                {"readingCount", stats.readingCount},
                {"averages", {
                    {"heartRate", stats.avgHeartRate},
                    {"systolicBP", stats.avgSystolicBP},
                    {"diastolicBP", stats.avgDiastolicBP},
                    {"spo2", stats.avgSpO2},
                    {"temperature", stats.avgTemperature}
                }},
                {"ranges", {
                    {"heartRate", {
                        {"min", stats.minHeartRate},
                        {"max", stats.maxHeartRate}
                    }},
                    {"systolicBP", {
                        {"min", stats.minSystolicBP},
                        {"max", stats.maxSystolicBP}
                    }}
                }}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // GET /api/patients/:id/similar?k=N
    svr.Get(R"(^/api/patients/(\d+)/similar$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            int k = 5;  // Default: find 5 similar patients
            
            if (req.has_param("k")) {
                k = std::stoi(req.get_param_value("k"));
            }
            
            // Get recent vitals for this patient
            auto recentReadings = continuousMonitoring->getLastReadings(patientID, 1);
            
            if (recentReadings.empty()) {
                json error = {{"status", "error"}, {"message", "No vitals found for patient"}};
                res.status = 404;
                res.set_content(error.dump(), "application/json");
                return;
            }
            
            auto similarPatients = patientClusteringTree->findSimilarPatients(recentReadings[0], k);
            
            json results = json::array();
            for (const auto& sp : similarPatients) {
                if (sp.patientID != patientID) {  // Exclude the query patient
                    results.push_back({
                        {"patientID", sp.patientID},
                        {"similarity", sp.similarity},
                        {"vitals", {
                            {"heartRate", (int)sp.vitals.coordinates[0]},
                            {"systolicBP", (int)sp.vitals.coordinates[1]},
                            {"diastolicBP", (int)sp.vitals.coordinates[2]},
                            {"spo2", (int)sp.vitals.coordinates[3]},
                            {"temperature", sp.vitals.coordinates[4]}
                        }}
                    });
                }
            }
            
            json response = {
                {"status", "success"},
                {"patientID", patientID},
                {"similarCount", results.size()},
                {"similarPatients", results}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // GET /api/monitoring/windows
    svr.Get("/api/monitoring/windows", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            auto patientIDs = continuousMonitoring->getAllPatientIDs();
            
            json windows = json::array();
            for (int pid : patientIDs) {
                auto readings = continuousMonitoring->getAllReadings(pid);
                windows.push_back({
                    {"patientID", pid},
                    {"readingCount", readings.size()}
                });
            }
            
            json response = {
                {"status", "success"},
                {"monitoredPatients", patientIDs.size()},
                {"windows", windows}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 500;
            res.set_content(error.dump(), "application/json");
        }
    });

    // GET /api/clustering/stats
    svr.Get("/api/clustering/stats", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            json response = {
                {"status", "success"},
                {"totalPoints", patientClusteringTree->size()},
                {"dimensions", patientClusteringTree->getDimensions()}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 500;
            res.set_content(error.dump(), "application/json");
        }
    });



        // POST /api/simulate/start/:id
    svr.Post(R"(^/api/simulate/start/(\d+)$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            
            // Parse options
            auto jsonData = json::parse(req.body.empty() ? "{}" : req.body);
            int intervalMs = jsonData.value("intervalMs", 2000);
            
            // Get patient's last vitals or use defaults
            VitalRecord baseVitals;
            auto recentReadings = continuousMonitoring->getLastReadings(patientID, 1);
            
            if (!recentReadings.empty()) {
                baseVitals = recentReadings[0];
            } else {
                // Default vitals
                baseVitals.patientID = patientID;
                baseVitals.timestamp = time(nullptr);
                baseVitals.heart_rate = 75;
                baseVitals.systolic_bp = 120;
                baseVitals.diastolic_bp = 80;
                baseVitals.spo2 = 98;
                baseVitals.temperature = 37.0f;
            }
            
            bool started = vitalSimulator->startSimulation(patientID, baseVitals, intervalMs);
            
            json response = {
                {"status", "success"},
                {"message", "Simulation started"},
                {"patientID", patientID},
                {"intervalMs", intervalMs}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // POST /api/simulate/stop/:id
    svr.Post(R"(^/api/simulate/stop/(\d+)$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            bool stopped = vitalSimulator->stopSimulation(patientID);
            
            json response = {
                {"status", "success"},
                {"message", stopped ? "Simulation stopped" : "No simulation running"},
                {"patientID", patientID}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // GET /api/simulate/status/:id
    svr.Get(R"(^/api/simulate/status/(\d+)$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            bool running = vitalSimulator->isRunning(patientID);
            
            json response = {
                {"status", "success"},
                {"patientID", patientID},
                {"running", running}
            };
            
            if (running) {
                auto config = vitalSimulator->getConfig(patientID);
                response["intervalMs"] = config.intervalMs;
                response["activeEvent"] = config.activeEvent;
            }
            
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // POST /api/simulate/event/:id
    svr.Post(R"(^/api/simulate/event/(\d+)$)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            auto jsonData = json::parse(req.body);
            
            int eventType = jsonData["event"];
            SimulationEvent event = static_cast<SimulationEvent>(eventType);
            
            bool triggered = vitalSimulator->triggerEvent(patientID, event);
            
            json response = {
                {"status", "success"},
                {"message", "Event triggered"},
                {"patientID", patientID},
                {"event", eventType}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // GET /api/simulate/active
    svr.Get("/api/simulate/active", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            auto active = vitalSimulator->getActiveSimulations();
            
            json response = {
                {"status", "success"},
                {"count", active.size()},
                {"patientIDs", active}
            };
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 500;
            res.set_content(error.dump(), "application/json");
        }
    });

    // ==================== ANALYSIS ENDPOINTS ====================

    // GET /api/vitals/:id/analyze
    svr.Get(R"(/api/vitals/(\d+)/analyze)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            
            // Get recent readings (last 20)
            auto readings = continuousMonitoring->getLastReadings(patientID, 20);
            
            if (readings.empty()) {
                json error = {{"status", "error"}, {"message", "No vitals found"}};
                res.status = 404;
                res.set_content(error.dump(), "application/json");
                return;
            }
            
            // Analyze patterns
            auto patterns = vitalAnalyzer->analyzePatterns(readings);
            
            json patternsJson = json::array();
            for (const auto& pattern : patterns) {
                patternsJson.push_back({
                    {"type", pattern.type},
                    {"vitalSign", pattern.vitalSign},
                    {"description", pattern.description},
                    {"changeValue", pattern.changeValue},
                    {"timeWindowMinutes", pattern.timeWindowMinutes},
                    {"isCritical", pattern.isCritical}
                });
            }
            
            // Get current status
            std::string status = vitalAnalyzer->getVitalStatus(readings.back());
            
            json response = {
                {"status", "success"},
                {"patientID", patientID},
                {"vitalStatus", status},
                {"patternsDetected", patterns.size()},
                {"patterns", patternsJson}
            };
            
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // GET /api/patient/:id/risk-score
    svr.Get(R"(/api/patient/(\d+)/risk-score)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            
            // Get recent readings
            auto readings = continuousMonitoring->getLastReadings(patientID, 20);
            
            if (readings.empty()) {
                json error = {{"status", "error"}, {"message", "No vitals found"}};
                res.status = 404;
                res.set_content(error.dump(), "application/json");
                return;
            }
            
            // Count alerts for this patient
            int alertCount = 0;
            auto unackAlerts = alertQueue->getUnacknowledgedAlerts();
            for (const auto& alert : unackAlerts) {
                if (alert.patientID == patientID && alert.priority <= 2) {
                    alertCount++;
                }
            }
            
            // Get drug interaction severity
            int drugSeverity = 0;
            Patient* patient = patientDB->search(patientID);
            if (patient && !patient->medications.empty()) {
                auto report = drugInteractionGraph->checkDrugCombination(patient->medications);
                drugSeverity = report.maxSeverity;
            }
            
            // Calculate risk
            auto assessment = vitalAnalyzer->calculateRiskScore(readings, alertCount, drugSeverity);
            
            // Convert patterns to JSON
            json patternsJson = json::array();
            for (const auto& pattern : assessment.patterns) {
                patternsJson.push_back({
                    {"type", pattern.type},
                    {"vitalSign", pattern.vitalSign},
                    {"description", pattern.description},
                    {"isCritical", pattern.isCritical}
                });
            }
            
            json response = {
                {"status", "success"},
                {"patientID", patientID},
                {"riskScore", assessment.riskScore},
                {"riskLevel", assessment.riskLevel},
                {"factors", {
                    {"abnormalVitalsCount", assessment.factors.abnormalVitalsCount},
                    {"criticalAlertsCount", assessment.factors.criticalAlertsCount},
                    {"drugInteractionSeverity", assessment.factors.drugInteractionSeverity},
                    {"hasDeteriorationTrend", assessment.factors.hasDeteriorationTrend},
                    {"hasErraticPattern", assessment.factors.hasErraticPattern}
                }},
                {"recommendations", assessment.recommendations},
                {"patterns", patternsJson}
            };
            
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // POST /api/vitals/:id/check-abnormal
    svr.Post(R"(/api/vitals/(\d+)/check-abnormal)", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            int patientID = std::stoi(req.matches[1]);
            auto jsonData = json::parse(req.body);
            
            VitalRecord vitals;
            vitals.patientID = patientID;
            vitals.timestamp = jsonData["timestamp"];
            vitals.heart_rate = jsonData["heart_rate"];
            vitals.systolic_bp = jsonData["systolic_bp"];
            vitals.diastolic_bp = jsonData["diastolic_bp"];
            vitals.spo2 = jsonData["spo2"];
            vitals.temperature = jsonData["temperature"];
            
            std::vector<std::string> abnormalities;
            bool hasAbnormality = vitalAnalyzer->checkAbnormal(vitals, abnormalities);
            std::string status = vitalAnalyzer->getVitalStatus(vitals);
            
            json response = {
                {"status", "success"},
                {"hasAbnormality", hasAbnormality},
                {"vitalStatus", status},
                {"abnormalities", abnormalities}
            };
            
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    
    
    std::string host = "0.0.0.0";
    int port = 8080;

    setupPhase1Systems();
    
    std::cout << "\n🚀 Server at http://" << host << ":" << port << std::endl;
    std::cout << "\nEndpoints:" << std::endl;
    std::cout << "  GET  /                - Health check" << std::endl;
    std::cout << "  POST /api/vitals      - Add vitals" << std::endl;
    std::cout << "  GET  /api/vitals/:id  - Get vitals" << std::endl;
    std::cout << "  GET  /api/vitals/timestamp/:ts - Get by timestamp" << std::endl; 
    std::cout << "  POST /api/patient     - Add patient" << std::endl;
    std::cout << "  GET  /api/patient/:id - Get patient" << std::endl;
    std::cout << "  GET  /api/patients    - Get all" << std::endl;
    std::cout << "  POST /api/alert       - Create alert" << std::endl;
    std::cout << "  GET  /api/alerts      - Get alerts" << std::endl;
    std::cout << "  GET  /api/alerts/next - Get & remove top alert" << std::endl;  // ADD
    std::cout << "  POST /api/alerts/:id/acknowledge - Acknowledge alert" << std::endl;
    std::cout << "\nPress Ctrl+C to stop\n" << std::endl;
    
    //svr.listen(host.c_str(), port);
    bool success = svr.listen(host.c_str(), port);
    if (!success) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
    }
    
    delete vitalSignsDB;
    delete patientDB;
    delete alertQueue;
    delete drugInteractionGraph;
    delete patientClusteringTree;
    delete continuousMonitoring;
    delete vitalSimulator;
    delete vitalAnalyzer;
    
    return 0;
}