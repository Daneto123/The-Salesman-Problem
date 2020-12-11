#ifndef __TSP__
#define __TSP__
#include <iostream>
#include <cassert>
#include <stack>
#include <vector>
#include <climits>
#include <chrono>
#include <algorithm>
#include "windows.h"
#include "psapi.h"
#include <fstream>
#include <cmath>
#include <utility>

using namespace std;
using namespace std::chrono;

class TSP{

    public:

    vector<vector<int>> read_matrix_from_file(const string &file_name);
    void print(const vector<int> path, const string &file_name);
    vector<vector<int>> read_point_to_file(const string &file_name);

    vector<int> nearest_neighbour(const vector<vector<int>> &matrix_p, int start_point,  const string &file_name);
    vector<int> repeated_nearest_neighbour(const vector<vector<int>> &matrix_p, const string &file_name);
    // void find_min_route(const vector<vector<int>> &tsp, const string &file_name);

    int two_opt(vector<vector<int>>, vector<int> &path, int pathLength, const string &file_name);
    bool is_path_shorter(vector<vector<int>> graph, int v1, int v2, int v3, int v4, int &total_dist);
    void reverse(vector<int> &path, int start, int end );


};

#endif