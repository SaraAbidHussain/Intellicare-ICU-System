#include <iostream>
#include <string>
#include "../../include/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include "data_structures/btree.h"
#include "data_structures/priority_queue.h"
#include "data_structures/hash_table.h"
#include "models/vital_record.h"
#include "models/patient.h"
#include "models/alert.h"

using namespace httplib;
using json = nlohmann::json;

// Global data structures
DiskBTree* vitalSignsDB;
HashTable<int, Patient>* patientDB;
PriorityQueue* alertQueue;

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

void enableCORS(Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    vitalSignsDB = new DiskBTree(50, "vitals");
    patientDB = new HashTable<int, Patient>(101, "patients.bin");
    alertQueue = new PriorityQueue("alerts.bin");
    
    Server svr;
    
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
    
    // POST /api/vitals
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
            
            vitalSignsDB->insert(record.timestamp, record);
            
            json response = {{"status", "success"}, {"message", "Vitals recorded"}};
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& e) {
            json error = {{"status", "error"}, {"message", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });
    
    // GET /api/vitals/:id
    svr.Get(R"(/api/vitals/(\d+))", [](const Request& req, Response& res) {
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
    
    // POST /api/patient
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
    
    // GET /api/alerts
    svr.Get("/api/alerts", [](const Request& req, Response& res) {
        enableCORS(res);
        try {
            json alerts = json::array();
            PriorityQueue tempQueue;
            
            while (!alertQueue->isEmpty()) {
                Alert alert = alertQueue->extractMin();
                alerts.push_back(alertToJson(alert));
                tempQueue.insert(alert);
            }
            
            while (!tempQueue.isEmpty()) {
                alertQueue->insert(tempQueue.extractMin());
            }
            
            json response = {{"status", "success"}, {"count", alerts.size()}, {"alerts", alerts}};
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
    
    std::string host = "0.0.0.0";
    int port = 8080;
    
    std::cout << "\n🚀 Server at http://" << host << ":" << port << std::endl;
    std::cout << "\nEndpoints:" << std::endl;
    std::cout << "  GET  /                - Health check" << std::endl;
    std::cout << "  POST /api/vitals      - Add vitals" << std::endl;
    std::cout << "  GET  /api/vitals/:id  - Get vitals" << std::endl;
    std::cout << "  POST /api/patient     - Add patient" << std::endl;
    std::cout << "  GET  /api/patient/:id - Get patient" << std::endl;
    std::cout << "  GET  /api/patients    - Get all" << std::endl;
    std::cout << "  POST /api/alert       - Create alert" << std::endl;
    std::cout << "  GET  /api/alerts      - Get alerts" << std::endl;
    std::cout << "\nPress Ctrl+C to stop\n" << std::endl;
    
    svr.listen(host.c_str(), port);
    
    delete vitalSignsDB;
    delete patientDB;
    delete alertQueue;
    
    return 0;
}