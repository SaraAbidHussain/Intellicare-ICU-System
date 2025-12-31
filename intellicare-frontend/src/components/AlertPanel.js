//file structure: intellicare-frontend/src/components/AlertPanel.js
import './AlertPanel.css';
import React from 'react';

function AlertPanel({ alerts }) {
  const formatTime = (timestamp) => {
    const now = Date.now() / 1000;
    const diff = Math.floor(now - timestamp);
    if (diff < 60) return 'just now';
    if (diff < 3600) return `${Math.floor(diff / 60)} min ago`;
    return `${Math.floor(diff / 3600)} hr ago`;
  };

  const getPriorityColor = (priority) => {
    if (priority === 1) return 'critical';
    if (priority === 2) return 'high';
    if (priority === 3) return 'medium';
    return 'low';
  };

  return (
    <div className="alert-panel">
      <div className="panel-header">
        <h3>🚨 Critical Alerts</h3>
        {alerts.length > 0 && (
          <span className="alert-count">{alerts.length}</span>
        )}
      </div>
      
      <div className="alert-list">
        {alerts.length === 0 ? (
          <div className="no-alerts">
            <p>✅ No active alerts</p>
            <span>All systems normal</span>
          </div>
        ) : (
          alerts.slice(0, 3).map(alert => (
            <div key={alert.alertID} className={`alert-item alert-${getPriorityColor(alert.priority)}`}>
              <div className="alert-header">
                <span className="alert-title">{alert.priorityString}</span>
                <span className="alert-time">{formatTime(alert.timestamp)}</span>
              </div>
              <div className="alert-message">{alert.message}</div>
              <div className="alert-patient">Patient #{alert.patientID}</div>
              <div className="alert-actions">
                <button className="alert-btn">Acknowledge</button>
                <button className="alert-btn">Notify Doctor</button>
              </div>
            </div>
          ))
        )}
      </div>
    </div>
  );
}

export default AlertPanel;