#include "nearest_neighbour.cpp"
#include "repeated_nearest_neighbour.cpp"
#include "two_opt.cpp"
#include "file_change.cpp"

int main(){

    TSP tsp;

    string answer = "", answer1 = "";
    fstream result;
    int start_point = 0, result_opt = 0;

    cout << "hello user" << endl;
    
    while(answer != "end"){

        cout << "name of file ";

        cin >> answer;

        if(answer == "end") break;

        cout << "type of file matrix/points ";

        cin >> answer1;

        cout << "from what point want to start ";

        cin >> start_point;

        if( answer1 == "matrix"){

            result.open("perform.txt", ios::out | ios::app);

            result << "result of " << answer << endl;

            tsp.read_matrix_from_file(answer);

            int length = tsp.nearest_neighbour(start_point);
            
            result.close();

                    result.open("perform_opt.txt", ios::out | ios::app);

                    result << "result of " << answer << endl;

                    if(answer[answer.length()-3] == '.'){

                        result_opt = tsp.two_opt_sym(length);

                    } else {

                        result_opt = tsp.two_opt_nsym(length);

                    }

                    result << endl << "result from 2 opt algo " << result_opt << endl;

                    result.close();

            cout << "min weigth path is " << min(length, result_opt) << " " << ( (length <= result_opt) ? "path can be seen in perform.txt" : "path can be seen in perform_opt.txt" ) << endl ;
        
            result_opt = 0;
        
        } else if( answer1 == "points"){

            result.open("perform.txt", ios::out | ios::app);

            result << "result of " << answer << endl;

            tsp.read_points_from_file(answer);

            int length = tsp.nearest_neighbour(start_point);

            result.close();

                        result.open("perform_opt.txt", ios::out | ios::app);

                        result << "result of " << answer << endl;

                        auto start = high_resolution_clock::now();

                    if(answer[answer.length()-3] == '.'){

                        result_opt = tsp.two_opt_sym(length);

                    } else {

                        result_opt = tsp.two_opt_nsym(length);

                    }

                        result << endl << "result from 2 opt algo " << result_opt << endl;

                        result.close();

            cout << "min weigth path is " << min(length, result_opt) << " " << ( (length <= result_opt) ? "path can be seen in perform.txt" : "path can be seen in perform_opt.txt" ) << endl ;

            result_opt = 0;
        
        } else {

            cout << "invalid type of file" << endl;
        }



        tsp.clearm();


    }


}
