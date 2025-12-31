import React from 'react';
//file structure: intellicare-frontend/src/components/VitalDisplay.js
function VitalDisplay({ label, value, unit, icon, normalRange, status, trend, previous, isSmall }) {
  const getChange = () => {
    if (!previous) return null;
    const change = value - previous;
    if (Math.abs(change) < 1) return null;
    return change > 0 ? `↑ ${Math.abs(change).toFixed(0)}` : `↓ ${Math.abs(change).toFixed(0)}`;
  };

  const change = getChange();

  return (
    <div className={`vital-display ${status} ${isSmall ? 'vital-small' : ''}`}>
      <div className="vital-icon">{icon}</div>
      <div className="vital-label">{label}</div>
      <div className="vital-value">
        {value}
        <span className="vital-unit">{unit}</span>
      </div>
      <div className="vital-info">
        <span className="vital-range">Normal: {normalRange}</span>
        {change && <span className="vital-change">{change} from 2 min ago</span>}
        {trend && !change && <span className="vital-trend">{trend}</span>}
      </div>
      <div className={`vital-status-indicator status-${status}`}></div>
    </div>
  );
}

export default VitalDisplay;