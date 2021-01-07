#include "tsp.h"

void TSP::reverse(vector<int> &path, int start, int end ){

	for (size_t i = start, j = end; i < j; i++,j--){

		if(graph[j][i] > 0){
			int temp = path[i ];
			path[i ] = path[j ];
			path[j ] = temp;
		}


	}



}

int TSP::get_path_length(vector<int> &path, int size, int start_point){

	int length = 0;

	for (int i = 0; i < size-1; i++)
	{
		length += graph[path[i]][path[i+1]];
	}

	length += graph[path[size-1]][path[start_point]]; // back home

	return length;
}



int TSP::two_opt(int path_length){

	if(min_path.size() == 0){

		for (size_t i = 0; i < get_graph_size(); i++){
			
			min_path.push_back(i);

		}

		min_path.push_back(0);
			
	}

	int new_leng = 0, size = get_graph_size(), start_point = min_path.back();

		for (int i = 1; i < size - 1; i++){
			for (int j = i + 1; j < size ; j++){
			


				if(graph[min_path[i - 1]][min_path[ j ]] + graph[min_path[ i ]][min_path[j + 1]] <
			       graph[min_path[i - 1]][min_path[ i ]] + graph[min_path[ j ]][min_path[j + 1]]){

					   reverse( min_path, i , j );

				}

				   

			}

		}


	print(min_path, "perform_opt.txt");

	

	return get_path_length(min_path, size, start_point);

}
