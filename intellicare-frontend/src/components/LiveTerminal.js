//file structure: intellicare-frontend/src/components/LiveTerminal.js
import './LiveTerminal.css';
import React, { useEffect, useRef } from 'react';

function LiveTerminal({ logs }) {
  const terminalRef = useRef(null);

  useEffect(() => {
    if (terminalRef.current) {
      terminalRef.current.scrollTop = terminalRef.current.scrollHeight;
    }
  }, [logs]);

  const getLogColor = (type) => {
    switch(type) {
      case 'receive': return '#00ffaa';
      case 'send': return '#00d4ff';
      case 'alert': return '#ff3366';
      case 'error': return '#ff6b35';
      case 'success': return '#00ff88';
      default: return '#8892a6';
    }
  };

  const getLogPrefix = (type) => {
    switch(type) {
      case 'receive': return '←';
      case 'send': return '→';
      case 'alert': return '⚠';
      case 'error': return '✗';
      case 'success': return '✓';
      default: return '•';
    }
  };

  return (
    <div className="live-terminal">
      <div className="terminal-header">
        <span className="terminal-title">📡 WebSocket Activity (Live)</span>
        <span className="terminal-status">● Connected</span>
      </div>
      
      <div className="terminal-content" ref={terminalRef}>
        {logs.map((log, idx) => (
          <div 
            key={idx} 
            className="terminal-line"
            style={{ color: getLogColor(log.type) }}
          >
            <span className="terminal-time">{log.timestamp}</span>
            <span className="terminal-prefix">{getLogPrefix(log.type)}</span>
            <span className="terminal-message">{log.message}</span>
          </div>
        ))}
      </div>
    </div>
  );
}

export default LiveTerminal;