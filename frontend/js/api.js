// API Helper Functions
class ICU_API {
    constructor(baseUrl) {
        this.baseUrl = baseUrl;
    }

    // Generic fetch wrapper
    async request(endpoint, options = {}) {
        const url = `${this.baseUrl}${endpoint}`;
        
        try {
            const response = await fetch(url, {
                ...options,
                headers: {
                    'Content-Type': 'application/json',
                    ...options.headers
                }
            });

            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }

            return await response.json();
        } catch (error) {
            console.error('API Error:', error);
            throw error;
        }
    }

    // Health check
    async healthCheck() {
        return await this.request('/');
    }

    // Patient operations
    async getAllPatients() {
        return await this.request('/api/patients');
    }

    async getPatient(patientId) {
        return await this.request(`/api/patient/${patientId}`);
    }

    async addPatient(patientData) {
        return await this.request('/api/patient', {
            method: 'POST',
            body: JSON.stringify(patientData)
        });
    }

    // Vital signs operations
    async getVitals(patientId, startTime, endTime) {
        let endpoint = `/api/vitals/${patientId}`;
        if (startTime && endTime) {
            endpoint += `?start=${startTime}&end=${endTime}`;
        }
        return await this.request(endpoint);
    }

    async addVitals(vitalData) {
        return await this.request('/api/vitals', {
            method: 'POST',
            body: JSON.stringify(vitalData)
        });
    }

    // Alert operations
    async getAllAlerts() {
        return await this.request('/api/alerts');
    }

    async createAlert(alertData) {
        return await this.request('/api/alert', {
            method: 'POST',
            body: JSON.stringify(alertData)
        });
    }
}

// Initialize API instance
const api = new ICU_API(API_CONFIG.BASE_URL);