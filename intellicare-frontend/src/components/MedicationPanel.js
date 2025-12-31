//file structure: intellicare-frontend/src/components/MedicationPanel.js
import React from 'react';
import './MedicationPanel.css';
function MedicationPanel({ medications }) {
  const getNextDose = (index) => {
    const hours = [8, 14, 20];
    const nextHour = hours[index % 3];
    return `${String(nextHour).padStart(2, '0')}:00`;
  };

  return (
    <div className="medication-panel">
      <div className="panel-header">
        <h3>💊 Current Medications</h3>
      </div>
      
      <div className="medication-list">
        {!medications || medications.length === 0 ? (
          <div className="no-medications">
            <p>No medications prescribed</p>
          </div>
        ) : (
          medications.map((med, idx) => (
            <div key={idx} className="medication-item">
              <div className="med-header">
                <span className="med-name">{med}</span>
                <span className="med-next">Next: {getNextDose(idx)}</span>
              </div>
              <div className="med-info">
                <span>Twice daily • With meals</span>
              </div>
            </div>
          ))
        )}
      </div>
    </div>
  );
}

export default MedicationPanel;