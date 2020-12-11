#include "tsp.h"

vector<vector<int>> TSP::read_matrix_from_file(const string &file_name){

    vector<vector<int>> matrixp;
    vector<int> arr;
    int element = 0, size = 0;
    fstream in;
    in.open(file_name);

    if(in.is_open()){

        in >> size;

        for (size_t i = 0; i < size; i++){
            for (size_t j = 0; j < size; j++){
                in >> element;
                //cout << element << " ";
                arr.push_back(element);
            }
            //cout<<endl;
            matrixp.push_back(arr);
            arr.erase(arr.begin(), arr.end());
        }

        in.close();
    }

    

    return matrixp;
}

vector<vector<int>> TSP::read_point_to_file(const string &file_name){

    vector<pair<int, int>> points;
    vector<vector<int>> matrixp;
    vector<int> arr;
    int size = 0, count = 0, weight = 0;
    int number_of_edge, x, y;

    fstream in;
    in.open(file_name, ios::in);

    in >> size;

    if(in.is_open()){

        while(!in.eof()){

            in >> number_of_edge >> x >> y;

            points.push_back(make_pair(x,y));
        }

        in.close();
    } else {

        cout<< "cant open";
    }

    for (size_t i = 0; i < size; i++){
        
        //cout<< points[i].first << " ";
    }

    cout<< "G";
    

    for (size_t i = 0; i < size; i++){

    
        while(count < size){

            weight = sqrt(((points[i].first - points[count].first)*(points[i].first - points[count].first)) + ((points[i].second - points[count].second)*(points[i].second - points[count].second)));
            arr.push_back(weight);
            count++;
        }

        matrixp.push_back(arr);
        arr.erase(arr.begin(), arr.end());
        count = 0;
    }


    // for (size_t i = 0; i < 48; i++){
    //     for (size_t j = 0; j < 48; j++){
            
    //         cout<< matrixp[i][j] << " ";
    //     }
    //     cout<< endl;
    // }
    

    return matrixp;

}

void TSP::print(const vector<int> path, const string &file_name){

    ofstream out;
    out.open(file_name, ios::out);

    if(out.is_open()){

        for (size_t i = 0; i < path.size(); i++){

           cout << path[i] << " ";
            out << path[i] << " ";
        }

        cout << endl;
        out << endl;

        out.close();
    }

    
}