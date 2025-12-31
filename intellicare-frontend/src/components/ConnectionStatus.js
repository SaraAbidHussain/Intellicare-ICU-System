//file structure: intellicare-frontend/src/components/ConnectionStatus.js
import React from 'react';

function ConnectionStatus({ connected }) {
  return (
    <div className={`connection-status ${connected ? 'connected' : 'disconnected'}`}>
      <div className="status-dot"></div>
      <span className="status-text">
        {connected ? 'WebSocket Connected' : 'Disconnected'}
      </span>
    </div>
  );
}

export default ConnectionStatus;