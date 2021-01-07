#ifndef __TSP__
#define __TSP__
#include <iostream>
#include <stack>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <climits>
#include <utility>
#include <cmath>
#include "windows.h"
#include "psapi.h"

using namespace std;
using namespace std::chrono;

class TSP{

private:

    vector<vector<int>> graph;
    vector<int> min_path;

    int get_graph_size()const { return graph[1].size(); }

public:

    TSP(){

        graph = vector<vector<int>>(0, vector<int>(0));
        min_path.resize(0);
    }

    TSP(int size){

        graph = vector<vector<int>>(size, vector<int>(size));
        min_path.resize(size);

    }

    void read_matrix_from_file(const string &file_name);
    void read_points_from_file(const string &file_name);
    void print(vector<int> path, const string &file_name);

    int nearest_neighbour(int start_point);

    int two_opt(int path_length);
    int get_path_length(vector<int> &path, int size, int start_point);
    //int get_path_length(vector<int> &path, int size, int start_point);
    void reverse(vector<int> &path, int start, int end);

    void clearm(){
        
        graph.clear();
        graph.shrink_to_fit();
        min_path.clear();
        min_path.shrink_to_fit();



    }

    void clear_v(vector<int> &vec){

        vec.clear();
        vec.shrink_to_fit();

    }

    ~TSP(){
        
        clearm();

    }

};

#endif