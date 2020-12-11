#include "tsp.h"



vector<int> TSP::nearest_neighbour(const vector<vector<int>> &matrix_p, int start_point, const string &file_name){

    auto start = high_resolution_clock::now(); 

    int number_of_nodes =  matrix_p[1].size(), element = 0, distance = 0, counter = 0, min_weight = 0, min = INT_MAX;
    bool flag = false;
    stack<int> temp;
    int* visited = new int[number_of_nodes];
    vector<int> min_path;

    visited[start_point] = 1;
    temp.push(start_point);
    min_path.push_back(start_point);

    while(!temp.empty()){

        element = temp.top();temp.pop();
        counter = 0;
        min = INT_MAX;

        while(counter < number_of_nodes){

            if(matrix_p[element][counter] > 1 && visited[counter] != 1 && matrix_p[element][counter] != 9999){

                if(min > matrix_p[element][counter]){

                    min = matrix_p[element][counter];
                    distance = counter; // пази следващият най-малък
                    flag = true;

                }

            }
            counter++;
        }

        if(flag == true){

            visited[distance] = 1;
            temp.push(distance);
            min_path.push_back(distance);
            min_weight += matrix_p[element][distance];
            flag = false;
            continue;

        }


    }

    min_weight += matrix_p[distance][start_point];

    min_path.push_back(start_point);

    min_path.push_back(min_weight);

    //cout << min_weight << endl;

    PROCESS_MEMORY_COUNTERS_EX pmc;
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

    fstream out;
    out.open(file_name);


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start); 

    if(out.is_open()){

        out << "-------------------------------------------" << endl;
        out << (virtualMemUsedByMe / 1048576.0 ) << "mb virtual memory is used by nearest" << endl;
        out << "-------------------------------------------" << endl;
        out << (physMemUsedByMe / 1048576.0 ) << "mb physical memory i used by nearest" << endl;
        out << "-------------------------------------------" << endl;

        out << "Time taken by function: " << duration.count() << " microseconds. for nearest" << endl; 

        out.close();

    }

    delete[] visited;

    return min_path;

}

vector<int> TSP::repeated_nearest_neighbour(const vector<vector<int>> &matrix_p, const string &file_name){

    auto start = high_resolution_clock::now();

    vector<int> m_path;
    int min = INT_MAX, size = matrix_p[1].size();
    vector<int> save;

    for (size_t i = 0; i < size; i++){

        m_path = nearest_neighbour(matrix_p, i, file_name);

        if(m_path.back() < min && m_path.size() == (size + 2)){
            save = m_path;
            min = m_path.back();
        }
        print(m_path, file_name);
            
    }

    PROCESS_MEMORY_COUNTERS_EX pmc;
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    fstream out;
    out.open(file_name);

    if(out.is_open()){

        out << "-------------------------------------------" << endl;
        out << (virtualMemUsedByMe / 1048576.0 ) << "mb virtual memory is used by repeated" << endl;
        out << "-------------------------------------------" << endl;
        out << (physMemUsedByMe / 1048576.0 ) << "mb physical memory i used by repeated" << endl;
        out << "-------------------------------------------" << endl;
        out << "Time taken by function: " << duration.count() << " microseconds. by repeated" << endl;
        out << "-------------------------------------------" << endl;


        out.close();
   }


    return save;
    
}
