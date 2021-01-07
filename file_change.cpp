#include "tsp.h"

void TSP::read_matrix_from_file(const string &file_name){

    vector<int> arr;
    int element = 0, size = 0;

    fstream in;
    in.open(file_name, ios::in | ios::app);

    if(in.is_open()){

        in >> size;

        arr.reserve(size);

        for (size_t i = 0; i < size; i++){
            for (size_t j = 0; j < size; j++){
                in >> element;
                //cout << element << " ";
                arr.push_back(element);
            }
            //cout<<endl;
            graph.push_back(arr);
            arr.erase(arr.begin(), arr.end());
        }

        in.close();
    } else {

        cout << "cant open";
    }

        clear_v(arr);

}

void TSP::read_points_from_file(const string &file_name){

    vector<pair<int, int>> points;
    vector<int> arr;
    int size = 0, count = 0, weight = 0;
    int number_of_edge, x, y;

    fstream in;
    in.open(file_name, ios::in | ios::app);

    if(in.is_open()){

        in >> size;
        
        arr.reserve(size);

        while(!in.eof()){

            in >> number_of_edge >> x >> y;

            points.push_back(make_pair(x,y));
        }

        in.close();

    } else {

        cout<< "cant open";
    }

    cout<< "G";
    
    for (size_t i = 0; i < size; i++){

    
        while(count < size){

            weight = sqrt(((points[i].first - points[count].first)*(points[i].first - points[count].first)) + ((points[i].second - points[count].second)*(points[i].second - points[count].second)));
            arr.push_back(weight);
            count++;
        }

        graph.push_back(arr);
        arr.erase(arr.begin(), arr.end());
        count = 0;
    }


    // for (size_t i = 0; i < 48; i++){
    //     for (size_t j = 0; j < 48; j++){
            
    //         cout<< matrixp[i][j] << " ";
    //     }
    //     cout<< endl;
    // }

            //cout << graph[5].capacity() << endl;

            points.clear();
            points.shrink_to_fit();

            clear_v(arr);

}

void TSP::print(vector<int> path, const string &file_name){

    ofstream out;
    out.open(file_name, ios::app);

    if(out.is_open()){

        for (size_t i = 0; i < path.size(); i++){

           //cout << path[i] << " ";
            out << path[i] << " ";
        }

        //cout << endl;
        out << endl;

        out.close();
    } else {

        cout << "can't open";
    }
    
}