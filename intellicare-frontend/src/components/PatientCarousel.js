import React, { useState, useEffect, useCallback } from 'react';
import api from '../api';
import './PatientCarousel.css';
import VitalDisplay from './VitalDisplay';
import ECGWaveform from './ECGWaveform';

function PatientCarousel({ patient, onNext, onPrev, onStopSimulation, isSimulating }) {
  const [vitals, setVitals] = useState(null);
  const [stats, setStats] = useState(null);
  const [riskScore, setRiskScore] = useState(null);
  const [patterns, setPatterns] = useState([]);
  const [similarPatients, setSimilarPatients] = useState([]);

  // Define fetchLatestVitals with useCallback
  const fetchLatestVitals = useCallback(async () => {
    if (!patient) return;
    
    try {
      const vitalsRes = await api.get(`/api/vitals/${patient.patientID}/recent?count=1`);
      if (vitalsRes.data.status === 'success' && vitalsRes.data.readings.length > 0) {
        setVitals(vitalsRes.data.readings[0]);
      }
    } catch (error) {
      console.error('Error fetching vitals:', error);
    }
  }, [patient?.patientID]); // ✅ Use optional chaining

  // Define fetchPatientData with useCallback
  const fetchPatientData = useCallback(async () => {
    if (!patient) return;
    
    try {
      // Fetch stats
      const statsRes = await api.get(`/api/vitals/${patient.patientID}/stats`);
      if (statsRes.data.status === 'success') {
        setStats(statsRes.data);
      }

      // Fetch risk score
      const riskRes = await api.get(`/api/patient/${patient.patientID}/risk-score`);
      if (riskRes.data.status === 'success') {
        setRiskScore(riskRes.data);
      }

      // Fetch pattern analysis
      const patternRes = await api.get(`/api/vitals/${patient.patientID}/analyze`);
      if (patternRes.data.status === 'success') {
        setPatterns(patternRes.data.patterns || []);
      }

      // Fetch similar patients
      const similarRes = await api.get(`/api/patients/${patient.patientID}/similar?k=3`);
      if (similarRes.data.status === 'success') {
        setSimilarPatients(similarRes.data.similarPatients || []);
      }

      // Fetch latest vitals
      await fetchLatestVitals();
    } catch (error) {
      console.error('Error fetching patient data:', error);
    }
  }, [patient, fetchLatestVitals]); // ✅ Add both dependencies

  // First useEffect - load data when patient changes
  useEffect(() => {
    if (patient) {
      fetchPatientData();
    }
  }, [patient, fetchPatientData]); // ✅ Add both dependencies

  // Second useEffect - real-time vitals updates
  useEffect(() => {
    if (!patient) return;
    
    const interval = setInterval(() => {
      fetchLatestVitals();
    }, 2000);
    
    return () => clearInterval(interval);
  }, [patient?.patientID, fetchLatestVitals]); // ✅ Use optional chaining

  if (!patient) return null;

  return (
    <div className="patient-carousel">
      {/* Navigation */}
      <button className="carousel-nav carousel-nav-left" onClick={onPrev}>
        ‹
      </button>
      <button className="carousel-nav carousel-nav-right" onClick={onNext}>
        ›
      </button>

      {/* Patient Header */}
      <div className="patient-monitor-header">
        <div className="patient-header-left">
          <h1 className="patient-monitor-name">{patient.name}</h1>
          <div className="patient-monitor-info">
            <span>Bed {patient.ward}</span>
            <span>•</span>
            <span>MRN: 2024-{String(patient.patientID).padStart(6, '0')}</span>
            <span>•</span>
            <span>Age: {patient.age}</span>
            <span>•</span>
            <span>{patient.condition}</span>
          </div>
        </div>
        <div className="patient-header-right">
          {riskScore && (
            <div className={`risk-badge risk-${riskScore.riskLevel?.toLowerCase()}`}>
              {riskScore.riskLevel}
            </div>
          )}
          {isSimulating && (
            <button className="stop-sim-btn" onClick={onStopSimulation}>
              ⏸ Pause
            </button>
          )}
        </div>
      </div>

      {/* Vital Signs Grid */}
      {vitals ? (
        <>
          <div className="vitals-grid">
            <VitalDisplay
              label="HEART RATE"
              value={vitals.heart_rate}
              unit="bpm"
              icon="💓"
              normalRange="60-100 bpm"
              status={getVitalStatus(vitals.heart_rate, 60, 100)}
              previous={stats?.averages?.heartRate}
            />
            
            <VitalDisplay
              label="BLOOD PRESSURE"
              value={`${vitals.systolic_bp}/${vitals.diastolic_bp}`}
              unit="mmHg"
              icon="🩸"
              normalRange="90-120 / 60-80"
              status={getVitalStatus(vitals.systolic_bp, 90, 140)}
              trend="↔ Stable"
            />
            
            <VitalDisplay
              label="OXYGEN SATURATION"
              value={vitals.spo2}
              unit="%"
              icon="🫁"
              normalRange="> 95%"
              status={vitals.spo2 >= 95 ? 'normal' : vitals.spo2 >= 90 ? 'warning' : 'critical'}
              previous={stats?.averages?.spo2}
            />
          </div>

          <div className="vitals-grid-secondary">
            <VitalDisplay
              label="TEMPERATURE"
              value={vitals.temperature.toFixed(1)}
              unit="°C"
              icon="🌡️"
              normalRange="36.5-37.5°C"
              status={getVitalStatus(vitals.temperature, 36.1, 37.5)}
              isSmall={true}
            />
            
            {/* Respiratory Rate - Not monitored by backend */}
            <div className="vital-display vital-display-placeholder vital-small">
              <div className="vital-icon">🌬️</div>
              <div className="vital-label">RESPIRATORY RATE</div>
              <div className="vital-value">
                --
                <span className="vital-unit">/min</span>
              </div>
              <div className="vital-info">
                <span className="vital-range">Not monitored</span>
              </div>
            </div>
            
            {/* Blood Glucose - Not monitored by backend */}
            <div className="vital-display vital-display-placeholder vital-small">
              <div className="vital-icon">🩸</div>
              <div className="vital-label">BLOOD GLUCOSE</div>
              <div className="vital-value">
                --
                <span className="vital-unit">mg/dL</span>
              </div>
              <div className="vital-info">
                <span className="vital-range">Not monitored</span>
              </div>
            </div>
          </div>

          {/* ECG Waveform */}
          <div className="ecg-container">
            <ECGWaveform heartRate={vitals.heart_rate} />
          </div>

          {/* Pattern Detection Results */}
          {patterns.length > 0 && (
            <div className="patterns-section">
              <h3 className="section-title">⚠️ Detected Patterns (Backend Analysis)</h3>
              <div className="patterns-grid">
                {patterns.map((pattern, idx) => (
                  <div key={idx} className={`pattern-card ${pattern.isCritical ? 'critical' : ''}`}>
                    <div className="pattern-icon">
                      {pattern.isCritical ? '🔴' : '🟡'}
                    </div>
                    <div className="pattern-info">
                      <div className="pattern-title">{pattern.vitalSign?.replace('_', ' ')}</div>
                      <div className="pattern-desc">{pattern.description}</div>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          )}

          {/* Similar Patients */}
          {similarPatients.length > 0 && (
            <div className="similar-patients-section">
              <h3 className="section-title">👥 Similar Patients (KD-Tree Clustering)</h3>
              <div className="similar-patients-grid">
                {similarPatients.map((sp, idx) => (
                  <div key={idx} className="similar-patient-card">
                    <div className="sp-header">
                      <span className="sp-id">Patient #{sp.patientID}</span>
                      <span className="sp-similarity">
                        Similarity: {sp.similarity?.toFixed(2)}
                      </span>
                    </div>
                    <div className="sp-vitals">
                      <span>💓 {sp.vitals?.heartRate}</span>
                      <span>🩸 {sp.vitals?.systolicBP}/{sp.vitals?.diastolicBP}</span>
                      <span>O₂ {sp.vitals?.spo2}%</span>
                      <span>🌡️ {sp.vitals?.temperature?.toFixed(1)}°C</span>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          )}
        </>
      ) : (
        <div className="no-vitals">
          <div className="loading-spinner"></div>
          <p>Loading vital signs...</p>
        </div>
      )}
    </div>
  );
}

// Helper function
function getVitalStatus(value, min, max) {
  if (value < min || value > max) {
    if (value < min * 0.8 || value > max * 1.2) return 'critical';
    return 'warning';
  }
  return 'normal';
}

export default PatientCarousel;