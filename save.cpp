void nearest_neighbour(int matrix_p[][10], int start_point){

    int number_of_nodes = 9;
    stack<int> temp;
    int* visited = new int[number_of_nodes + 1];
    //vector<int> min_path;
    visited[start_point] = 1;
    temp.push(start_point);
    int element = 0, distance = 0, counter = 0, min_weight = 0, min = INT_MAX;
    bool flag = false;
    cout << start_point << " ";

    while(!temp.empty()){

        element = temp.top();
        counter = 0;
        min = INT_MAX;

        while(counter < number_of_nodes){

            if(matrix_p[element][counter] > 1 && visited[counter] != 1){
                if(min > matrix_p[element][counter]){
                    min = matrix_p[element][counter];
                    min_weight += matrix_p[element][counter];
                    distance = counter;
                    flag = true;
                }
            }
            counter++;
        }
        if(flag == true){
            visited[distance] = 1;
            temp.push(distance);
            //min_path.push_back(distance);
            cout << distance << " ";
            flag = false;
            continue;
        }
        temp.pop();

    }











//#include "twoOpt.h"


// Input: edge 1's v, edge 2's u
// Remove edge 1 and edge 2, reconnect using new path
void TSP::reverse(vector<int> &path, int start, int end, int n)
{
	while (start < end)
	{
		// Start, end is relative value to the start,
		// the index is start|slut % size
		int temp = path[start % n];
		path[start % n] = path[end % n];
		path[end % n] = temp;
		start++;
		end--;
	}
}


bool TSP::is_path_shorter(vector<vector<int>> graph, int v1, int v2, int v3, int v4, int &total_dist)
{
	if ((graph[v1][v3] + graph[v2][v4]) < (graph[v1][v2] + graph[v3][v4]))
	{
		total_dist -= (graph[v1][v2] + graph[v3][v4] - graph[v1][v3] - graph[v2][v4]);
        
		return true;
	}
	return false;
}


// Non-looping version of two-opt heuristic
int TSP::twoOpt(vector<vector<int>> graph, vector<int> &path, int pathLength)
{
	int counter = 0;
	int old_distance = pathLength;
	int n = graph[1].size();
	int v1, v2, u1, u2;

	// Iterate over each edge
	for (int i = 0; i < n; i++){
		for (int j = i + 2; (j + 1) % n != i; j++){

			// Check if new edges would shorten the path length
			// If so, decreases pathLength
			if (is_path_shorter(graph, path[i], path[(i+1) % n], path[j % n], path[(j+1) % n], pathLength))
			{

				// Swap u1--v1 and u2--v2
				reverse(path, i + 1, j, n); // v1, u2

				// if (pathLength - old_distance < 5 && counter == term_cond)
				// 	break;

				// reset i
				if (pathLength > old_distance )
					i = 0;

				old_distance = pathLength;
				counter++;
			}
		}
	}
	return pathLength;
}



int TSP::get_path_length(vector<vector<int>> graph, vector<int> &path, int size){

	int length = 0;
	for (int i = 0; i < size-1; i++){
        
		length += graph[path[i]][path[i+1]];
	}

	length += graph[path[size-1]] [path[0]]; // back home

	return length;
}

