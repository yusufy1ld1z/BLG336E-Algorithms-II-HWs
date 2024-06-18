/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 2
 * Yusuf Yıldız
 * 150210006
*/

#include <iostream>   // For input/output
#include <cmath>      // For distance calculations
#include <vector>     // For vector operations
#include <algorithm>  // For sorting operations
#include <limits>     // For min(), max() functions
#include <fstream>    // To read/write files
#include <chrono>     // For time measurement

//(These libraries are sufficient for this assignment.)

using namespace std;
// Structure to represent a point in 2D space
struct Point {
    double x, y;
};

// Helper function to calculate Manhattan distance between two points
double manhattanDistance(Point p1, Point p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y); // Manhattan distance
}

// Helper function to calculate distance between two points
double distance(Point p1, Point p2) {

    //YOUR CODE HERE
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y- p2.y, 2)); // for Euclidean distance
//    return manhattanDistance(p1, p2); // for Manhattan distance

}

// Helper function to compare points based on x-coordinate, can use this for sorting.
bool compareX(Point p1, Point p2) {

    //YOUR CODE HERE
    return p1.x < p2.x;

}

// Helper function to compare points based on y-coordinate, can use this for sorting.
bool compareY(Point p1, Point p2) {

    //YOUR CODE HERE
    return p1.y < p2.y;

}

// Function to find the closest pair of points using brute force.
pair<Point, Point> bruteForceClosestPair(vector<Point>& points, int start, int end) {

    //YOUR CODE HERE

    // Initialize the minimum distance and the closest pair with the first two points
    double min_distance = numeric_limits<double>::max();
    pair<Point, Point> closest_pair = {points[start], points[start + 1]}; // Cause of the BUG :((((

    // Loop through each pair of points and calculate the distance between them
    for(int i = start; i < end; i++) {
        for(int j = i + 1; j < end; j++) {
            double current_distance = distance(points[i], points[j]);
            // Update the minimum distance and the closest pair if the current distance is smaller
            if (current_distance < min_distance) {
                min_distance = current_distance;
                closest_pair = {points[i], points[j]};
            }
        }
    }

    return closest_pair;
}

// The main objective of the assignment. This is the function to find the closest pair of points recursively using divide and conquer.
pair<Point, Point> closestPair(vector<Point>& points, int start, int end) {

    //YOUR CODE HERE
    if (end - start <= 3) { // Because it is meaningless to divide the points into smaller groups if there are less than 3 points.
        return bruteForceClosestPair(points, start, end);
    }
    // Divide and Conquer
    int mid_ix = (start + end) / 2; // Find the middle index to divide the points into two halves
    Point mid_point = points[mid_ix]; // Find the middle point to use as a reference for the strip

    // Recursively find the closest pair in the left and right halves
    pair<Point, Point> left_pair = closestPair(points, start, mid_ix);
    pair<Point, Point> right_pair = closestPair(points, mid_ix, end);

    // Find the closest distance and the closest pair between the left and right pairs
    double left_closest_distance = distance(left_pair.first, left_pair.second);
    double right_closest_distance = distance(right_pair.first, right_pair.second);

    double min_distance = min(left_closest_distance, right_closest_distance);
    pair<Point, Point> minimum_pair = (left_closest_distance < right_closest_distance) ? left_pair : right_pair;

    // Find the points within the strip that are closer than the minimum distance
    vector<Point> points_on_strip;
    for (int i = start; i < end; i++) {
        // Check if the point is within the strip by comparing the x-coordinate with the middle point
        if (abs(points[i].x - mid_point.x) < min_distance) {
            points_on_strip.push_back(points[i]);
        }
    }

    if (points_on_strip.size() < 2) { // If there are less than 2 points in the strip, there is no pair, so return the minimum pair.
        return minimum_pair;
    }

    // Find the closest pair within the strip using the brute force method
    pair<Point, Point> closest_strip_pair = bruteForceClosestPair(points_on_strip, 0, static_cast<int>(points_on_strip.size()));

    // Check if the closest pair within the strip is closer than the minimum distance
    if (distance(closest_strip_pair.first, closest_strip_pair.second) < min_distance) {
        return closest_strip_pair; // Return the closest pair within the strip if it's closer
    }

    return minimum_pair; // Return the minimum pair if the closest pair within the strip is not closer
}

// Function to remove a pair of points from the point vector. Returns the newly formed vector.
vector<Point> removePairFromVector(vector<Point>& point_vector, pair<Point,Point> point_pair) {

    //YOUR CODE HERE
    vector<Point> new_vector;
    // Loop through each point in the original vector
    for (Point point : point_vector) {
        // Check if the current point is not same with the input point_pair
        if (!((point.x == point_pair.first.x && point.y == point_pair.first.y) ||
              (point.x == point_pair.second.x && point.y == point_pair.second.y))) {
            // If it's not part of the pair, add it to the new vector
            new_vector.push_back(point);
        }
    }

    return new_vector;
}

// This function should wrap up the entire algorithm. It should:
//    1- Find the closest pair
//    2- Add the pair to the "pairs" vector
//    3- Remove the paired result from the map
// at each iteration.
// Then it should set "unconnected" variable as the final, unconnected city (if it exists).
void findClosestPairOrder(vector<Point> points) {
    vector<pair<Point,Point>> pairs; //add your pairs to this vector
    Point unconnected = {-1,-1}; //set this as the unconnected point if it exists, leave as is otherwise.

    //YOUR CODE HERE

    // Sort the points by their x-coordinate using the compareX function, this is necessary for the desired output
    sort(points.begin(), points.end(), compareX);

    // Main loop that iteratively finds the closest pair and removes it
    while (points.size() > 1) {
        // Find the closest pair within the current set of points using the closestPair function
        pair<Point, Point> closest_pair = closestPair(points, 0, static_cast<int>(points.size()));

        // Ensure the first element in the pair has the smaller y-coordinate (or both have the same y and smaller x)
        if (compareY(closest_pair.second, closest_pair.first)) {
            swap(closest_pair.first, closest_pair.second); // Swap if second has smaller y
        } else if (closest_pair.first.y == closest_pair.second.y && compareX(closest_pair.second, closest_pair.first)) {
            swap(closest_pair.first, closest_pair.second); // Swap if y's are equal and second has smaller x
        }

        // Add the identified closest pair to the result vector
        pairs.push_back(closest_pair);

        // Remove the closest pair from the remaining points for the next iteration
        points = removePairFromVector(points, closest_pair);
    }

    // Check if there's an unconnected point (one point left)
    if (!points.empty()) {
        unconnected = points[0]; // Assign the remaining point as the unconnected one
    }

    //before printing, please make sure that within each pair, the city with the smaller y coordinate is printed first...
    //...if the y coordinates are the same, print the city with the smaller x coordinate first.

    //This part prints the output, don't modify.    
    for(size_t i = 0; i < pairs.size(); i++){
        cout << "Pair " << i+1 << ": " << pairs[i].first.x << ", " << pairs[i].first.y << " - " << pairs[i].second.x << ", " << pairs[i].second.y << endl;
    }
    if (unconnected.x != -1){
        cout << "Unconnected " << unconnected.x << ", " << unconnected.y;
    }
}

//Read the coordinates from the file and convert them to a vector. Return the vector as a "vector<Point>"
vector<Point> readCoordinatesFromFile(const string& filename) {
    vector<Point> points; // Create an empty vector to store the Point objects

    //YOUR CODE HERE

    // Open the file for reading
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl; // Inform about error with filename
    }

    // Variables to store individual x and y coordinates from the file
    double pair_x, pair_y;

    // Read coordinates from the file line by line
    while (file >> pair_x >> pair_y) {
        // Create a new Point object with default values (-1, -1) initially
        Point point{-1, -1};

        // Update the point's coordinates with the read values
        point.x = pair_x;
        point.y = pair_y;

        // Add the point to the vector
        points.push_back(point);
    }

    // Close the file after reading
    file.close();

    // Return the vector containing all the Point objects
    return points;
}


// Main function. Ideally, you don't need to modify this.
int main(int argc, char* argv[]) {
    vector<Point> points = readCoordinatesFromFile(argv[1]);
    findClosestPairOrder(points);

    // Below lines are added for measuerement purposes. Uncomment them if you want to measure the execution time.

//    bool approachFlag = true; // True when the user wants to use the divide and conquer approach, false for the brute force.
//    cout << "\nApproach: " << (approachFlag ? "Divide and Conquer" : "Brute Force") << endl;
//    auto start = std::chrono::high_resolution_clock::now();
//    if(approachFlag) {
//        sort(points.begin(), points.end(), compareX);
//        pair<Point, Point> closest_pair = closestPair(points, 0, static_cast<int>(points.size()));
//        cout << "Closest Pair: (" << closest_pair.first.x << ", " << closest_pair.first.y << ") - (" << closest_pair.second.x << ", " << closest_pair.second.y << ")" << endl;
//    }
//    else {
//        pair<Point, Point> closest_pair = bruteForceClosestPair(points, 0, static_cast<int>(points.size()));
//        cout << "Closest Pair: (" << closest_pair.first.x << ", " << closest_pair.first.y << ") - (" << closest_pair.second.x << ", " << closest_pair.second.y << ")" << endl;
//    }
//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//    std::cout << "\nExecution time: " << duration.count() << " nanoseconds\n";

    return 0;
}
