//file structure: intellicare-frontend/src/components/ECGWaveform.js
import React, { useEffect, useRef } from 'react';

function ECGWaveform({ heartRate }) {
  const canvasRef = useRef(null);
  const animationRef = useRef(null);
  const xPos = useRef(0);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    const width = canvas.width;
    const height = canvas.height;
    const centerY = height / 2;

    const draw = () => {
      // Clear with fade effect
      ctx.fillStyle = 'rgba(10, 14, 26, 0.1)';
      ctx.fillRect(0, 0, width, height);

      // Draw grid
      ctx.strokeStyle = 'rgba(0, 255, 170, 0.1)';
      ctx.lineWidth = 1;
      for (let i = 0; i < width; i += 20) {
        ctx.beginPath();
        ctx.moveTo(i, 0);
        ctx.lineTo(i, height);
        ctx.stroke();
      }
      for (let i = 0; i < height; i += 20) {
        ctx.beginPath();
        ctx.moveTo(0, i);
        ctx.lineTo(width, i);
        ctx.stroke();
      }

      // Generate ECG waveform
      const speed = (heartRate / 60) * 2; // Speed based on heart rate
      xPos.current += speed;
      if (xPos.current > width) xPos.current = 0;

      const beatWidth = width / (heartRate / 60 * 5); // Width of one heartbeat
      const phase = (xPos.current % beatWidth) / beatWidth;

      let y = centerY;
      
      // Generate PQRST complex
      if (phase < 0.1) {
        // P wave
        y = centerY - Math.sin(phase * Math.PI * 10) * 15;
      } else if (phase < 0.2) {
        // PR segment
        y = centerY;
      } else if (phase < 0.25) {
        // Q wave
        y = centerY + 10;
      } else if (phase < 0.35) {
        // R wave (main spike)
        y = centerY - Math.sin((phase - 0.25) * Math.PI * 10) * 80;
      } else if (phase < 0.4) {
        // S wave
        y = centerY + 15;
      } else if (phase < 0.5) {
        // ST segment
        y = centerY;
      } else if (phase < 0.7) {
        // T wave
        y = centerY - Math.sin((phase - 0.5) * Math.PI * 5) * 25;
      } else {
        // Baseline
        y = centerY;
      }

      // Draw waveform
      ctx.strokeStyle = '#00ffaa';
      ctx.lineWidth = 2;
      ctx.shadowBlur = 10;
      ctx.shadowColor = '#00ffaa';
      ctx.beginPath();
      ctx.moveTo(xPos.current - 2, y);
      ctx.lineTo(xPos.current, y);
      ctx.stroke();

      animationRef.current = requestAnimationFrame(draw);
    };

    draw();

    return () => {
      if (animationRef.current) {
        cancelAnimationFrame(animationRef.current);
      }
    };
  }, [heartRate]);

  return (
    <div className="ecg-waveform">
      <div className="ecg-header">
        <span className="ecg-title">📈 ECG Waveform (Real-Time via WebSocket)</span>
        <div className="ecg-controls">
          <button className="ecg-btn active">Live</button>
          <button className="ecg-btn">5 Min</button>
          <button className="ecg-btn">1 Hour</button>
        </div>
      </div>
      <canvas 
        ref={canvasRef} 
        width={1200} 
        height={200}
        style={{ width: '100%', height: '200px' }}
      />
    </div>
  );
}

export default ECGWaveform;