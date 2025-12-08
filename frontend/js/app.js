// ==================== GLOBAL STATE ====================
let currentPatients = [];
let currentAlerts = [];

// ==================== INITIALIZATION ====================
document.addEventListener('DOMContentLoaded', function() {
    console.log('IntelliCare ICU Dashboard Initializing...');
    
    // Check server connection
    checkServerStatus();
    
    // Load initial data
    loadAllData();
    
    // Auto-refresh every 30 seconds
    setInterval(loadAllData, 30000);
});

// ==================== SERVER STATUS ====================
async function checkServerStatus() {
    const statusIndicator = document.querySelector('.status-indicator');
    const statusText = document.querySelector('.status-text');
    
    try {
        const response = await api.healthCheck();
        
        if (response.status === 'online') {
            statusIndicator.classList.add('online');
            statusIndicator.classList.remove('offline');
            statusText.textContent = 'Server Online';
            console.log('✅ Server online:', response);
        }
    } catch (error) {
        statusIndicator.classList.add('offline');
        statusIndicator.classList.remove('online');
        statusText.textContent = 'Server Offline';
        console.error('❌ Server offline:', error);
    }
}

// ==================== LOAD DATA ====================
async function loadAllData() {
    await Promise.all([
        loadPatients(),
        loadAlerts()
    ]);
}

// ==================== PATIENTS ====================
async function loadPatients() {
    const patientsList = document.getElementById('patientsList');
    
    try {
        const response = await api.getAllPatients();
        
        if (response.status === 'success' && response.patients) {
            currentPatients = response.patients;
            
            if (currentPatients.length === 0) {
                patientsList.innerHTML = `
                    <div class="empty-state">
                        <div class="empty-state-icon">👥</div>
                        <p>No patients yet. Click "Add Patient" to get started.</p>
                    </div>
                `;
                return;
            }
            
            patientsList.innerHTML = currentPatients.map(patient => `
                <div class="patient-card" onclick="showPatientDetails(${patient.patientID})">
                    <div class="patient-header">
                        <span class="patient-id">ID: ${patient.patientID}</span>
                        <span>${patient.gender === 'M' ? '👨' : '👩'}</span>
                    </div>
                    <div class="patient-name">${patient.name}</div>
                    <div class="patient-info">
                        <div>📅 Age: ${patient.age}</div>
                        <div>🏥 Ward: ${patient.ward}</div>
                        <div>💉 Blood: ${patient.bloodType || 'N/A'}</div>
                        <div>📋 ${patient.condition}</div>
                    </div>
                </div>
            `).join('');
            
            console.log(`✅ Loaded ${currentPatients.length} patients`);
        }
    } catch (error) {
        console.error('❌ Error loading patients:', error);
        patientsList.innerHTML = '<p class="loading">Error loading patients. Check console.</p>';
    }
}

// ==================== ALERTS ====================
async function loadAlerts() {
    const alertsList = document.getElementById('alertsList');
    
    try {
        const response = await api.getAllAlerts();
        
        if (response.status === 'success' && response.alerts) {
            currentAlerts = response.alerts;
            
            if (currentAlerts.length === 0) {
                alertsList.innerHTML = '<p class="loading">No alerts. All patients stable. ✅</p>';
                return;
            }
            
            // Show only top 5 most critical
            const topAlerts = currentAlerts.slice(0, 5);
            
            alertsList.innerHTML = topAlerts.map(alert => {
                const priority = ALERT_PRIORITY[alert.priority] || ALERT_PRIORITY[5];
                const date = new Date(alert.timestamp * 1000).toLocaleString();
                
                return `
                    <div class="alert-card priority-${alert.priority}">
                        <div class="alert-icon">${priority.icon}</div>
                        <div class="alert-content">
                            <div class="alert-priority" style="color: ${priority.color}">
                                ${priority.name}
                            </div>
                            <div class="alert-message">
                                <strong>Patient ${alert.patientID}:</strong> ${alert.message}
                            </div>
                            <div class="alert-meta">
                                ${date}
                            </div>
                        </div>
                    </div>
                `;
            }).join('');
            
            console.log(`✅ Loaded ${currentAlerts.length} alerts`);
        }
    } catch (error) {
        console.error('❌ Error loading alerts:', error);
        alertsList.innerHTML = '<p class="loading">Error loading alerts.</p>';
    }
}

// ==================== PATIENT DETAILS ====================
async function showPatientDetails(patientID) {
    const modal = document.getElementById('patientModal');
    const detailsDiv = document.getElementById('patientDetails');
    
    try {
        // Get patient info
        const patientResponse = await api.getPatient(patientID);
        
        if (patientResponse.status !== 'success') {
            alert('Patient not found');
            return;
        }
        
        const patient = patientResponse.data;
        
        // Get latest vitals (last 24 hours)
        const endTime = Math.floor(Date.now() / 1000);
        const startTime = endTime - (24 * 60 * 60);
        const vitalsResponse = await api.getVitals(patientID, startTime, endTime);
        
        const latestVital = vitalsResponse.readings && vitalsResponse.readings.length > 0 
            ? vitalsResponse.readings[vitalsResponse.readings.length - 1]
            : null;
        
        detailsDiv.innerHTML = `
            <div class="patient-details-header">
                <h2>${patient.name}</h2>
                <p>Patient ID: ${patient.patientID} | Age: ${patient.age} | Gender: ${patient.gender}</p>
                <p>Ward: ${patient.ward} | Blood Type: ${patient.bloodType || 'N/A'}</p>
                <p>Condition: ${patient.condition}</p>
                
                ${latestVital ? `
                    <div class="vitals-grid">
                        <div class="vital-box">
                            <div class="vital-label">Heart Rate</div>
                            <div class="vital-value">${latestVital.heart_rate} <small>bpm</small></div>
                        </div>
                        <div class="vital-box">
                            <div class="vital-label">Blood Pressure</div>
                            <div class="vital-value">${latestVital.systolic_bp}/${latestVital.diastolic_bp}</div>
                        </div>
                        <div class="vital-box">
                            <div class="vital-label">SpO2</div>
                            <div class="vital-value">${latestVital.spo2}%</div>
                        </div>
                        <div class="vital-box">
                            <div class="vital-label">Temperature</div>
                            <div class="vital-value">${latestVital.temperature}°C</div>
                        </div>
                    </div>
                ` : '<p style="margin-top: 20px;">No vital signs recorded yet.</p>'}
            </div>
            
            <div style="margin-top: 25px;">
                <h3>Medications</h3>
                ${patient.medications && patient.medications.length > 0 
                    ? `<ul>${patient.medications.map(med => `<li>${med}</li>`).join('')}</ul>`
                    : '<p>No medications recorded.</p>'}
            </div>
            
            <div style="margin-top: 25px;">
                <h3>Vital Signs History</h3>
                ${vitalsResponse.readings && vitalsResponse.readings.length > 0
                    ? `<p>Total readings: ${vitalsResponse.readings.length}</p>`
                    : '<p>No vital signs history.</p>'}
            </div>
        `;
        
        modal.style.display = 'block';
        
    } catch (error) {
        console.error('❌ Error loading patient details:', error);
        alert('Error loading patient details');
    }
}

function closePatientModal() {
    document.getElementById('patientModal').style.display = 'none';
}

// ==================== ADD PATIENT ====================
function showAddPatientModal() {
    document.getElementById('addPatientModal').style.display = 'block';
}

function closeAddPatientModal() {
    document.getElementById('addPatientModal').style.display = 'none';
    document.getElementById('addPatientForm').reset();
}

async function handleAddPatient(event) {
    event.preventDefault();
    
    const form = event.target;
    const formData = new FormData(form);
    
    const patientData = {
        patientID: parseInt(formData.get('patientID')),
        name: formData.get('name'),
        age: parseInt(formData.get('age')),
        gender: formData.get('gender'),
        ward: formData.get('ward'),
        condition: formData.get('condition'),
        admissionDate: formData.get('admissionDate'),
        bloodType: formData.get('bloodType') || ''
    };
    
    try {
        const response = await api.addPatient(patientData);
        
        if (response.status === 'success') {
            alert('✅ Patient added successfully!');
            closeAddPatientModal();
            loadPatients(); // Reload patient list
        }
    } catch (error) {
        console.error('❌ Error adding patient:', error);
        alert('Error adding patient. Check console for details.');
    }
}

// Close modals when clicking outside
window.onclick = function(event) {
    const patientModal = document.getElementById('patientModal');
    const addPatientModal = document.getElementById('addPatientModal');
    
    if (event.target === patientModal) {
        closePatientModal();
    }
    if (event.target === addPatientModal) {
        closeAddPatientModal();
    }
}