/**
 * BLG 336E - Analysis of Algorithms II
 * Assignment 1
 * Yusuf YILDIZ
 * 150210006
*/

/* * * * * * * * * * * * * * * * * *
 * DO NOT MODIFY UNLESS REQUESTED  *
* * * * * * * * * * * * * * * * * */

#include <iostream>         
#include <vector>
#include <fstream>
#include <sstream>       
#include <stack>
#include <queue>
#include <chrono>       // For time measurement

/* INCLUDE ANY OTHER NECESSARY LIBRARIES HERE */
/* START YOUR CODE HERE */

#include <algorithm> // For sorting

/* END YOUR CODE HERE */

using namespace std;
using namespace std::chrono;

/* USE HERE IF YOU NEED TO DEFINE EXTRA FUNCTIONS */
/* START YOUR CODE HERE */

vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // NORTH, SOUTH, WEST, EAST
vector<vector<bool>> visited; // To keep track of visited cells

/**
 * It is used for checking whether the given value is in the map or not. Since it is not declared that the resource type is between 1 and 9,
 * this function is implemented to check whether the given value is in the map or not. Thus, the program can be used for any resource type.
 * 
 * @param map The map represented as a 2D vector of integers.
 * @param value The resource value to search for in the map.
 * @return true if the value is in the map, false otherwise.
 */
bool containsValue(const vector<vector<int>>& map, int value) {
    for (const auto& row : map) {
        for (int cell : row) {
            if (cell == value) {
                return true;  // Value found, return true
            }
        }
    }
    return false;  // Value not found in the vector
}

/**
 * It is used for comparing the colonies. It is implemented for sorting the colonies in descending order by their sizes and, 
 * if happens, when their sizes are equal, in ascending order by their resource types. 
 * 
 * @param colony1 The first colony to compare.
 * @param colony2 The second colony to compare.
 * @return true if the first colony is smaller than the second colony, false otherwise, and considers the resource type if the sizes are equal.
 */
bool compareColonies(const pair<int, int>& colony1, const pair<int, int>& colony2) {
    if (colony1.first != colony2.first) {
        // Sort by colony size in descending order
        return colony1.first > colony2.first;
    } else {
        // If colony sizes are equal, sort by resource type in ascending order
        return colony1.second < colony2.second;
    }
}

/* END YOUR CODE HERE */

/**
 * Perform Depth-First Search (DFS) traversal on a map.
 * 
 * @param map The map represented as a 2D vector of integers.
 * @param row The starting row index for the DFS traversal.
 * @param col The starting column index for the DFS traversal.
 * @param resource The resource value to search for during the traversal.
 * @return The size of the colony found during the DFS traversal.
 */
int dfs(vector<vector<int>>& map, int row, int col, int resource) {

    /* START YOUR CODE HERE */

    int colony_size = 0;
    stack<pair<int, int>> location_stack; // stack to keep the locations
    location_stack.push({row, col}); // push the starting location

    while (!location_stack.empty()){
        pair<int, int> cur_location = location_stack.top(); // get the rightmost element
        location_stack.pop();

        int cur_row = cur_location.first;
        int cur_col = cur_location.second;

        if (visited[cur_row][cur_col] || map[cur_row][cur_col] != resource) continue; // if the cell is visited or not the desired resource, continue
                                                                                      // otherwise, mark the cell as visited and increase the colony size
        visited[cur_row][cur_col] = true; 
        colony_size++;

        for (auto direction : directions) {
            int new_loc_first = (cur_row + direction.first + map.size()) % map.size();          // since the map is circular, the new location is calculated                                                                                                
            int new_loc_second = (cur_col + direction.second + map[0].size()) % map[0].size();  // by taking the modulo of the map size in 2 dimensions

            if (map[new_loc_first][new_loc_second] == resource && !visited[new_loc_first][new_loc_second]) { 
                location_stack.push({new_loc_first, new_loc_second});                           // if the new location is the desired resource and not visited, push it to the stack
            }
        }
    }
    return colony_size;

    /* END YOUR CODE HERE */

}

/**
 * Perform Breadth-First Search (BFS) traversal on a map.
 * 
 * @param map The map represented as a 2D vector of integers.
 * @param row The starting row index for the BFS traversal.
 * @param col The starting column index for the BFS traversal.
 * @param resource The resource value to search for during the traversal.
 * @return The size of the colony found during the BFS traversal.
 */
int bfs(vector<vector<int>>& map, int row, int col, int resource) {
    
    /* START YOUR CODE HERE */

    int colony_size = 0;
    queue<pair<int, int>> location_queue; // queue to keep the locations
    location_queue.push({row, col}); //push the starting location

    while (!location_queue.empty()){
        pair<int, int> cur_location = location_queue.front(); // get the leftmost element
        location_queue.pop(); 

        int cur_row = cur_location.first;
        int cur_col = cur_location.second;

        if (visited[cur_row][cur_col] || map[cur_row][cur_col] != resource) continue; // if the cell is visited or not the desired resource, continue
                                                                                      // otherwise, mark the cell as visited and increase the colony size
        visited[cur_row][cur_col] = true;
        colony_size++;

        for (auto direction : directions) {
            int new_loc_first = (cur_row + direction.first + map.size()) % map.size();          // since the map is circular, the new location is calculated  
            int new_loc_second = (cur_col + direction.second + map[0].size()) % map[0].size();  // by taking the modulo of the map size in 2 dimensions

            if (map[new_loc_first][new_loc_second] == resource && !visited[new_loc_first][new_loc_second]){
                location_queue.push({new_loc_first, new_loc_second});                           // if the new location is the desired resource and not visited, push it to the queue
            }
        }

    }
    return colony_size;

    /* END YOUR CODE HERE */

}

/**
 * Finds the top-k largest colonies in a given map.
 *
 * @param map The map represented as a 2D vector of integers.
 * @param useDFS A boolean flag indicating whether to use Depth-First Search (DFS) or Breadth-First Search (BFS) algorithm.
 * @param k The number of top colonies to find.
 * @return A vector of pairs representing the size and resource type of the top-k largest colonies.
 */
vector<pair<int, int>> top_k_largest_colonies(vector<vector<int>>& map, bool useDFS, unsigned int k) {

    auto start = high_resolution_clock::now();      // Start measuring time
    
    /* START YOUR CODE HERE */

    bool emptyFlag = map.empty() || map[0].empty(); // check if the map is empty or not, flag is used for not to lose clock output
    vector<pair<int, int>> topColonies; // vector to keep the top colonies, declared here for to escape from warnings

    if (!emptyFlag) { // if the map is not empty
        vector<pair<int, int>> colonies;

        visited.assign(map.size(), vector<bool>(map[0].size(), false)); // initialize the global visited vector with the size of the map, and all false values

        for (int resource = 1; containsValue(map, resource); resource++) { // check the possible resource types until it is not in the map
            for (size_t i = 0; i < map.size(); ++i) {
                for (size_t j = 0; j < map[0].size(); ++j) {
                    int colonySize = (useDFS) ? dfs(map, i, j, resource) : bfs(map, i, j, resource); // call the DFS or BFS function according to the flag
                    if (colonySize > 0) {
                        colonies.push_back({colonySize, resource}); // if the colony size is greater than 0, push the colony to the colonies vector to be sorted later on
                    }
                }
            }
        }

        sort(colonies.begin(), colonies.end(), compareColonies); // sort the found colonies in descending order with the user-defined compare function
        topColonies.assign(colonies.begin(), colonies.begin() + min(k, static_cast<unsigned int>(colonies.size()))); // get the top k colonies and assign them to the topColonies vector
    }

    /* END YOUR CODE HERE */

    auto stop = high_resolution_clock::now();                       // Stop measuring time
    auto duration = duration_cast<nanoseconds>(stop - start);       // Calculate the duration
    cout << "Time taken: " << duration.count() << " nanoseconds" << endl;

    /* START YOUR CODE HERE */

    if (emptyFlag) { // if the map is empty, return an empty vector
        return {};
    } else {
        return topColonies; // else, return the top colonies, it is set inside of the if block to escape from warnings
    }

    /* END YOUR CODE HERE */

}

// Main function
int main(int argc, char* argv[]) {

    // Check the command line arguments
    if (argc != 4) {

        // It should be: <1 or 0> <k> <filename.txt>
        cerr << "Usage: " << argv[0] << " <1 or 0> <k> <filename.txt>" << endl;
        return 1;
    }

    // Get the arguments
    bool useDFS = (atoi(argv[1]) == 1);
    int k = atoi(argv[2]);

    // Read the map from the text file
    string filename = argv[3];
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    vector<vector<int>> sector_map;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        vector<int> row;
        int num;
        while (iss >> num) {
            row.push_back(num);
        }
        sector_map.push_back(row);
    }
    file.close();   

    // Find the largest colony
    vector<pair<int, int>> result = top_k_largest_colonies(sector_map, useDFS, k);

    // Print the result
    cout << "Algorithm: " << (useDFS ? "DFS" : "BFS") << endl;
    cout << "Map: " << filename << endl;

    // If no colonies are found
    if (result.empty()) {
        cout << "No colonies found." << endl;
        return 0;
    }

    // Print the top-k colonies
    for (size_t i = 0; i < result.size(); ++i) {
        cout << "Colony " << i + 1 << ": Size = " << result[i].first << ", Resource Type = " << result[i].second << endl;
    }

    return 0;
}
