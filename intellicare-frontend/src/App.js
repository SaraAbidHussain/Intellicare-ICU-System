// //file structure: intellicare-frontend/src/App.js
// import React, { useState, useEffect } from 'react';
// import api from './api';
// import './App.css';
// import PatientCarousel from './components/PatientCarousel';
// import AlertPanel from './components/AlertPanel';
// import MedicationPanel from './components/MedicationPanel';
// import LiveTerminal from './components/LiveTerminal';
// import ConnectionStatus from './components/ConnectionStatus';
// import DrugInteractionModal from './components/DrugInteractionModal';

// // API Configuration - CHANGE THIS TO YOUR SERVER
// // API base is configured in src/api.js

// function App() {
//   const [patients, setPatients] = useState([]);
//   const [currentPatientIndex, setCurrentPatientIndex] = useState(0);
//   const [alerts, setAlerts] = useState([]);
//   const [terminalLogs, setTerminalLogs] = useState([]);
//   const [connected, setConnected] = useState(false);
//   const [uptime, setUptime] = useState(0);
//   const [activeSimulations, setActiveSimulations] = useState([]);
//   const [showDrugModal, setShowDrugModal] = useState(false);

//   // Fetch initial data
//   useEffect(() => {
//     loadInitialData();
//     checkConnection();
//   }, []);

//   // Auto-rotate patients every 10 seconds
//   useEffect(() => {
//     if (patients.length === 0) return;
    
//     const interval = setInterval(() => {
//       setCurrentPatientIndex(prev => (prev + 1) % patients.length);
//     }, 10000);
    
//     return () => clearInterval(interval);
//   }, [patients.length]);

//   // Poll for updates every 2 seconds
//   useEffect(() => {
//     const interval = setInterval(() => {
//       refreshData();
//     }, 2000);
    
//     return () => clearInterval(interval);
//   }, [patients]);

//   // Uptime counter
//   useEffect(() => {
//     const interval = setInterval(() => {
//       setUptime(prev => prev + 1);
//     }, 1000);
    
//     return () => clearInterval(interval);
//   }, []);

//   const loadInitialData = async () => {
//     try {
//       // Load patients
//   const patientsRes = await api.get('/api/patients');
//       if (patientsRes.data.status === 'success') {
//         const patientsData = patientsRes.data.patients;
//         const patientsWithDetails = await Promise.all(
//         patientsData.map(async (p) => {
//           const detailRes = await api.get(`/api/patient/${p.patientID}`);
//           return detailRes.data.status === 'success' ? detailRes.data.data : p;
//         })
//       );
//         setPatients(patientsData);
        
//         addLog('✓ Loaded ' + patientsData.length + ' patients', 'success');
        
//         // Start simulations for all patients
//         for (const patient of patientsData) {
//           await startSimulation(patient.patientID);
//         }
//       }
      
//       // Load alerts
//   const alertsRes = await api.get('/api/alerts');
//       if (alertsRes.data.status === 'success') {
//         setAlerts(alertsRes.data.alerts);
//         addLog('✓ Loaded ' + alertsRes.data.alerts.length + ' alerts', 'info');
//       }
      
//     } catch (error) {
//       console.error('Error loading data:', error);
//       addLog('✗ Error loading data', 'error');
//     }
//   };

//   const refreshData = async () => {
//     if (patients.length === 0) return;
    
//     try {
//       // Refresh current patient's vitals
//       const currentPatient = patients[currentPatientIndex];
//       if (!currentPatient) return;
      
//       //const vitalsRes = await axios.get(`/vitals/${currentPatient.patientID}/recent?count=1`);
//   const vitalsRes = await api.get(`/api/vitals/${currentPatient.patientID}/recent?count=1`);
//       if (vitalsRes.data.status === 'success' && vitalsRes.data.readings.length > 0) {
//         const latest = vitalsRes.data.readings[0];
        
//         // Update patient data
//         setPatients(prev => prev.map(p => 
//           p.patientID === currentPatient.patientID 
//             ? { ...p, latestVitals: latest }
//             : p
//         ));
        
//         // Add to terminal
//         addLog(
//           `← Received: HR=${latest.heart_rate}, BP=${latest.systolic_bp}/${latest.diastolic_bp}, O₂=${latest.spo2}%`,
//           'receive'
//         );
//       }
      
//       // Refresh alerts
//   const alertsRes = await api.get('/api/alerts');
//       if (alertsRes.data.status === 'success') {
//         const newAlerts = alertsRes.data.alerts;
//         if (newAlerts.length > alerts.length) {
//           addLog('← Received: Alert: ' + newAlerts[0].message, 'alert');
//         }
//         setAlerts(newAlerts);
//       }
      
//     } catch (error) {
//       console.error('Error refreshing data:', error);
//     }
//   };

//   const startSimulation = async (patientID) => {
//     try {
//   await api.post(`/api/simulate/start/${patientID}`, { intervalMs: 2000 });
//       setActiveSimulations(prev => [...prev, patientID]);
//       addLog(`→ Started simulation for Patient #${patientID}`, 'send');
//     } catch (error) {
//       console.error('Error starting simulation:', error);
//     }
//   };

//   const stopSimulation = async (patientID) => {
//     try {
//   await api.post(`/api/simulate/stop/${patientID}`);
//       setActiveSimulations(prev => prev.filter(id => id !== patientID));
//       addLog(`→ Stopped simulation for Patient #${patientID}`, 'send');
//     } catch (error) {
//       console.error('Error stopping simulation:', error);
//     }
//   };

//   const checkConnection = () => {
//     api.get('/')
//       .then(() => setConnected(true))
//       .catch(() => setConnected(false));
//   };

//   const addLog = (message, type = 'info') => {
//     const timestamp = new Date().toLocaleTimeString();
//     setTerminalLogs(prev => [
//       ...prev.slice(-99), // Keep last 100 logs
//       { message, type, timestamp }
//     ]);
//   };

//   const formatUptime = (seconds) => {
//     const h = Math.floor(seconds / 3600);
//     const m = Math.floor((seconds % 3600) / 60);
//     const s = seconds % 60;
//     return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
//   };

//   const currentPatient = patients[currentPatientIndex];
//   const criticalAlerts = alerts.filter(a => a.priority === 1).length;

//   return (
//     <div className="app">
//       {/* Top Bar */}
//       <header className="top-bar">
//         <div className="top-bar-left">
//           <div className="logo">
//             <span className="logo-icon">🏥</span>
//             <span className="logo-text">ICU Monitoring Dashboard</span>
//           </div>
//         </div>
        
//         <div className="top-bar-center">
//           <ConnectionStatus connected={connected} />
//         </div>
        
//         <div className="top-bar-right">
//           <div className="uptime">
//             {formatUptime(uptime)}
//           </div>
//           {criticalAlerts > 0 && (
//             <div className="critical-badge">
//               🚨 {criticalAlerts} Critical Alerts
//             </div>
//           )}
//         </div>
//       </header>

//       {/* Main Content */}
//       <div className="main-container">
//         {/* Left Sidebar - Patient List */}
//         <aside className="left-sidebar">
//           <h3 className="sidebar-title">
//             👥 Active Patients ({patients.length})
//           </h3>
//           <div className="patient-list-compact">
//             {patients.map((patient, idx) => (
//               <div
//                 key={patient.patientID}
//                 className={`patient-card-compact ${idx === currentPatientIndex ? 'active' : ''}`}
//                 onClick={() => setCurrentPatientIndex(idx)}
//               >
//                 <div className="patient-compact-header">
//                   <span className="patient-name">{patient.name}</span>
//                   <span className={`status-badge ${patient.latestVitals ? 'status-stable' : 'status-monitoring'}`}>
//                     {patient.latestVitals ? 'STABLE' : 'MONITORING'}
//                   </span>
//                 </div>
//                 <div className="patient-compact-info">
//                   <span>Bed {patient.ward}</span>
//                   <span>•</span>
//                   <span>{patient.condition}</span>
//                   <span>•</span>
//                   <span>{patient.age} years</span>
//                 </div>
//                 {patient.latestVitals && (
//                   <div className="patient-compact-vitals">
//                     <span>💓 {patient.latestVitals.heart_rate}</span>
//                     <span>🩸 {patient.latestVitals.systolic_bp}/{patient.latestVitals.diastolic_bp}</span>
//                     <span>O₂ {patient.latestVitals.spo2}%</span>
//                   </div>
//                 )}
//               </div>
//             ))}
//           </div>
//         </aside>

//         {/* Center - Patient Monitor */}
//         <main className="center-content">
//           {currentPatient ? (
//             <>
//               <PatientCarousel
//                 patient={currentPatient}
//                 onNext={() => setCurrentPatientIndex((currentPatientIndex + 1) % patients.length)}
//                 onPrev={() => setCurrentPatientIndex((currentPatientIndex - 1 + patients.length) % patients.length)}
//                 onStopSimulation={() => stopSimulation(currentPatient.patientID)}
//                 isSimulating={activeSimulations.includes(currentPatient.patientID)}
//               />
              
//               {/* Drug Interaction Check Button */}
//               {currentPatient.medications && currentPatient.medications.length > 0 && (
//                 <button 
//                   className="drug-check-btn"
//                   onClick={() => setShowDrugModal(true)}
//                 >
//                   💊 Check Drug Interactions
//                 </button>
//               )}
//             </>
//           ) : (
//             <div className="no-patients">
//               <h2>No Patients Available</h2>
//               <p>Add patients to start monitoring</p>
//             </div>
//           )}
//         </main>

//         {/* Right Sidebar - Alerts & Terminal */}
//         <aside className="right-sidebar">
//           {/* Alerts Section */}
//           <div className="sidebar-section">
//             <AlertPanel alerts={alerts} />
//           </div>

//           {/* Medications Section */}
//           {currentPatient && (
//             <div className="sidebar-section">
//               <MedicationPanel medications={currentPatient.medications || []} />
//             </div>
//           )}

//           {/* Live Terminal */}
//           <div className="sidebar-section terminal-section">
//             <LiveTerminal logs={terminalLogs} />
//           </div>
//         </aside>
//       </div>

//       {/* Drug Interaction Modal */}
//       {showDrugModal && (
//         <DrugInteractionModal
//           patient={currentPatient}
//           onClose={() => setShowDrugModal(false)}
//         />
//       )}
//     </div>
//   );
// }

// export default App;
//file structure: intellicare-frontend/src/App.js
// import React, { useState, useEffect } from 'react';
// import api from './api';
// import './App.css';
// import PatientCarousel from './components/PatientCarousel';
// import AlertPanel from './components/AlertPanel';
// import MedicationPanel from './components/MedicationPanel';
// import LiveTerminal from './components/LiveTerminal';
// import ConnectionStatus from './components/ConnectionStatus';
// import DrugInteractionModal from './components/DrugInteractionModal';

// // API Configuration - CHANGE THIS TO YOUR SERVER
// // API base is configured in src/api.js

// function App() {
//   const [patients, setPatients] = useState([]);
//   const [currentPatientIndex, setCurrentPatientIndex] = useState(0);
//   const [alerts, setAlerts] = useState([]);
//   const [terminalLogs, setTerminalLogs] = useState([]);
//   const [connected, setConnected] = useState(false);
//   const [uptime, setUptime] = useState(0);
//   const [activeSimulations, setActiveSimulations] = useState([]);
//   const [showDrugModal, setShowDrugModal] = useState(false);

//   // Fetch initial data after testing API connection
//   const testApiConnection = async () => {
//     console.log('Testing API connection...');
//     try {
//       // Test 1: Basic endpoint
//       const response1 = await api.get('/');
//       console.log('Backend health check:', response1.data);
      
//       // Test 2: Patients endpoint
//       const response2 = await api.get('/api/patients');
//       console.log('Patients data:', response2.data);
      
//       return true;
//     } catch (error) {
//       console.error('API Connection failed:', error.message);
//       console.error('Error details:', error.response?.status, error.config?.url);
//       return false;
//     }
//   };

//   useEffect(() => {
//     testApiConnection().then(success => {
//       if (success) {
//         loadInitialData();
//         setConnected(true);
//       } else {
//         setConnected(false);
//       }
//     });
//   }, []);

//   // Auto-rotate patients every 10 seconds
//   useEffect(() => {
//     if (patients.length === 0) return;
    
//     const interval = setInterval(() => {
//       setCurrentPatientIndex(prev => (prev + 1) % patients.length);
//     }, 10000);
    
//     return () => clearInterval(interval);
//   }, [patients.length]);

//   // Poll for updates every 2 seconds
//   useEffect(() => {
//     const interval = setInterval(() => {
//       refreshData();
//     }, 2000);
    
//     return () => clearInterval(interval);
//   }, [patients]);

//   // Uptime counter
//   useEffect(() => {
//     const interval = setInterval(() => {
//       setUptime(prev => prev + 1);
//     }, 1000);
    
//     return () => clearInterval(interval);
//   }, []);

//   const loadInitialData = async () => {
//     try {
//       // Load patients
//   const patientsRes = await api.get('/api/patients');
//       if (patientsRes.data.status === 'success') {
//         const patientsData = patientsRes.data.patients;
//         const patientsWithDetails = await Promise.all(
//         patientsData.map(async (p) => {
//           const detailRes = await api.get(`/api/patient/${p.patientID}`);
//           return detailRes.data.status === 'success' ? detailRes.data.data : p;
//         })
//       );
//         setPatients(patientsData);
        
//         addLog('✓ Loaded ' + patientsData.length + ' patients', 'success');
        
//         // Start simulations for all patients
//         for (const patient of patientsData) {
//           await startSimulation(patient.patientID);
//         }
//       }
      
//       // Load alerts
//   const alertsRes = await api.get('/api/alerts');
//       if (alertsRes.data.status === 'success') {
//         setAlerts(alertsRes.data.alerts);
//         addLog('✓ Loaded ' + alertsRes.data.alerts.length + ' alerts', 'info');
//       }
      
//     } catch (error) {
//       console.error('Error loading data:', error);
//       addLog('✗ Error loading data', 'error');
//     }
//   };

//   const refreshData = async () => {
//     if (patients.length === 0) return;
    
//     try {
//       // Refresh current patient's vitals
//       const currentPatient = patients[currentPatientIndex];
//       if (!currentPatient) return;
      
//       //const vitalsRes = await axios.get(`/vitals/${currentPatient.patientID}/recent?count=1`);
//   const vitalsRes = await api.get(`/api/vitals/${currentPatient.patientID}/recent?count=1`);
//       if (vitalsRes.data.status === 'success' && vitalsRes.data.readings.length > 0) {
//         const latest = vitalsRes.data.readings[0];
        
//         // Update patient data
//         setPatients(prev => prev.map(p => 
//           p.patientID === currentPatient.patientID 
//             ? { ...p, latestVitals: latest }
//             : p
//         ));
        
//         // Add to terminal
//         addLog(
//           `← Received: HR=${latest.heart_rate}, BP=${latest.systolic_bp}/${latest.diastolic_bp}, O₂=${latest.spo2}%`,
//           'receive'
//         );
//       }
      
//       // Refresh alerts
//   const alertsRes = await api.get('/api/alerts');
//       if (alertsRes.data.status === 'success') {
//         const newAlerts = alertsRes.data.alerts;
//         if (newAlerts.length > alerts.length) {
//           addLog('← Received: Alert: ' + newAlerts[0].message, 'alert');
//         }
//         setAlerts(newAlerts);
//       }
      
//     } catch (error) {
//       console.error('Error refreshing data:', error);
//     }
//   };

//   const startSimulation = async (patientID) => {
//     try {
//   await api.post(`/api/simulate/start/${patientID}`, { intervalMs: 2000 });
//       setActiveSimulations(prev => [...prev, patientID]);
//       addLog(`→ Started simulation for Patient #${patientID}`, 'send');
//     } catch (error) {
//       console.error('Error starting simulation:', error);
//     }
//   };

//   const stopSimulation = async (patientID) => {
//     try {
//   await api.post(`/api/simulate/stop/${patientID}`);
//       setActiveSimulations(prev => prev.filter(id => id !== patientID));
//       addLog(`→ Stopped simulation for Patient #${patientID}`, 'send');
//     } catch (error) {
//       console.error('Error stopping simulation:', error);
//     }
//   };

  

//   const addLog = (message, type = 'info') => {
//     const timestamp = new Date().toLocaleTimeString();
//     setTerminalLogs(prev => [
//       ...prev.slice(-99), // Keep last 100 logs
//       { message, type, timestamp }
//     ]);
//   };

//   const formatUptime = (seconds) => {
//     const h = Math.floor(seconds / 3600);
//     const m = Math.floor((seconds % 3600) / 60);
//     const s = seconds % 60;
//     return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
//   };

//   const currentPatient = patients[currentPatientIndex];
//   const criticalAlerts = alerts.filter(a => a.priority === 1).length;

//   return (
//     <div className="app">
//       {/* Top Bar */}
//       <header className="top-bar">
//         <div className="top-bar-left">
//           <div className="logo">
//             <span className="logo-icon">🏥</span>
//             <span className="logo-text">ICU Monitoring Dashboard</span>
//           </div>
//         </div>
        
//         <div className="top-bar-center">
//           <ConnectionStatus connected={connected} />
//         </div>
        
//         <div className="top-bar-right">
//           <div className="uptime">
//             {formatUptime(uptime)}
//           </div>
//           {criticalAlerts > 0 && (
//             <div className="critical-badge">
//               🚨 {criticalAlerts} Critical Alerts
//             </div>
//           )}
//         </div>
//       </header>

//       {/* Main Content */}
//       <div className="main-container">
//         {/* Left Sidebar - Patient List */}
//         <aside className="left-sidebar">
//           <h3 className="sidebar-title">
//             👥 Active Patients ({patients.length})
//           </h3>
//           <div className="patient-list-compact">
//             {patients.map((patient, idx) => (
//               <div
//                 key={patient.patientID}
//                 className={`patient-card-compact ${idx === currentPatientIndex ? 'active' : ''}`}
//                 onClick={() => setCurrentPatientIndex(idx)}
//               >
//                 <div className="patient-compact-header">
//                   <span className="patient-name">{patient.name}</span>
//                   <span className={`status-badge ${patient.latestVitals ? 'status-stable' : 'status-monitoring'}`}>
//                     {patient.latestVitals ? 'STABLE' : 'MONITORING'}
//                   </span>
//                 </div>
//                 <div className="patient-compact-info">
//                   <span>Bed {patient.ward}</span>
//                   <span>•</span>
//                   <span>{patient.condition}</span>
//                   <span>•</span>
//                   <span>{patient.age} years</span>
//                 </div>
//                 {patient.latestVitals && (
//                   <div className="patient-compact-vitals">
//                     <span>💓 {patient.latestVitals.heart_rate}</span>
//                     <span>🩸 {patient.latestVitals.systolic_bp}/{patient.latestVitals.diastolic_bp}</span>
//                     <span>O₂ {patient.latestVitals.spo2}%</span>
//                   </div>
//                 )}
//               </div>
//             ))}
//           </div>
//         </aside>

//         {/* Center - Patient Monitor */}
//         <main className="center-content">
//           {currentPatient ? (
//             <>
//               <PatientCarousel
//                 patient={currentPatient}
//                 onNext={() => setCurrentPatientIndex((currentPatientIndex + 1) % patients.length)}
//                 onPrev={() => setCurrentPatientIndex((currentPatientIndex - 1 + patients.length) % patients.length)}
//                 onStopSimulation={() => stopSimulation(currentPatient.patientID)}
//                 isSimulating={activeSimulations.includes(currentPatient.patientID)}
//               />
              
//               {/* Drug Interaction Check Button */}
//               {currentPatient.medications && currentPatient.medications.length > 0 && (
//                 <button 
//                   className="drug-check-btn"
//                   onClick={() => setShowDrugModal(true)}
//                 >
//                   💊 Check Drug Interactions
//                 </button>
//               )}
//             </>
//           ) : (
//             <div className="no-patients">
//               <h2>No Patients Available</h2>
//               <p>Add patients to start monitoring</p>
//             </div>
//           )}
//         </main>

//         {/* Right Sidebar - Alerts & Terminal */}
//         <aside className="right-sidebar">
//           {/* Alerts Section */}
//           <div className="sidebar-section">
//             <AlertPanel alerts={alerts} />
//           </div>

//           {/* Medications Section */}
//           {currentPatient && (
//             <div className="sidebar-section">
//               <MedicationPanel medications={currentPatient.medications || []} />
//             </div>
//           )}

//           {/* Live Terminal */}
//           <div className="sidebar-section terminal-section">
//             <LiveTerminal logs={terminalLogs} />
//           </div>
//         </aside>
//       </div>

//       {/* Drug Interaction Modal */}
//       {showDrugModal && (
//         <DrugInteractionModal
//           patient={currentPatient}
//           onClose={() => setShowDrugModal(false)}
//         />
//       )}
//     </div>
//   );
// }

// export default App;


import React, { useState, useEffect, useCallback } from 'react';
import api from './api';
import './App.css';
import PatientCarousel from './components/PatientCarousel';
import AlertPanel from './components/AlertPanel';
import MedicationPanel from './components/MedicationPanel';
import LiveTerminal from './components/LiveTerminal';
import ConnectionStatus from './components/ConnectionStatus';
import DrugInteractionModal from './components/DrugInteractionModal';

function App() {
  const [patients, setPatients] = useState([]);
  const [currentPatientIndex, setCurrentPatientIndex] = useState(0);
  const [alerts, setAlerts] = useState([]);
  const [terminalLogs, setTerminalLogs] = useState([]);
  const [connected, setConnected] = useState(false);
  const [uptime, setUptime] = useState(0);
  const [showDrugModal, setShowDrugModal] = useState(false);
  const [activeSimulations, setActiveSimulations] = useState([]);

  // ✅ Define addLog first
  const addLog = useCallback((message, type = 'info') => {
    const timestamp = new Date().toLocaleTimeString();
    setTerminalLogs(prev => [
      ...prev.slice(-20),
      { message, type, timestamp }
    ]);
  }, []);

  // ✅ Start simulation for a patient
  const startSimulation = useCallback(async (patientID) => {
    try {
      await api.post(`/api/simulate/start/${patientID}`, { intervalMs: 2000 });
      setActiveSimulations(prev => [...prev, patientID]);
      addLog(`→ Started simulation for Patient #${patientID}`, 'send');
    } catch (error) {
      console.error('Error starting simulation:', error);
    }
  }, [addLog]);

  // ✅ Stop simulation
  const stopSimulation = useCallback(async (patientID) => {
    try {
      await api.post(`/api/simulate/stop/${patientID}`);
      setActiveSimulations(prev => prev.filter(id => id !== patientID));
      addLog(`→ Stopped simulation for Patient #${patientID}`, 'send');
    } catch (error) {
      console.error('Error stopping simulation:', error);
    }
  }, [addLog]);

  // ✅ Load initial data
  const loadInitialData = useCallback(async () => {
    try {
      console.log('Loading patients...');
      
      // Load patients
      const patientsRes = await api.get('/api/patients');
      console.log('Patients response:', patientsRes.data);
      
      if (patientsRes.data.status === 'success') {
        const patientsData = patientsRes.data.patients;
        console.log('Setting patients:', patientsData);
        setPatients(patientsData);
        addLog(`✓ Loaded ${patientsData.length} patients`, 'success');
        
        // Start simulations for all patients
        for (const patient of patientsData) {
          await startSimulation(patient.patientID);
        }
      }
      
      // Load alerts
      const alertsRes = await api.get('/api/alerts');
      console.log('Alerts response:', alertsRes.data);
      
      if (alertsRes.data.status === 'success') {
        setAlerts(alertsRes.data.alerts);
        addLog(`✓ Loaded ${alertsRes.data.alerts.length} alerts`, 'info');
      }
      
      setConnected(true);
      
    } catch (error) {
      console.error('Error loading data:', error);
      console.error('Error response:', error.response);
      addLog('✗ Connection error', 'error');
      setConnected(false);
    }
  }, [addLog, startSimulation]);

  // ✅ Fetch latest vitals for current patient
  const fetchLatestVitals = useCallback(async () => {
    const currentPatient = patients[currentPatientIndex];
    if (!currentPatient || !connected) return;
    
    try {
      const vitalsRes = await api.get(`/api/vitals/${currentPatient.patientID}/recent?count=1`);
      if (vitalsRes.data.status === 'success' && vitalsRes.data.readings.length > 0) {
        const latest = vitalsRes.data.readings[0];
        
        // Update patient with latest vitals
        setPatients(prev => prev.map(p => 
          p.patientID === currentPatient.patientID 
            ? { ...p, latestVitals: latest }
            : p
        ));
        
        addLog(`← HR:${latest.heart_rate} BP:${latest.systolic_bp}/${latest.diastolic_bp} O₂:${latest.spo2}%`, 'receive');
      }
    } catch (error) {
      console.error('Error fetching vitals:', error);
    }
  }, [patients, currentPatientIndex, connected, addLog]);

  // ✅ Poll for new alerts
  const fetchAlerts = useCallback(async () => {
    if (!connected) return;
    
    try {
      const alertsRes = await api.get('/api/alerts');
      if (alertsRes.data.status === 'success') {
        const newAlerts = alertsRes.data.alerts;
        
        // Check if new alerts arrived
        if (newAlerts.length > alerts.length) {
          const newAlert = newAlerts[newAlerts.length - 1];
          addLog(`⚠ Alert: ${newAlert.message}`, 'alert');
        }
        
        setAlerts(newAlerts);
      }
    } catch (error) {
      console.error('Error fetching alerts:', error);
    }
  }, [connected, alerts.length, addLog]);

  // Fetch initial data
  useEffect(() => {
    console.log('App mounted, loading data...');
    loadInitialData();
  }, [loadInitialData]);

  // Auto-rotate patients every 10 seconds
  useEffect(() => {
    if (patients.length === 0) return;
    
    const interval = setInterval(() => {
      setCurrentPatientIndex(prev => (prev + 1) % patients.length);
    }, 10000);
    
    return () => clearInterval(interval);
  }, [patients.length]);

  // Real-time vitals updates (every 2 seconds)
  useEffect(() => {
    if (!connected || patients.length === 0) return;
    
    const interval = setInterval(() => {
      fetchLatestVitals();
    }, 2000);
    
    return () => clearInterval(interval);
  }, [connected, patients, fetchLatestVitals]);

  // Real-time alerts updates (every 3 seconds)
  useEffect(() => {
    if (!connected) return;
    
    const interval = setInterval(() => {
      fetchAlerts();
    }, 3000);
    
    return () => clearInterval(interval);
  }, [connected, fetchAlerts]);

  // Uptime counter
  useEffect(() => {
    const interval = setInterval(() => {
      setUptime(prev => prev + 1);
    }, 1000);
    return () => clearInterval(interval);
  }, []);

  const formatUptime = (seconds) => {
    const m = Math.floor(seconds / 60);
    const s = seconds % 60;
    return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
  };

  const currentPatient = patients[currentPatientIndex];
  const criticalAlerts = alerts.filter(a => a.priority === 1).length;
  const isSimulating = currentPatient && activeSimulations.includes(currentPatient.patientID);

  return (
    <div className="app">
      <header className="top-bar">
        <div className="top-bar-left">
          <div className="logo">
            <span className="logo-icon">🏥</span>
            <span className="logo-text">ICU Monitoring Dashboard</span>
          </div>
        </div>
        
        <div className="top-bar-center">
          <ConnectionStatus connected={connected} />
        </div>
        
        <div className="top-bar-right">
          <div className="uptime">{formatUptime(uptime)}</div>
          {criticalAlerts > 0 && (
            <div className="critical-badge">🚨 {criticalAlerts} Critical Alerts</div>
          )}
        </div>
      </header>

      <div className="main-container">
        <aside className="left-sidebar">
          <h3 className="sidebar-title">👥 Active Patients ({patients.length})</h3>
          <div className="patient-list-compact">
            {patients.map((patient, idx) => (
              <div
                key={patient.patientID}
                className={`patient-card-compact ${idx === currentPatientIndex ? 'active' : ''}`}
                onClick={() => setCurrentPatientIndex(idx)}
              >
                <div className="patient-compact-header">
                  <span className="patient-name">{patient.name}</span>
                  <span className="status-badge status-stable">STABLE</span>
                </div>
                <div className="patient-compact-info">
                  <span>Bed {patient.ward}</span>
                  <span>•</span>
                  <span>{patient.condition}</span>
                  <span>•</span>
                  <span>{patient.age} years</span>
                </div>
                {patient.latestVitals && (
                  <div className="patient-compact-vitals">
                    <span>💓 {patient.latestVitals.heart_rate}</span>
                    <span>🩸 {patient.latestVitals.systolic_bp}/{patient.latestVitals.diastolic_bp}</span>
                    <span>O₂ {patient.latestVitals.spo2}%</span>
                  </div>
                )}
              </div>
            ))}
          </div>
        </aside>

        <main className="center-content">
          {currentPatient ? (
            <>
              <PatientCarousel
                patient={currentPatient}
                onNext={() => setCurrentPatientIndex((currentPatientIndex + 1) % patients.length)}
                onPrev={() => setCurrentPatientIndex((currentPatientIndex - 1 + patients.length) % patients.length)}
                onStopSimulation={() => stopSimulation(currentPatient.patientID)}
                isSimulating={isSimulating}
              />
              
              {currentPatient.medications && currentPatient.medications.length > 0 && (
                <button className="drug-check-btn" onClick={() => setShowDrugModal(true)}>
                  💊 Check Drug Interactions
                </button>
              )}
            </>
          ) : patients.length > 0 ? (
            <div className="no-patients">
              <h2>Click a patient to view details</h2>
              <p>Select a patient from the sidebar</p>
            </div>
          ) : (
            <div className="no-patients">
              <h2>Loading patients...</h2>
              <p>Please wait while we connect to the backend</p>
            </div>
          )}
        </main>

        <aside className="right-sidebar">
          <div className="sidebar-section">
            <AlertPanel alerts={alerts} />
          </div>

          {currentPatient && (
            <div className="sidebar-section">
              <MedicationPanel medications={currentPatient.medications || []} />
            </div>
          )}

          <div className="sidebar-section terminal-section">
            <LiveTerminal logs={terminalLogs} />
          </div>
        </aside>
      </div>

      {showDrugModal && currentPatient && (
        <DrugInteractionModal
          patient={currentPatient}
          onClose={() => setShowDrugModal(false)}
        />
      )}
    </div>
  );
}

export default App;