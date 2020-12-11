#include "tsp.h"



void TSP::reverse(vector<int> &path, int start, int end ){

	for (size_t i = start, j = end; i < j; i++,j--){

		int temp = path[i];
		path[i] = path[j];
		path[j] = temp;

	}

}

int get_path_length(vector<vector<int>> graph, vector<int> &path, int size, int start_point){

	int length = 0;

	for (int i = 0; i < size-1; i++)
	{
		length += graph[path[i]][path[i+1]];
	}

	length += graph[path[size-1]] [path[start_point]]; // back home

	return length;
}

// Non-looping version of two-opt heuristic
int TSP::two_opt(vector<vector<int>> graph, vector<int> &path, int pathLength, const string &file_name){

	int new_leng = 0, n = graph[1].size(), start = path.back();

		for (int i = 1; i < n - 1; i++){
			for (int j = i + 1; j < n ; j++){
				
				reverse(path, i, j);
				if(get_path_length( graph, path, n, start ) < pathLength){

					pathLength = get_path_length( graph, path, n, start );

				} else {

					reverse(path, i, j);

				}

			}

		}


		print(path, "perform.txt");


	PROCESS_MEMORY_COUNTERS_EX pmc;
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

	fstream out;
	out.open(file_name);

	if(out.is_open()){

		out << "-------------------------------------------" << endl;
		out << (virtualMemUsedByMe / 1048576.0 ) << "mb virtual memory is used by nearest" << endl;
		out << "-------------------------------------------" << endl;
		out << (physMemUsedByMe / 1048576.0 ) << "mb physical memory i used by nearest" << endl;
		out << "-------------------------------------------" << endl;

		out.close();
	}


	return pathLength;

}