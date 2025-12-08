// API Configuration
const API_CONFIG = {
    BASE_URL: 'http://13.60.91.171:8080',  // Your AWS EC2 IP
    ENDPOINTS: {
        HEALTH: '/',
        PATIENTS: '/api/patients',
        PATIENT: '/api/patient',
        VITALS: '/api/vitals',
        ALERTS: '/api/alerts',
        ALERT: '/api/alert'
    }
};

// Alert priority mapping
const ALERT_PRIORITY = {
    1: { name: 'CRITICAL', color: '#dc3545', icon: '🔴' },
    2: { name: 'HIGH', color: '#fd7e14', icon: '🟠' },
    3: { name: 'MEDIUM', color: '#ffc107', icon: '🟡' },
    4: { name: 'LOW', color: '#28a745', icon: '🟢' },
    5: { name: 'INFO', color: '#17a2b8', icon: '🔵' }
};

// Export for use in other files
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { API_CONFIG, ALERT_PRIORITY };
}