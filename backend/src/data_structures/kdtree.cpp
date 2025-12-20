#include "kdtree.h"
#include <iomanip>
#include <limits>

// ==================== VitalPoint Implementation ====================

VitalPoint::VitalPoint() : patientID(0), timestamp(0) {
    coordinates.resize(5, 0.0);
}

VitalPoint::VitalPoint(const VitalRecord& record) 
    : patientID(record.patientID), timestamp(record.timestamp) {
    coordinates = {
        static_cast<double>(record.heart_rate),
        static_cast<double>(record.systolic_bp),
        static_cast<double>(record.diastolic_bp),
        static_cast<double>(record.spo2),
        static_cast<double>(record.temperature)
    };
}

VitalPoint::VitalPoint(int pid, const std::vector<double>& coords, long ts)
    : patientID(pid), coordinates(coords), timestamp(ts) {}

double VitalPoint::distanceTo(const VitalPoint& other) const {
    double sum = 0.0;
    for (size_t i = 0; i < coordinates.size(); i++) {
        double diff = coordinates[i] - other.coordinates[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void VitalPoint::display() const {
    std::cout << "Patient " << patientID 
              << " | HR:" << (int)coordinates[0]
              << " BP:" << (int)coordinates[1] << "/" << (int)coordinates[2]
              << " SpO2:" << (int)coordinates[3]
              << " Temp:" << std::fixed << std::setprecision(1) << coordinates[4];
}

// ==================== KDNode Implementation ====================

KDNode::KDNode(const VitalPoint& p, int dim) 
    : point(p), left(nullptr), right(nullptr), splitDimension(dim) {}

KDNode::~KDNode() {
    // Destructor handled by clearTree in KDTree
}

// ==================== KDTree Implementation ====================

KDTree::KDTree(int dims, const std::string& filePath)
    : root(nullptr), dimensions(dims), totalPoints(0), dataFilePath(filePath) {
    
    if (!dataFilePath.empty()) {
        loadFromDisk();
    }
    
    std::cout << "[KD-TREE] Initialized with " << dimensions << " dimensions" << std::endl;
}

KDTree::~KDTree() {
    if (!dataFilePath.empty()) {
        saveToDisk();
    }
    clearTree(root);
}

void KDTree::clearTree(KDNode* node) {
    if (node == nullptr) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

KDNode* KDTree::buildTree(std::vector<VitalPoint>& points, int depth) {
    if (points.empty()) return nullptr;
    
    int dim = depth % dimensions;
    
    // Sort points by current dimension
    std::sort(points.begin(), points.end(), 
              [dim](const VitalPoint& a, const VitalPoint& b) {
        return a.coordinates[dim] < b.coordinates[dim];
    });
    
    // Choose median
    int medianIdx = points.size() / 2;
    KDNode* node = new KDNode(points[medianIdx], dim);
    
    // Recursively build left and right subtrees
    std::vector<VitalPoint> leftPoints(points.begin(), points.begin() + medianIdx);
    std::vector<VitalPoint> rightPoints(points.begin() + medianIdx + 1, points.end());
    
    node->left = buildTree(leftPoints, depth + 1);
    node->right = buildTree(rightPoints, depth + 1);
    
    return node;
}

void KDTree::build(std::vector<VitalPoint>& points) {
    clearTree(root);
    root = buildTree(points, 0);
    totalPoints = points.size();
    std::cout << "[KD-TREE] Built tree with " << totalPoints << " points" << std::endl;
}

KDNode* KDTree::insertHelper(KDNode* node, const VitalPoint& point, int depth) {
    if (node == nullptr) {
        totalPoints++;
        return new KDNode(point, depth % dimensions);
    }
    
    int dim = depth % dimensions;
    
    if (point.coordinates[dim] < node->point.coordinates[dim]) {
        node->left = insertHelper(node->left, point, depth + 1);
    } else {
        node->right = insertHelper(node->right, point, depth + 1);
    }
    
    return node;
}

void KDTree::insert(const VitalPoint& point) {
    root = insertHelper(root, point, 0);
}

void KDTree::insert(const VitalRecord& record) {
    VitalPoint point(record);
    insert(point);
}

void KDTree::nearestNeighborHelper(KDNode* node, const VitalPoint& target, int depth,
                                   VitalPoint& best, double& bestDist) {
    if (node == nullptr) return;
    
    // Calculate distance to current node
    double dist = node->point.distanceTo(target);
    
    // Update best if closer (and not the same point)
    if (dist < bestDist && dist > 0.001) {
        bestDist = dist;
        best = node->point;
    }
    
    int dim = depth % dimensions;
    double diff = target.coordinates[dim] - node->point.coordinates[dim];
    
    // Decide which subtree to search first
    KDNode* nearSubtree = (diff < 0) ? node->left : node->right;
    KDNode* farSubtree = (diff < 0) ? node->right : node->left;
    
    // Search near subtree
    nearestNeighborHelper(nearSubtree, target, depth + 1, best, bestDist);
    
    // Check if we need to search far subtree
    if (diff * diff < bestDist) {
        nearestNeighborHelper(farSubtree, target, depth + 1, best, bestDist);
    }
}

VitalPoint KDTree::findNearest(const VitalPoint& target) {
    if (root == nullptr) {
        throw std::runtime_error("KD-Tree is empty");
    }
    
    VitalPoint best = root->point;
    double bestDist = std::numeric_limits<double>::max();
    
    nearestNeighborHelper(root, target, 0, best, bestDist);
    
    return best;
}

VitalPoint KDTree::findNearest(const VitalRecord& targetRecord) {
    VitalPoint target(targetRecord);
    return findNearest(target);
}

void KDTree::kNearestHelper(KDNode* node, const VitalPoint& target, int depth,
                           std::vector<std::pair<double, VitalPoint>>& neighbors, int k) {
    if (node == nullptr) return;
    
    double dist = node->point.distanceTo(target);
    
    // Add current node if it's not the query point itself
    if (dist > 0.001) {
        neighbors.push_back({dist, node->point});
        std::push_heap(neighbors.begin(), neighbors.end());
        
        if (neighbors.size() > k) {
            std::pop_heap(neighbors.begin(), neighbors.end());
            neighbors.pop_back();
        }
    }
    
    int dim = depth % dimensions;
    double diff = target.coordinates[dim] - node->point.coordinates[dim];
    
    KDNode* nearSubtree = (diff < 0) ? node->left : node->right;
    KDNode* farSubtree = (diff < 0) ? node->right : node->left;
    
    kNearestHelper(nearSubtree, target, depth + 1, neighbors, k);
    
    // Check if far subtree could contain closer points
    if (neighbors.size() < k || diff * diff < neighbors.front().first) {
        kNearestHelper(farSubtree, target, depth + 1, neighbors, k);
    }
}

std::vector<VitalPoint> KDTree::findKNearest(const VitalPoint& target, int k) {
    if (root == nullptr) {
        return std::vector<VitalPoint>();
    }
    
    std::vector<std::pair<double, VitalPoint>> neighbors;
    kNearestHelper(root, target, 0, neighbors, k);
    
    // Extract points and sort by distance
    std::sort(neighbors.begin(), neighbors.end());
    
    std::vector<VitalPoint> result;
    for (const auto& pair : neighbors) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<VitalPoint> KDTree::findKNearest(const VitalRecord& targetRecord, int k) {
    VitalPoint target(targetRecord);
    return findKNearest(target, k);
}

void KDTree::rangeSearchHelper(KDNode* node, const VitalPoint& center, double radius,
                              int depth, std::vector<VitalPoint>& results) {
    if (node == nullptr) return;
    
    double dist = node->point.distanceTo(center);
    if (dist <= radius && dist > 0.001) {
        results.push_back(node->point);
    }
    
    int dim = depth % dimensions;
    double diff = center.coordinates[dim] - node->point.coordinates[dim];
    
    if (diff - radius <= 0) {
        rangeSearchHelper(node->left, center, radius, depth + 1, results);
    }
    if (diff + radius >= 0) {
        rangeSearchHelper(node->right, center, radius, depth + 1, results);
    }
}

std::vector<VitalPoint> KDTree::rangeSearch(const VitalPoint& center, double radius) {
    std::vector<VitalPoint> results;
    rangeSearchHelper(root, center, radius, 0, results);
    return results;
}

void KDTree::SimilarPatient::display() const {
    std::cout << "  Patient " << patientID 
              << " | Similarity: " << std::fixed << std::setprecision(2) << similarity
              << " | ";
    vitals.display();
    std::cout << std::endl;
}

std::vector<KDTree::SimilarPatient> KDTree::findSimilarPatients(const VitalRecord& patientVitals, int k) {
    auto nearest = findKNearest(patientVitals, k);
    
    std::vector<SimilarPatient> result;
    for (const auto& point : nearest) {
        SimilarPatient sp;
        sp.patientID = point.patientID;
        sp.similarity = point.distanceTo(VitalPoint(patientVitals));
        sp.vitals = point;
        result.push_back(sp);
    }
    
    return result;
}

std::vector<KDTree::SimilarPatient> KDTree::findSimilarPatients(int patientID,
                                                                const std::vector<VitalRecord>& allRecords,
                                                                int k) {
    // Find latest record for this patient
    VitalRecord targetRecord;
    bool found = false;
    
    for (const auto& record : allRecords) {
        if (record.patientID == patientID) {
            if (!found || record.timestamp > targetRecord.timestamp) {
                targetRecord = record;
                found = true;
            }
        }
    }
    
    if (!found) {
        return std::vector<SimilarPatient>();
    }
    
    return findSimilarPatients(targetRecord, k);
}

void KDTree::display() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              KD-TREE PATIENT CLUSTERING            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\nDimensions: " << dimensions << " | Total Points: " << totalPoints << "\n" << std::endl;
}

int KDTree::countNodes(KDNode* node) const {
    if (node == nullptr) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

void KDTree::saveToDisk() {
    if (dataFilePath.empty()) return;
    
    std::ofstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[KD-TREE] Error: Cannot open file for writing" << std::endl;
        return;
    }
    
    file.write(reinterpret_cast<const char*>(&dimensions), sizeof(dimensions));
    file.write(reinterpret_cast<const char*>(&totalPoints), sizeof(totalPoints));
    
    serializeHelper(root, file);
    
    file.close();
    std::cout << "[KD-TREE] Saved " << totalPoints << " points to " << dataFilePath << std::endl;
}

void KDTree::serializeHelper(KDNode* node, std::ofstream& file) {
    if (node == nullptr) {
        int nullMarker = -1;
        file.write(reinterpret_cast<const char*>(&nullMarker), sizeof(nullMarker));
        return;
    }
    
    int nodeMarker = 1;
    file.write(reinterpret_cast<const char*>(&nodeMarker), sizeof(nodeMarker));
    
    // Write point data
    file.write(reinterpret_cast<const char*>(&node->point.patientID), sizeof(int));
    file.write(reinterpret_cast<const char*>(&node->point.timestamp), sizeof(long));
    
    int coordSize = node->point.coordinates.size();
    file.write(reinterpret_cast<const char*>(&coordSize), sizeof(coordSize));
    for (double coord : node->point.coordinates) {
        file.write(reinterpret_cast<const char*>(&coord), sizeof(double));
    }
    
    file.write(reinterpret_cast<const char*>(&node->splitDimension), sizeof(int));
    
    // Recursively serialize children
    serializeHelper(node->left, file);
    serializeHelper(node->right, file);
}

void KDTree::loadFromDisk() {
    if (dataFilePath.empty()) return;
    
    std::ifstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[KD-TREE] No existing data file found" << std::endl;
        return;
    }
    
    clearTree(root);
    
    file.read(reinterpret_cast<char*>(&dimensions), sizeof(dimensions));
    file.read(reinterpret_cast<char*>(&totalPoints), sizeof(totalPoints));
    
    root = deserializeHelper(file);
    
    file.close();
    std::cout << "[KD-TREE] Loaded " << totalPoints << " points from " << dataFilePath << std::endl;
}

KDNode* KDTree::deserializeHelper(std::ifstream& file) {
    int marker;
    file.read(reinterpret_cast<char*>(&marker), sizeof(marker));
    
    if (marker == -1) {
        return nullptr;
    }
    
    VitalPoint point;
    file.read(reinterpret_cast<char*>(&point.patientID), sizeof(int));
    file.read(reinterpret_cast<char*>(&point.timestamp), sizeof(long));
    
    int coordSize;
    file.read(reinterpret_cast<char*>(&coordSize), sizeof(coordSize));
    point.coordinates.resize(coordSize);
    for (int i = 0; i < coordSize; i++) {
        file.read(reinterpret_cast<char*>(&point.coordinates[i]), sizeof(double));
    }
    
    int splitDim;
    file.read(reinterpret_cast<char*>(&splitDim), sizeof(splitDim));
    
    KDNode* node = new KDNode(point, splitDim);
    node->left = deserializeHelper(file);
    node->right = deserializeHelper(file);
    
    return node;
}

void KDTree::clear() {
    clearTree(root);
    root = nullptr;
    totalPoints = 0;
    std::cout << "[KD-TREE] Cleared" << std::endl;
}