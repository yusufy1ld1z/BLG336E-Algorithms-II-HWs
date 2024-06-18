/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 3
 * Yusuf YILDIZ
 * 150210006
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

/* YOU CAN INCLUDE ANY OTHER NECESSARY LIBRARIES */
#include <set>
#include "cmath"
#include "iomanip"

/* YOU CAN DEFINE EXTRA FUNCTIONS AND MODIFY THE CODE TEMPLATE */

/* YOU HAVE TO CONSIDER THE OUTPUT FORMAT TO TEST YOUR CODE WITH CALICO */

using namespace std;

// Define the structures
/*
    * Schedule: Represents a schedule with floor name, room number, start time, end time, and priority.
    * TimeIntervals: Represents a time interval with floor name, room number, start time, and end time.
    * Priority: Represents a priority with floor name, room number, and priority value.
    * Item: Represents an item with name, price, and value.
    
*/
struct Schedule{

    // YOUR CODE HERE  
    string floorName;
    string roomNo;
    string startTime;
    string endTime;
    int priority;
};

struct TimeIntervals {

    // YOUR CODE HERE
    string floorName;
    string roomNo;
    string startTime;
    string endTime;

    TimeIntervals() {} // Default constructor

    TimeIntervals(const Schedule& schedule) { // Convert Schedule to TimeIntervals
        floorName = schedule.floorName;
        roomNo = schedule.roomNo;
        startTime = schedule.startTime;
        endTime = schedule.endTime;
    }
    void print() const {
        cout << floorName << '\t' << roomNo << '\t' << startTime << '\t' << endTime << endl;
    }
};

struct Priority {

    // YOUR CODE HERE
    string floorName;
    string roomNo;
    int priority;
};

struct Item {

    // YOUR CODE HERE
    string name;
    int price;
    double value;
};

// I/O Functions
/**
 * Read the time intervals from the given file
 * @param filename: The name of the file to read
 * @return A vector of TimeIntervals
 */ 
vector<TimeIntervals> readTimeIntervals(const string& filename) {
    vector<TimeIntervals> intervals;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return intervals;
    }

    string line;
    getline(file, line); // skip the first line
    while (getline(file, line)) {
        istringstream iss(line);
        TimeIntervals interval;
        iss >> interval.floorName >> interval.roomNo >> interval.startTime >> interval.endTime;
        intervals.push_back(interval);
    }

    file.close();
    return intervals;
}

/**
 * Read the priorities from the given file
 * @param filename: The name of the file to read
 * @return A vector of Priorities
 */
vector<Priority> readPriorities(const string& filename) {
    vector<Priority> priorities;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return priorities;
    }

    string line;
    getline(file, line); // skip the first line
    while (getline(file, line)) {
        istringstream iss(line);
        Priority priority;
        iss >> priority.floorName >> priority.roomNo >> priority.priority;
        priorities.push_back(priority);
    }

    file.close();
    return priorities;
}

/**
 * Read the items from the given file
 * @param filename: The name of the file to read
 * @return A vector of Items
 */
vector<Item> readItems(const string& filename) {
    vector<Item> items;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return items;
    }

    string line;
    getline(file, line); // skip the first line
    while (getline(file, line)) {
        istringstream iss(line);
        Item item;
        iss >> item.name >> item.price >> item.value;
        items.push_back(item);
    }

    file.close();
    return items;
}

//* START: WEIGHTED INTERVAL SCHEDULING (for each floor) *//

// WIS and Knapsack Algorithms Implementations

/**
 * @brief Compare two schedules by their end times   
 * 
 * @param a Input schedule a
 * @param b Input schedule b
 * @return true of false
 */
bool compareByEndTime(const Schedule &a, const Schedule &b) {
    return a.endTime < b.endTime;
}

/**
 * @brief Computes the p(i) array for the Weighted Interval Scheduling problem.
 *
 * This function takes a vector of `Schedule` objects representing the available jobs and computes the p(i) array 
 * used in the Weighted Interval Scheduling (WIS) algorithm. The p(i) array stores the index of the latest non-conflicting job 
 * that can be scheduled before job i.
 *
 * @param jobs A vector containing the `Schedule` objects representing the available jobs.
 * @return A vector containing the p(i) values for each job in the `jobs` vector.
 *
 * @complexity
 *  - Time complexity: O(n^2)
 *  - Space complexity: O(n)
 */
vector<int> computeP(const vector<Schedule>& jobs) {
    vector<int> p(jobs.size());
    for (size_t i = 0; i < jobs.size(); ++i) {
        p[i] = -1;
        for (int j = i - 1; j >= 0; --j) {
            if (jobs[j].endTime <= jobs[i].startTime) {
                p[i] = j;
                break;
            }
        }
    }
    return p;
}

// Memoization table M
vector<int> M;
// Memoization table for intervals corresponding to M values
vector<vector<TimeIntervals>> M_intervals;

/**
 * @brief Computes the maximum value achievable using memoization for the Weighted Interval Scheduling problem.
 *
 * @param jobs A vector containing the `Schedule` objects representing the available jobs.
 * @param j The index of the current job in the `jobs` vector.
 * @return The maximum achievable value by scheduling compatible jobs.
 *
 * @complexity
 *  - Time complexity: close to O(n) (average case) due to memoization, can be O(n^2) in the worst case.
 *  - Space complexity: O(n) for memoization tables.
 *
 * @note
 * This function relies on the pre-computed `p` array (returned by `computeP`) to identify compatible jobs.
 * The `M_intervals` table stores the optimal set of schedules for each subproblem, allowing retrieval of the 
 * specific jobs included in the solution.
 */
int WISCompute(const vector<Schedule>& jobs, int j) {
    vector<int> p = computeP(jobs);
    if (M[j] == -1) {
        if (j == 0) {
            M[j] = 0; // Base case
        } else {
            int include = jobs[j - 1].priority + WISCompute(jobs, p[j - 1] + 1);
            int exclude = WISCompute(jobs, j - 1);
            if (include > exclude) {
                // Update M_intervals if including current job yields a better result
                M_intervals[j] = M_intervals[p[j - 1] + 1];
                M_intervals[j].push_back(jobs[j - 1]);
            } else {
                // If excluding current job is better, just copy the previous intervals
                M_intervals[j] = M_intervals[j - 1];
            }
            M[j] = max(include, exclude);
        }
    }
    return M[j];
}

/**
 * @brief Solves the Weighted Interval Scheduling problem using dynamic programming.
 *
 * This function implements the Weighted Interval Scheduling (WIS) algorithm 
 * to find the maximum achievable value by scheduling compatible jobs. It takes a 
 * vector of `Schedule` objects and returns the maximum value.
 *
 * @param jobs A vector containing the `Schedule` objects representing the available jobs.
 * @return The maximum achievable value by scheduling compatible jobs.
 *
 * @complexity
 *  - Time complexity: O(n log n) (average case, from sorting), O(n^2) (worst case, from recursive calls) due to sorting and memoization.
 *  - Space complexity: O(n) for memoization tables.
 *
 * @note
 * This function utilizes the `computeP` function to pre-compute compatibility information 
 * and the `WISCompute` function for the recursive solution with memoization.
 */
int weighted_interval_scheduling(vector<Schedule>& jobs) {
    sort(jobs.begin(), jobs.end(), compareByEndTime);
    vector<int> p = computeP(jobs);
    M.assign(jobs.size() + 1, -1); // Initialize memoization table M with -1
    M_intervals.resize(jobs.size() + 1); // Resize M_intervals
    return WISCompute(jobs, jobs.size());
}

// *** END ***


//* START: KNAPSACK (for items) *//

/**
 * @brief Solves the 0/1 Knapsack Problem using dynamic programming.
 *
 * This function implements the 0/1 Knapsack problem using dynamic programming 
 * to find the most valuable subset of items that can be purchased with a given budget.
 * It takes a vector of `Item` objects and a budget as input and returns a vector 
 * containing the selected items.
 *
 * @param items A vector containing the `Item` objects representing available items.
 * @param budget The maximum budget available for purchasing items.
 * @return A vector containing the most valuable items that can be purchased within the budget.
 *
 * @complexity
 *  - Time complexity: O(n * budget) where n is the number of items and budget is the 
 *                     maximum budget.
 *  - Space complexity: O(n * budget) for the dynamic programming table.
 *
 * @note
 * This function utilizes a 2D table (dp) to store the maximum value achievable 
 * for different budget constraints and item combinations. The function then backtracks 
 * to identify the actual items included in the optimal solution.
 */
vector<Item> knapsack(const vector<Item>& items, int budget) {

    // YOUR CODE HERE
    int n = items.size();
    
    // Create a 2D table to store the maximum value that can be obtained with each budget
    vector<vector<double>> dp(n + 1, vector<double>(budget + 1, 0.0));

    // Fill the table iteratively
    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j <= budget; ++j) {
            // If the current item can be included in the knapsack
            if (items[i - 1].price <= j) {
                // Choose the maximum value between including and excluding the current item
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - items[i - 1].price] + items[i - 1].value);
            } else {
                // If the current item cannot be included, copy the value from the previous row
                dp[i][j] = dp[i - 1][j];
            }
        }
    }

    // Retrieve the selected items
    vector<Item> selected_items;
    int j = budget;
    for (int i = n; i > 0 && j > 0; --i) {
        // If the value in the current cell is different from the value in the cell above it,
        // it means the item was included in the knapsack
        if (dp[i][j] != dp[i - 1][j]) {
            selected_items.push_back(items[i - 1]);
            j -= items[i - 1].price; // Update the remaining budget
        }
    }

    return selected_items;
}

// *** END ***


int main(int argc, char *argv[])
{
    string case_no = argv[1];
    string case_name = "case_" + case_no;
    string path = "./inputs/" + case_name;

    int total_budget = 200000; 

    // YOUR CODE HERE

    // Read the input files
    vector<TimeIntervals> timeIntervals = readTimeIntervals(path + "/room_time_intervals.txt");
    vector<Priority> priorities = readPriorities(path + "/priority.txt");
    vector<Item> items = readItems(path + "/items.txt");

    // Extract unique floor names
    set<string> uniqueFloors;
    for (const auto& interval : timeIntervals) {
        uniqueFloors.insert(interval.floorName);
    }

    cout << "Best Schedule for Each Floor" << endl;
    // Apply weighted interval scheduling specifically for each floor
    for (const auto& floor : uniqueFloors) {
        vector<Schedule> floorSchedules;
        // Filter time slots and priorities by floor
        for (const auto& interval : timeIntervals) {
            if (interval.floorName == floor) {
                Schedule schedule;
                schedule.floorName = interval.floorName;
                schedule.roomNo = interval.roomNo;
                schedule.startTime = interval.startTime;
                schedule.endTime = interval.endTime;
                // Find and add the priority value
                for (const auto& priority : priorities) {
                    if (priority.floorName == interval.floorName && priority.roomNo == interval.roomNo) {
                        schedule.priority = priority.priority;
                        break;
                    }
                }
                floorSchedules.push_back(schedule);
            }
        }
        // Apply weighted interval scheduling(WIS) 
        int max_value = weighted_interval_scheduling(floorSchedules);
        cout << floor << " --> Priority Gain: " << max_value << endl;
        for (const auto& interval : M_intervals[M.size() - 1]) {
            interval.print();
        }
    }
    
    // Find the most valuable items that can be purchased with the given budget
    vector<Item> selected_items = knapsack(items, total_budget);

    // Calculate the total value of selected items
    double total_value = 0.0;
    for (const auto& item : selected_items) {
        total_value += item.value;
    }
    total_value = round(total_value * 10.0) / 10.0; // Round to one decimal place

    // Print the selected items and total value
    cout << "Best Use of Budget:" << endl;
    cout << "Total Value --> " << fixed << setprecision(1) << total_value << endl;

    // Print the selected items
    cout << "Selected items:" << endl;
    for (const auto& item : selected_items) {
        cout  << item.name << endl;
    }

    return 0;
}