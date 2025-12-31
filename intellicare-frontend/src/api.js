// // src/api.js
// import axios from 'axios';

// // Create axios instance with base URL
// const api = axios.create({
//   baseURL: 'http://localhost:8080',
//   timeout: 10000,
//   headers: {
//     'Content-Type': 'application/json',
//   },
// });

// // Request interceptor
// api.interceptors.request.use(
//   config => {
//     console.log(`API Request: ${config.method.toUpperCase()} ${config.baseURL}${config.url}`);
//     return config;
//   },
//   error => {
//     console.error('API Request Error:', error);
//     return Promise.reject(error);
//   }
// );

// // Response interceptor
// api.interceptors.response.use(
//   response => {
//     console.log(`API Response: ${response.status} ${response.config.url}`);
//     return response;
//   },
//   error => {
//     console.error('API Response Error:', error.response?.status, error.message);
//     return Promise.reject(error);
//   }
// );

// export default api;


// src/api.js
import axios from 'axios';

// Create axios instance with base URL
const api = axios.create({
  baseURL: 'http://localhost:8080',
  timeout: 10000,
  headers: {
    'Content-Type': 'application/json',
  },
});

// Request interceptor
api.interceptors.request.use(
  config => {
    console.log(`API Request: ${config.method.toUpperCase()} ${config.baseURL}${config.url}`);
    return config;
  },
  error => {
    console.error('API Request Error:', error);
    return Promise.reject(error);
  }
);

// Response interceptor
api.interceptors.response.use(
  response => {
    console.log(`API Response: ${response.status} ${response.config.url}`);
    return response;
  },
  error => {
    console.error('API Response Error:', error.response?.status, error.message);
    return Promise.reject(error);
  }
);

export default api;