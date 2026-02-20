# IntelliCare-ICU: Intelligent ICU Patient Monitoring System

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![C++](https://img.shields.io/badge/C++-17-blue)]()
[![AWS](https://img.shields.io/badge/AWS-EC2-orange)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()

> A comprehensive ICU patient monitoring system built with custom data structures and algorithms, deployed on AWS EC2 with real-time analytics capabilities.

---

## 📋 Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Project Timeline](#project-timeline)
- [Implemented Features](#implemented-features)
- [Technology Stack](#technology-stack)
- [Data Structures & Algorithms](#data-structures--algorithms)
- [API Documentation](#api-documentation)
- [Installation & Setup](#installation--setup)
- [Testing](#testing)
- [Future Roadmap](#future-roadmap)
- [Performance Metrics](#performance-metrics)
- [Project Structure](#project-structure)

---

## 🎯 Overview

IntelliCare-ICU is an intelligent patient monitoring system designed for intensive care units. The system provides real-time vital sign monitoring, patient clustering, drug interaction analysis, and predictive alerts using custom-built data structures and algorithms.

**Key Objectives:**
- Real-time vital signs monitoring with millisecond-level precision
- Efficient patient lookup and vitals retrieval across any timestamp
- Drug interaction safety verification
- Patient clustering based on similar conditions
- Predictive alerting for critical situations

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Frontend (React/HTML)                    │
└─────────────────────┬───────────────────────────────────────┘
                      │ REST API
┌─────────────────────▼───────────────────────────────────────┐
│                  C++ Backend Server                          │
│                    (AWS EC2)                                 │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   B-Tree    │  │  Hash Table  │  │  Priority    │      │
│  │  (Vitals)   │  │  (Patients)  │  │    Queue     │      │
│  └─────────────┘  └──────────────┘  └──────────────┘      │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ Drug Graph  │  │   KD-Tree    │  │   Sliding    │      │
│  │    (DFS)    │  │ (Clustering) │  │   Window     │      │
│  └─────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────┬───────────────────────────────────────┘
                      │ Disk I/O
┌─────────────────────▼───────────────────────────────────────┐
│              Persistent Storage (Binary Files)              │
└─────────────────────────────────────────────────────────────┘
```

---

## 📅 Project Timeline

### **Phase 1: Core Data Management**

**Duration:** Week 1-2  
**Status:** Production-ready

#### Implemented Features:

##### Custom B-Tree Implementation
- Initial in-memory implementation
- Migrated to disk-based storage for persistence
- Added indexing for efficient range queries
- Optimized to load only required data into memory
- **Time Complexity:** O(log n) for insertions and searches
- **Space Optimization:** Loads data on-demand instead of full dataset

##### Patient Lookup System (HashMap)
- O(1) average lookup time for patient records
- Collision handling using chaining
- Dynamic resizing with load factor management (0.75 threshold)
- Disk persistence for data recovery

##### Medication Lookup (HashTable)
- Fast medication information retrieval
- Drug database with dosage, side effects, and contraindications
- Binary serialization for efficient storage

#### Technical Achievements:
```cpp
// Key metrics:
- B-Tree Degree: 50 (supports up to 99 keys per node)
- Hash Table Load Factor: 0.75
- Average Query Time: < 1ms for patient lookup
- Disk Storage Format: Binary for optimal I/O
```

---

### **Phase 2: Priority & Alert System**

**Duration:** Week 2-3  
**Status:** Deployed on AWS EC2

#### Implemented Features:

##### Min-Heap Priority Queue
- Critical alerts have highest priority (lowest value = most urgent)
- Automatic alert prioritization based on severity
- Five priority levels: CRITICAL, HIGH, MEDIUM, LOW, INFO
- **Time Complexity:** O(log n) for insertion and extraction

##### Alert Classification System
- 7 alert types: Vital Abnormal, Drug Interaction, Equipment Failure, etc.
- Timestamp tracking and acknowledgment system
- Persistent storage for alert history

##### AWS Deployment
- Deployed on EC2 instance
- Git-based deployment workflow (push to GitHub → pull on EC2)
- REST API with CORS support
- Basic frontend for visualization

#### Alert Categories:

| Priority | Value | Use Case | Response Time |
|----------|-------|----------|---------------|
| CRITICAL | 1 | Life-threatening | Immediate |
| HIGH | 2 | Urgent attention | < 5 min |
| MEDIUM | 3 | Monitor closely | < 15 min |
| LOW | 4 | Non-urgent | < 1 hour |
| INFO | 5 | Informational | N/A |

---

### **Phase 3: Drug Safety Analysis**

**Duration:** Week 3-4  
**Status:** Integration complete

#### Implemented Features:

##### Drug Interaction Graph
- Graph-based representation with drugs as nodes
- Edges represent interactions with severity levels
- Undirected graph (symmetric interactions)
- **Algorithm:** Depth-First Search (DFS) for interaction detection

##### Interaction Severity Levels
- NONE (0): No known interaction
- MILD (1): Minor effects, monitoring recommended
- MODERATE (2): May require dosage adjustment
- SEVERE (3): Dangerous combination, alternative recommended
- CONTRAINDICATED (4): Never use together

##### Safety Verification System
- Checks all pairwise drug combinations
- Generates comprehensive safety reports
- Identifies critical drug pairs requiring intervention
- Provides detailed interaction descriptions

#### Technical Implementation:

```cpp
// DFS Traversal for Interaction Paths
Time Complexity: O(V + E) where V = drugs, E = interactions
Space Complexity: O(V) for visited set and path storage

// Safety Check
- Pairwise comparison: O(n²) for n medications
- Path finding: O(V + E) per pair using DFS
```

#### Sample Drug Database:
- 20+ common ICU medications loaded
- Warfarin, ACE inhibitors, antibiotics, statins, etc.
- Expandable database structure

---

### **Phase 4: Patient Grouping & Analytics**

**Duration:** Week 4-5  
**Status:** Fully operational

#### Implemented Features:

##### 1. KD-Tree for Patient Clustering

**5-dimensional space:** Heart Rate, Systolic BP, Diastolic BP, SpO2, Temperature

Applications:
- Find similar patients for treatment protocol reference
- Cluster patients for specialized care teams
- Compare outcomes across similar cases

**Operations:**
- `insert()`: O(log n) average case
- `findNearest()`: O(log n) average case
- `findKNearest()`: O(k log n) for k neighbors
- `rangeSearch()`: O(n^(1-1/d) + m) where m = results

**Use Case Example:**
```
Patient 999 (Critical): HR=118, BP=158/93, SpO2=89%, Temp=38.7°C

Similar Patients Found:
1. Patient 503: HR=115, BP=155/92, SpO2=90% (Similarity: 5.2)
2. Patient 507: HR=120, BP=160/95, SpO2=88% (Similarity: 6.8)
3. Patient 505: HR=125, BP=165/98, SpO2=87% (Similarity: 9.1)

→ Group these patients for coordinated treatment protocol
```

##### 2. Circular Buffer (Ring Buffer)

- Fixed-size buffer with automatic overwrite of oldest data
- Generic implementation supporting any data type
- Efficient O(1) push and pop operations
- Memory-efficient: Pre-allocated fixed size

**Features:**
- Configurable capacity
- FIFO (First In, First Out) behavior
- Supports peek operations without removal
- Statistical functions (min, max, average) for numeric types

**Applications:**
- Real-time vital sign streaming
- Heart rate monitoring (last N beats)
- Continuous BP tracking
- Temperature trends

##### 3. Sliding Window Manager

- Manages multiple circular buffers (one per patient)
- Default window size: 100 readings
- Tracks most recent vital signs per patient
- Provides statistical analysis over the window

**Statistics Provided:**
- Average vitals (HR, BP, SpO2, Temperature)
- Min/Max ranges for each vital sign
- Reading count in current window
- Trend analysis support

**Real-Time Monitoring Example:**
```
Patient 701 - Continuous Monitoring (Last 100 readings)

After 30 minutes:
  Avg HR: 75 bpm | Avg BP: 122/81 | Window: 30 readings

After 60 minutes:
  Avg HR: 78 bpm | Avg BP: 125/82 | Window: 60 readings

After 120 minutes (2 hours):
  Avg HR: 85 bpm | Avg BP: 135/88 | Window: 100 readings
  
Alert: Gradual deterioration pattern detected
```

#### Performance Metrics:

| Operation | Complexity | Use Case |
|-----------|-----------|----------|
| Add Reading | O(1) | Real-time vital insertion |
| Get Last N | O(N) | Recent history retrieval |
| Statistics | O(W) | Window analysis (W=window size) |
| Find Similar | O(log n) | Patient clustering |

---

## 🔧 Technology Stack

### Backend
- **Language:** C++17
- **Server:** cpp-httplib (Lightweight HTTP server)
- **JSON:** nlohmann/json
- **Build System:** GNU Make
- **Compiler:** g++ with C++17 standard

### Deployment
- **Platform:** AWS EC2 (Ubuntu)
- **Version Control:** Git/GitHub
- **Deployment Method:** Pull-based (push to GitHub → pull on EC2)

### Frontend *(In Development)*
- Basic HTML/CSS/JavaScript
- REST API integration
- Real-time dashboard (planned enhancement)

---

## 📊 Data Structures & Algorithms

### Summary Table

| Data Structure | Purpose | Time Complexity | Space | Status |
|----------------|---------|-----------------|-------|--------|
| **B-Tree** | Vital signs storage & retrieval | O(log n) | O(n) | 
| **Hash Table** | Patient & medication lookup | O(1) avg | O(n) | 
| **Min-Heap** | Alert prioritization | O(log n) | O(n) | 
| **Graph + DFS** | Drug interaction detection | O(V+E) | O(V) | 
| **KD-Tree** | Patient clustering (5D) | O(log n) | O(n) | 
| **Circular Buffer** | Fixed sliding window | O(1) | O(k) | 
| **Sliding Window** | Continuous monitoring | O(1) insert | O(kp) | 


---

## 🌐 API Documentation

### Base URL
```
http://your-ec2-instance:8080
```

### Endpoints

#### Health Check
```http
GET /
Response: { "status": "online", "message": "IntelliCare ICU API", "version": "1.0.0" }
```

#### Vital Signs

**Add Vital Signs**
```http
POST /api/vitals
Body: {
  "patientID": 101,
  "timestamp": 1640000000,
  "heart_rate": 75,
  "systolic_bp": 120,
  "diastolic_bp": 80,
  "spo2": 98,
  "temperature": 37.0
}
Response: { "status": "success", "message": "Vitals recorded" }
```

**Get Vital Signs (Range Query)**
```http
GET /api/vitals/:id?start=<timestamp>&end=<timestamp>
Response: {
  "status": "success",
  "count": 150,
  "readings": [...]
}
```

**Get Recent Readings**
```http
GET /api/vitals/:id/recent?count=10
Response: {
  "status": "success",
  "patientID": 101,
  "count": 10,
  "readings": [...]
}
```

**Get Vital Statistics**
```http
GET /api/vitals/:id/stats
Response: {
  "status": "success",
  "readingCount": 100,
  "averages": {
    "heartRate": 75.5,
    "systolicBP": 122.3,
    "diastolicBP": 80.1,
    "spo2": 97.8,
    "temperature": 37.1
  },
  "ranges": {
    "heartRate": { "min": 68, "max": 85 },
    "systolicBP": { "min": 115, "max": 135 }
  }
}
```

#### Patients

**Add Patient**
```http
POST /api/patient
Body: {
  "patientID": 101,
  "name": "John Doe",
  "age": 65,
  "gender": "M",
  "ward": "ICU-A",
  "condition": "Post-cardiac surgery",
  "admissionDate": "2024-01-15",
  "bloodType": "O+"
}
```

**Get Patient by ID**
```http
GET /api/patient/:id
Response: { "status": "success", "data": {...} }
```

**Get All Patients**
```http
GET /api/patients
Response: {
  "status": "success",
  "count": 25,
  "patients": [...]
}
```

**Find Similar Patients**
```http
GET /api/patients/:id/similar?k=5
Response: {
  "status": "success",
  "patientID": 101,
  "similarCount": 5,
  "similarPatients": [
    {
      "patientID": 103,
      "similarity": 5.2,
      "vitals": {...}
    },
    ...
  ]
}
```

#### Alerts

**Create Alert**
```http
POST /api/alert
Body: {
  "patientID": 101,
  "priority": 1,
  "type": 0,
  "message": "Critical: Heart rate above 120 bpm"
}
```

**Get All Alerts**
```http
GET /api/alerts
Response: {
  "status": "success",
  "count": 15,
  "alerts": [...]
}
```

#### Drug Safety

**Check Drug Interactions**
```http
POST /api/drug-check
Body: {
  "medications": ["Warfarin", "Aspirin", "Ibuprofen"]
}
Response: {
  "status": "success",
  "isSafe": false,
  "totalInteractions": 3,
  "maxSeverity": 3,
  "interactions": [...],
  "criticalPairs": ["Warfarin + Aspirin", "Warfarin + Ibuprofen"]
}
```

**Get All Drugs**
```http
GET /api/drugs
Response: {
  "status": "success",
  "count": 25,
  "drugs": ["Warfarin", "Aspirin", ...]
}
```

#### Monitoring

**Get Monitoring Windows**
```http
GET /api/monitoring/windows
Response: {
  "status": "success",
  "monitoredPatients": 12,
  "windows": [
    { "patientID": 101, "readingCount": 100 },
    ...
  ]
}
```

**Get Clustering Statistics**
```http
GET /api/clustering/stats
Response: {
  "status": "success",
  "totalPoints": 1250,
  "dimensions": 5
}
```

---

##  Installation & Setup

### Prerequisites

```bash
# Install required packages
sudo apt update
sudo apt install g++ make git

# Install dependencies
# cpp-httplib (header-only)
# nlohmann/json (header-only)
```

### Clone & Build

```bash
# Clone repository
git clone https://github.com/your-username/intellicare-icu.git
cd intellicare-icu

# Build project
make clean
make

# Run tests
make test_all

# Start server
./bin/server
```

### AWS EC2 Deployment

```bash
# On local machine
git add .
git commit -m "Update features"
git push origin main

# On EC2 instance
cd intellicare-icu
git pull origin main
make clean && make
./bin/server
```

---

##  Testing

### Test Coverage

#### Phase 1 Tests
```bash
make test_btree          # B-tree operations
make test_hash           # Hash table operations
```

#### Phase 2 Tests
```bash
make test_priority_queue # Priority queue & alerts
make test_patient        # Patient model
```

#### Phase 3 Tests
```bash
make test_drug_graph     # Drug interactions & DFS
```

#### Phase 4 Tests
```bash
make test_kdtree         # Patient clustering
make test_sliding_window # Continuous monitoring
make test_circular_buffer # Ring buffer operations
```

#### Run All Tests
```bash
make test_all
```

### Sample Test Results

```
========== TEST: KD-Tree Patient Clustering ==========
 Basic operations: PASSED
 Nearest neighbor: PASSED (found in 0.8ms)
 K-nearest search: PASSED (5 neighbors in 1.2ms)
 Similar patients: PASSED (3 clusters identified)
 Range search: PASSED (15 patients within radius)
 Batch build: PASSED (100 points in 12ms)
 Persistence: PASSED (saved/loaded successfully)

========== TEST: Sliding Window ==========
 Basic operations: PASSED
 Multiple patients: PASSED (3 windows active)
 Statistics: PASSED (avg, min, max calculated)
 Overwrite behavior: PASSED (old data removed)
 Real-time simulation: PASSED (120 readings)

ALL TESTS PASSED 
```

---

## 🎯 Future Roadmap

### **Phase 5: Predictive Analysis** 

**Estimated Duration:** 1-1.5 weeks

#### Planned Features:

- **Anomaly Detection Filter**
  - Pattern recognition in vital signs
  - Detect abnormal heartbeat patterns
  - Blood pressure trend analysis
  - Early warning for cardiac events
  - Algorithm: Moving average + standard deviation thresholds

- **Risk Scoring System**
  - Multi-factor risk calculation
  - Weighted scoring based on:
    - Vital sign deviations
    - Drug interactions
    - Patient history
    - Recent trends
  - Risk categories: Low, Moderate, High, Critical

- **Predictive Alerts**
  - Proactive alerting before critical events
  - ML-ready architecture (rules-based initially)
  - Trend-based predictions
  - Integration with existing alert system

#### Technical Approach:

```cpp
// Planned algorithm structure
class AnomalyDetector {
  - detectHeartRateAnomaly()
  - detectBPPattern()
  - calculateRiskScore()
  - generatePredictiveAlert()
};

Risk Score = w1*vitalScore + w2*trendScore + w3*interactionScore
where w1, w2, w3 are tunable weights
```

---

### **Phase 6: Polish & Real-time Features** 

**Estimated Duration:** 1 week

#### Planned Features:

- **Real-time Data Simulator**
  - Generate realistic vital sign patterns
  - Simulate various medical scenarios
  - Emergency situation simulation
  - Multiple patient simulation

- **Enhanced Frontend**
  - Live vital sign dashboards
  - Real-time graphs using Chart.js
  - Alert notification system
  - Patient clustering visualization
  - Drug interaction checker UI

- **Performance Optimization**
  - Query caching for frequent requests
  - Connection pooling
  - Batch processing for vitals
  - Memory optimization

- **Advanced Visualizations**
  - Heart rate waveforms
  - Blood pressure trends
  - Patient similarity heat maps
  - Alert timeline

#### UI Mockup Goals:

```
┌─────────────────────────────────────────────┐
│  IntelliCare ICU Dashboard                  │
├─────────────────────────────────────────────┤
│  🚨 Active Alerts (3)    👥 Patients (25)  │
│  ───────────────────────────────────────────│
│  ┌───────────┐ ┌───────────┐ ┌───────────┐│
│  │ Patient A │ │ Patient B │ │ Patient C ││
│  │ HR: 120 ↑ │ │ HR: 75 ✓  │ │ HR: 85 ↑  ││
│  │ BP: 160/95│ │ BP: 120/80│ │ BP: 130/85││
│  │ 🔴 CRITICAL│ │ 🟢 STABLE │ │ 🟡 MONITOR││
│  └───────────┘ └───────────┘ └───────────┘│
│  ───────────────────────────────────────────│
│  📊 Real-time Graphs │ 💊 Drug Safety      │
└─────────────────────────────────────────────┘
```

---

## 📈 Performance Metrics

### Current System Performance

#### Database Operations

| Operation | Time Complexity | Actual Performance |
|-----------|----------------|-------------------|
| Insert Vital | O(log n) | < 2ms |
| Range Query | O(log n + k) | < 5ms for 100 records |
| Patient Lookup | O(1) | < 0.5ms |
| Find Similar | O(log n) | < 3ms |

#### Memory Usage

```
B-Tree Node Size: ~800 bytes (degree 50)
Hash Table: 1009 buckets (resizable)
Priority Queue: Dynamic heap allocation
KD-Tree: 5D points + tree structure
Sliding Windows: 100 readings × N patients
```

#### Disk Storage

```
vitals_index.dat           - B-tree index structure
vitals_data.dat            - Vital signs records
patients.bin               - Patient database
alerts.bin                 - Alert history
drug_interactions.bin      - Drug graph
patient_clustering.bin     - KD-tree
sliding_windows.bin        - Monitoring windows
```

#### API Response Times *(AWS EC2 t2.micro)*

```
GET /api/vitals/:id          → 15-25ms
GET /api/vitals/:id/stats    → 10-15ms
GET /api/patients/:id        → 5-10ms
POST /api/vitals             → 20-30ms
POST /api/drug-check         → 25-40ms (3-5 drugs)
GET /api/patients/:id/similar → 30-50ms
```

---



## 📚 Project Structure

```
intellicare-icu/
├── include/
│   ├── data_structures/
│   │   ├── btree.h
│   │   ├── hash_table.h
│   │   ├── priority_queue.h
│   │   ├── drug_graph.h
│   │   ├── kdtree.h
│   │   ├── circular_buffer.h
│   │   └── sliding_window.h
│   ├── models/
│   │   ├── vital_record.h
│   │   ├── patient.h
│   │   ├── alert.h
│   │   └── medication.h
│   ├── httplib.h
│   └── nlohmann/json.hpp
├── src/
│   ├── data_structures/
│   │   ├── btree.cpp
│   │   ├── priority_queue.cpp
│   │   ├── drug_graph.cpp
│   │   ├── kdtree.cpp
│   │   └── sliding_window.cpp
│   ├── models/
│   │   ├── vital_record.cpp
│   │   ├── patient.cpp
│   │   ├── alert.cpp
│   │   └── medication.cpp
│   └── server.cpp
├── tests/
│   ├── test_btree.cpp
│   ├── test_hash_table.cpp
│   ├── test_priority_queue.cpp
│   ├── test_drug_graph.cpp
│   ├── test_kdtree.cpp
│   ├── test_sliding_window.cpp
│   └── test_circular_buffer.cpp
├── data/           # Binary storage files
├── bin/            # Compiled executables
├── Makefile
└── README.md
```

---

## 🤝 Contributing

This is currently a solo project for academic/portfolio purposes. Future contributions may be accepted after Phase 6 completion.

---

## 📝 License

MIT License - See LICENSE file for details

---

## 👨‍💻 Author

**Sara Abid**  
Computer Science Student  
[GitHub](https://github.com/SaraAbidHussain) 

---


## 🙏 Acknowledgments

- AWS EC2 for hosting infrastructure
- cpp-httplib for lightweight HTTP server
- nlohmann/json for JSON parsing
- The open-source community

---

## 📊 Project Stats

```
Data Structures: 7 custom implementations
API Endpoints: 15+
Test Cases: 50+
```

---


*This project demonstrates proficiency in data structures, algorithms, system design, backend development, and cloud deployment. Built entirely from scratch using core C++ principles.*
