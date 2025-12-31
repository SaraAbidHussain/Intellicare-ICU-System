//file structure: intellicare-frontend/src/components/DrugInteractionModal.js
import React, { useState, useEffect } from 'react';
import api from '../api';
import './DrugInteractionModal.css';

function DrugInteractionModal({ patient, onClose }) {
  const [report, setReport] = useState(null);
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    if (patient && patient.medications && patient.medications.length > 0) {
      checkInteractions();
    }
  }, [patient]);

  const checkInteractions = async () => {
    if (!patient.medications || patient.medications.length === 0) return;

    setLoading(true);
    try {
      const response = await api.post('/api/drug-check', {
        medications: patient.medications
      });

      if (response.data.status === 'success') {
        setReport(response.data);
      }
    } catch (error) {
      console.error('Error checking drug interactions:', error);
    }
    setLoading(false);
  };

  const getSeverityColor = (severity) => {
    if (severity === 4) return '#ff3366'; // Contraindicated
    if (severity === 3) return '#ff6b35'; // Severe
    if (severity === 2) return '#ffb020'; // Moderate
    if (severity === 1) return '#ffd700'; // Mild
    return '#00d4ff';
  };

  const getSeverityLabel = (severityString) => {
    if (!severityString) return 'Unknown';
    return severityString.split(' ')[1] || severityString;
  };

  if (!patient) return null;

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="drug-modal" onClick={(e) => e.stopPropagation()}>
        <div className="modal-header">
          <h2>💊 Drug Interaction Analysis</h2>
          <button className="modal-close" onClick={onClose}>×</button>
        </div>

        <div className="modal-body">
          <div className="patient-drug-info">
            <h3>Patient: {patient.name}</h3>
            <div className="medication-list-modal">
              {patient.medications && patient.medications.map((med, idx) => (
                <span key={idx} className="medication-badge">{med}</span>
              ))}
            </div>
          </div>

          {loading ? (
            <div className="loading-container">
              <div className="loading-spinner"></div>
              <p>Analyzing drug interactions...</p>
            </div>
          ) : report ? (
            <div className="interaction-report">
              {/* Safety Status */}
              <div className={`safety-status ${report.isSafe ? 'safe' : 'unsafe'}`}>
                <div className="safety-icon">
                  {report.isSafe ? '✅' : '⚠️'}
                </div>
                <div className="safety-text">
                  <h3>{report.isSafe ? 'SAFE' : 'UNSAFE'}</h3>
                  <p>{report.isSafe ? 'No critical interactions detected' : 'Critical interactions found'}</p>
                </div>
              </div>

              {/* Stats */}
              <div className="interaction-stats">
                <div className="stat-card">
                  <div className="stat-label">Total Interactions</div>
                  <div className="stat-value">{report.totalInteractions}</div>
                </div>
                <div className="stat-card">
                  <div className="stat-label">Max Severity</div>
                  <div className="stat-value" style={{ color: getSeverityColor(report.maxSeverity) }}>
                    {report.maxSeverity > 0 ? getSeverityLabel(report.interactions?.[0]?.severityString) : 'None'}
                  </div>
                </div>
                <div className="stat-card">
                  <div className="stat-label">Critical Pairs</div>
                  <div className="stat-value" style={{ color: '#ff3366' }}>
                    {report.criticalPairs?.length || 0}
                  </div>
                </div>
              </div>

              {/* Critical Pairs */}
              {report.criticalPairs && report.criticalPairs.length > 0 && (
                <div className="critical-pairs-section">
                  <h4>🚨 Critical Drug Pairs</h4>
                  <div className="critical-pairs-list">
                    {report.criticalPairs.map((pair, idx) => (
                      <div key={idx} className="critical-pair-item">
                        {pair}
                      </div>
                    ))}
                  </div>
                </div>
              )}

              {/* Detailed Interactions */}
              {report.interactions && report.interactions.length > 0 && (
                <div className="interactions-section">
                  <h4>Detailed Interactions</h4>
                  <div className="interactions-list">
                    {report.interactions.map((interaction, idx) => (
                      <div 
                        key={idx} 
                        className="interaction-item"
                        style={{ borderColor: getSeverityColor(interaction.severity) }}
                      >
                        <div className="interaction-header">
                          <span className="interaction-drugs">
                            💊 {interaction.drug1} ↔️ {interaction.drug2}
                          </span>
                          <span 
                            className="interaction-severity"
                            style={{ 
                              backgroundColor: `${getSeverityColor(interaction.severity)}20`,
                              color: getSeverityColor(interaction.severity),
                              border: `1px solid ${getSeverityColor(interaction.severity)}`
                            }}
                          >
                            {getSeverityLabel(interaction.severityString)}
                          </span>
                        </div>
                        <div className="interaction-description">
                          {interaction.description}
                        </div>
                      </div>
                    ))}
                  </div>
                </div>
              )}

              {report.totalInteractions === 0 && (
                <div className="no-interactions">
                  <div className="no-interactions-icon">✅</div>
                  <h3>No Known Interactions</h3>
                  <p>These medications appear safe to use together</p>
                </div>
              )}
            </div>
          ) : (
            <div className="no-medications">
              <p>No medications to check</p>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

export default DrugInteractionModal;