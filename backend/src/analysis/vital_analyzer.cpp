#include "vital_analyzer.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

VitalAnalyzer::VitalAnalyzer() {}

bool VitalAnalyzer::isHeartRateNormal(int hr) {
    return hr >= 60 && hr <= 100;
}

bool VitalAnalyzer::isBloodPressureNormal(int systolic, int diastolic) {
    return (systolic >= 90 && systolic <= 140) && 
           (diastolic >= 60 && diastolic <= 90);
}

bool VitalAnalyzer::isSpO2Normal(int spo2) {
    return spo2 >= 95;
}

bool VitalAnalyzer::isTemperatureNormal(float temp) {
    return temp >= 36.1f && temp <= 37.2f;
}

float VitalAnalyzer::calculatePercentChange(float oldVal, float newVal) {
    if (oldVal == 0) return 0;
    return ((newVal - oldVal) / oldVal) * 100.0f;
}

std::vector<DetectedPattern> VitalAnalyzer::analyzePatterns(const std::vector<VitalRecord>& readings) {
    std::vector<DetectedPattern> patterns;
    
    if (readings.size() < 2) {
        return patterns;
    }
    
    // Check for sudden changes in each vital
    DetectedPattern hrPattern = detectSuddenChange(readings, "heart_rate");
    if (hrPattern.type != PATTERN_NONE) patterns.push_back(hrPattern);
    
    DetectedPattern bpPattern = detectSuddenChange(readings, "systolic_bp");
    if (bpPattern.type != PATTERN_NONE) patterns.push_back(bpPattern);
    
    DetectedPattern spo2Pattern = detectSuddenChange(readings, "spo2");
    if (spo2Pattern.type != PATTERN_NONE) patterns.push_back(spo2Pattern);
    
    DetectedPattern tempPattern = detectSuddenChange(readings, "temperature");
    if (tempPattern.type != PATTERN_NONE) patterns.push_back(tempPattern);
    
    // Check for trends if enough data
    if (readings.size() >= 5) {
        DetectedPattern hrTrend = detectTrend(readings, "heart_rate");
        if (hrTrend.type != PATTERN_NONE) patterns.push_back(hrTrend);
        
        DetectedPattern bpTrend = detectTrend(readings, "systolic_bp");
        if (bpTrend.type != PATTERN_NONE) patterns.push_back(bpTrend);
    }
    
    // Check for erratic patterns
    if (readings.size() >= 10) {
        DetectedPattern hrErratic = detectErratic(readings, "heart_rate");
        if (hrErratic.type != PATTERN_NONE) patterns.push_back(hrErratic);
    }
    
    return patterns;
}

DetectedPattern VitalAnalyzer::detectSuddenChange(const std::vector<VitalRecord>& readings, 
                                                   const std::string& vitalSign) {
    DetectedPattern pattern;
    
    if (readings.size() < 2) return pattern;
    
    // Compare last reading with 5 minutes ago (or oldest if less data)
    int lookbackIndex = std::max(0, (int)readings.size() - 6);
    const VitalRecord& old = readings[lookbackIndex];
    const VitalRecord& current = readings.back();
    
    float oldVal, newVal;
    std::string unit;
    
    if (vitalSign == "heart_rate") {
        oldVal = old.heart_rate;
        newVal = current.heart_rate;
        unit = "bpm";
    } else if (vitalSign == "systolic_bp") {
        oldVal = old.systolic_bp;
        newVal = current.systolic_bp;
        unit = "mmHg";
    } else if (vitalSign == "spo2") {
        oldVal = old.spo2;
        newVal = current.spo2;
        unit = "%";
    } else if (vitalSign == "temperature") {
        oldVal = old.temperature;
        newVal = current.temperature;
        unit = "°C";
    } else {
        return pattern;
    }
    
    float percentChange = calculatePercentChange(oldVal, newVal);
    float absoluteChange = newVal - oldVal;
    
    // Detect significant changes (>20% or specific thresholds)
    bool significantChange = false;
    
    if (vitalSign == "heart_rate" && std::abs(absoluteChange) > 20) {
        significantChange = true;
    } else if (vitalSign == "systolic_bp" && std::abs(absoluteChange) > 25) {
        significantChange = true;
    } else if (vitalSign == "spo2" && absoluteChange < -3) {
        significantChange = true;
    } else if (vitalSign == "temperature" && std::abs(absoluteChange) > 0.5f) {
        significantChange = true;
    }
    
    if (significantChange) {
        pattern.vitalSign = vitalSign;
        pattern.changeValue = absoluteChange;
        pattern.timeWindowMinutes = 5;
        
        if (absoluteChange > 0) {
            pattern.type = SUDDEN_SPIKE;
            std::ostringstream desc;
            desc << "Sudden spike in " << vitalSign << ": +" 
                 << std::abs(absoluteChange) << " " << unit << " in last 5 minutes";
            pattern.description = desc.str();
        } else {
            pattern.type = SUDDEN_DROP;
            std::ostringstream desc;
            desc << "Sudden drop in " << vitalSign << ": -" 
                 << std::abs(absoluteChange) << " " << unit << " in last 5 minutes";
            pattern.description = desc.str();
        }
        
        pattern.isCritical = (vitalSign == "spo2" && absoluteChange < -3) ||
                            (vitalSign == "heart_rate" && std::abs(absoluteChange) > 30);
    }
    
    return pattern;
}

DetectedPattern VitalAnalyzer::detectTrend(const std::vector<VitalRecord>& readings,
                                           const std::string& vitalSign) {
    DetectedPattern pattern;
    
    if (readings.size() < 5) return pattern;
    
    // Get last 5 readings
    std::vector<float> values;
    for (size_t i = readings.size() - 5; i < readings.size(); i++) {
        if (vitalSign == "heart_rate") {
            values.push_back(readings[i].heart_rate);
        } else if (vitalSign == "systolic_bp") {
            values.push_back(readings[i].systolic_bp);
        }
    }
    
    // Calculate trend (simple linear regression)
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    for (size_t i = 0; i < values.size(); i++) {
        float x = i;
        float y = values[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    float n = values.size();
    float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    // Detect significant trends
    if (vitalSign == "heart_rate" && std::abs(slope) > 2) {
        pattern.vitalSign = vitalSign;
        pattern.type = slope > 0 ? TRENDING_UP : TRENDING_DOWN;
        pattern.changeValue = slope * 5; // Change over 5 readings
        pattern.timeWindowMinutes = 10;
        
        std::ostringstream desc;
        desc << "Trending " << (slope > 0 ? "up" : "down") << ": "
             << (slope > 0 ? "+" : "") << std::fixed << std::setprecision(1) 
             << (slope * 5) << " bpm over 10 minutes";
        pattern.description = desc.str();
        pattern.isCritical = std::abs(slope * 5) > 15;
    } else if (vitalSign == "systolic_bp" && std::abs(slope) > 3) {
        pattern.vitalSign = vitalSign;
        pattern.type = slope > 0 ? TRENDING_UP : TRENDING_DOWN;
        pattern.changeValue = slope * 5;
        pattern.timeWindowMinutes = 10;
        
        std::ostringstream desc;
        desc << "BP trending " << (slope > 0 ? "up" : "down") << ": "
             << (slope > 0 ? "+" : "") << std::fixed << std::setprecision(1)
             << (slope * 5) << " mmHg over 10 minutes";
        pattern.description = desc.str();
        pattern.isCritical = std::abs(slope * 5) > 20;
    }
    
    return pattern;
}

DetectedPattern VitalAnalyzer::detectErratic(const std::vector<VitalRecord>& readings,
                                             const std::string& vitalSign) {
    DetectedPattern pattern;
    
    if (readings.size() < 10) return pattern;
    
    // Get last 10 readings
    std::vector<float> values;
    for (size_t i = readings.size() - 10; i < readings.size(); i++) {
        if (vitalSign == "heart_rate") {
            values.push_back(readings[i].heart_rate);
        }
    }
    
    float variability = calculateVariability(values);
    
    // High variability indicates erratic pattern
    if (vitalSign == "heart_rate" && variability > 10) {
        pattern.type = ERRATIC;
        pattern.vitalSign = vitalSign;
        pattern.changeValue = variability;
        pattern.timeWindowMinutes = 20;
        
        std::ostringstream desc;
        desc << "Erratic heart rate pattern detected (variability: " 
             << std::fixed << std::setprecision(1) << variability << " bpm)";
        pattern.description = desc.str();
        pattern.isCritical = variability > 15;
    }
    
    return pattern;
}

float VitalAnalyzer::calculateVariability(const std::vector<float>& values) {
    if (values.empty()) return 0;
    
    float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    
    float variance = 0;
    for (float val : values) {
        variance += (val - mean) * (val - mean);
    }
    variance /= values.size();
    
    return std::sqrt(variance);
}

RiskAssessment VitalAnalyzer::calculateRiskScore(const std::vector<VitalRecord>& recentReadings,
                                                 int alertCount,
                                                 int drugInteractionSeverity) {
    RiskAssessment assessment;
    
    if (recentReadings.empty()) {
        assessment.riskScore = 50;
        assessment.riskLevel = "MEDIUM";
        assessment.recommendations.push_back("No recent vital data available");
        return assessment;
    }
    
    const VitalRecord& latest = recentReadings.back();
    
    // Count abnormal vitals
    assessment.factors.abnormalVitalsCount = 0;
    if (!isHeartRateNormal(latest.heart_rate)) assessment.factors.abnormalVitalsCount++;
    if (!isBloodPressureNormal(latest.systolic_bp, latest.diastolic_bp)) assessment.factors.abnormalVitalsCount++;
    if (!isSpO2Normal(latest.spo2)) assessment.factors.abnormalVitalsCount++;
    if (!isTemperatureNormal(latest.temperature)) assessment.factors.abnormalVitalsCount++;
    
    // Set factors
    assessment.factors.criticalAlertsCount = alertCount;
    assessment.factors.drugInteractionSeverity = drugInteractionSeverity;
    
    // Detect patterns
    assessment.patterns = analyzePatterns(recentReadings);
    
    for (const auto& pattern : assessment.patterns) {
        if (pattern.type == TRENDING_DOWN || pattern.type == TRENDING_UP) {
            assessment.factors.hasDeteriorationTrend = true;
        }
        if (pattern.type == ERRATIC) {
            assessment.factors.hasErraticPattern = true;
        }
    }
    
    // Calculate risk score (0-100)
    int score = 0;
    
    // Abnormal vitals (up to 40 points)
    score += assessment.factors.abnormalVitalsCount * 10;
    
    // Critical alerts (up to 30 points)
    score += std::min(alertCount * 10, 30);
    
    // Drug interactions (up to 15 points)
    score += std::min(drugInteractionSeverity * 5, 15);
    
    // Patterns (up to 15 points)
    if (assessment.factors.hasDeteriorationTrend) score += 10;
    if (assessment.factors.hasErraticPattern) score += 5;
    
    assessment.riskScore = std::min(score, 100);
    
    // Determine risk level
    if (assessment.riskScore >= 75) {
        assessment.riskLevel = "CRITICAL";
    } else if (assessment.riskScore >= 50) {
        assessment.riskLevel = "HIGH";
    } else if (assessment.riskScore >= 25) {
        assessment.riskLevel = "MEDIUM";
    } else {
        assessment.riskLevel = "LOW";
    }
    
    // Generate recommendations
    if (!isHeartRateNormal(latest.heart_rate)) {
        if (latest.heart_rate > 100) {
            assessment.recommendations.push_back("Monitor for tachycardia - consider cardiac assessment");
        } else {
            assessment.recommendations.push_back("Monitor for bradycardia - check medications");
        }
    }
    
    if (!isSpO2Normal(latest.spo2)) {
        assessment.recommendations.push_back("Low oxygen saturation - increase O2 support");
    }
    
    if (!isBloodPressureNormal(latest.systolic_bp, latest.diastolic_bp)) {
        if (latest.systolic_bp > 140) {
            assessment.recommendations.push_back("Hypertension detected - consider antihypertensive medication");
        } else {
            assessment.recommendations.push_back("Hypotension detected - assess fluid status");
        }
    }
    
    if (assessment.factors.hasDeteriorationTrend) {
        assessment.recommendations.push_back("Deteriorating trend - increase monitoring frequency");
    }
    
    if (drugInteractionSeverity >= 3) {
        assessment.recommendations.push_back("Severe drug interactions - review medication regimen");
    }
    
    return assessment;
}

bool VitalAnalyzer::checkAbnormal(const VitalRecord& vitals, std::vector<std::string>& abnormalities) {
    bool hasAbnormality = false;
    
    if (!isHeartRateNormal(vitals.heart_rate)) {
        abnormalities.push_back("Heart rate: " + std::to_string(vitals.heart_rate) + " bpm");
        hasAbnormality = true;
    }
    
    if (!isBloodPressureNormal(vitals.systolic_bp, vitals.diastolic_bp)) {
        abnormalities.push_back("Blood pressure: " + std::to_string(vitals.systolic_bp) + 
                               "/" + std::to_string(vitals.diastolic_bp) + " mmHg");
        hasAbnormality = true;
    }
    
    if (!isSpO2Normal(vitals.spo2)) {
        abnormalities.push_back("SpO2: " + std::to_string(vitals.spo2) + "%");
        hasAbnormality = true;
    }
    
    if (!isTemperatureNormal(vitals.temperature)) {
        std::ostringstream ss;
        ss << "Temperature: " << std::fixed << std::setprecision(1) << vitals.temperature << "°C";
        abnormalities.push_back(ss.str());
        hasAbnormality = true;
    }
    
    return hasAbnormality;
}

std::string VitalAnalyzer::getVitalStatus(const VitalRecord& vitals) {
    int abnormalCount = 0;
    bool critical = false;
    
    if (!isHeartRateNormal(vitals.heart_rate)) {
        abnormalCount++;
        if (vitals.heart_rate < 40 || vitals.heart_rate > 140) critical = true;
    }
    
    if (!isBloodPressureNormal(vitals.systolic_bp, vitals.diastolic_bp)) {
        abnormalCount++;
        if (vitals.systolic_bp < 80 || vitals.systolic_bp > 180) critical = true;
    }
    
    if (!isSpO2Normal(vitals.spo2)) {
        abnormalCount++;
        if (vitals.spo2 < 90) critical = true;
    }
    
    if (!isTemperatureNormal(vitals.temperature)) {
        abnormalCount++;
        if (vitals.temperature < 35.5f || vitals.temperature > 39.0f) critical = true;
    }
    
    if (critical) return "CRITICAL";
    if (abnormalCount >= 2) return "UNSTABLE";
    if (abnormalCount == 1) return "MONITORING";
    return "STABLE";
}