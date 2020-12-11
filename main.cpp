#include "nearest_neignbour.cpp"
//#include "greedy.cpp"
#include "file.cpp"
#include "p.cpp"

int main(){

    TSP the_salesman_problem;

    int matrixp[10][10] =  {{000, 374, 200, 223, 108, 178, 252, 285, 240, 356},
                            {374, 000, 255, 166, 433, 199, 135, 95, 136,  17},
                            {200, 255, 000, 128, 277, 128, 180, 160, 131, 247},
                            {223, 166, 128, 000, 430, 47,  52,  84,  40, 155},
                            {108, 433, 277, 430, 000, 453, 478, 344, 389, 423},
                            {178, 199, 128, 47, 453, 000,  91,  110,  64, 181},
                            {252, 135, 180, 52, 478,  91, 000,  114,  83, 117},
                            {285, 95,  160, 84, 344, 110, 114, 000,   47,  78},
                            {240, 136, 131, 40, 389, 64, 83, 47, 000, 118},
                            {356, 17, 247, 155, 423, 181, 117, 78, 118, 000}};



        // 108 + 277 + 128 + 40 + 47 + 78 + 17 + 135 + 91 + 178
        
        // auto start = high_resolution_clock::now();
        //the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_from_file("br17.atsp"), 0, "perform_nn.txt"), "perform_nn.txt");
        //the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_point_to_file("arr48.tsp"), 0, "perform_nn.txt"), "perform_nn.txt");
        //the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_from_file("ftv64.atsp"), 0, "perform_nn.txt"), "perform_nn.txt");
        //the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_from_file("ftv170.atsp"), 0, "perform_nn.txt"), "perform_nn.txt");
        //the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_from_file("rbg403.atsp"), 0, "perform_nn.txt"), "perform_nn.txt");

        
        // the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_point_to_file("arr48.tsp"), 10, "perform_nn.txt") , "perform.txt");
        // vector<int> saves = the_salesman_problem.nearest_neighbour(the_salesman_problem.read_point_to_file("arr48.tsp"), 10, "perform_nn.txt");
        // int lenght = saves.back();
        // cout << lenght << endl;
        // saves.pop_back();

        // int result = the_salesman_problem.two_opt(the_salesman_problem.read_point_to_file("arr48.tsp"), saves, lenght, "perform.txt");
        // cout << result;

        // the_salesman_problem.print(the_salesman_problem.repeated_nearest_neighbour(the_salesman_problem.read_from_file("ftv170.astp"), "perform_rnn.txt"), "perform_rnn.txt");

        // auto stop = high_resolution_clock::now();
        // auto duration = duration_cast<microseconds>(stop - start); 

        // cout << "Time taken by function: " << duration.count() << " microseconds. for g" << endl;

        // cout << "-------------------------------------------" << endl;
        // cout << "-------------------------------------------" << endl;
        //the_salesman_problem.print(the_salesman_problem.repeated_nearest_neighbour(the_salesman_problem.read_from_file("br17.atsp"), "perform_rnn.txt"), "perform_rnn.txt");
        //cout << "-------------------------------------------" << endl;
        //cout << "-------------------------------------------" << endl;
        //the_salesman_problem.find_min_route(the_salesman_problem.read_from_file("br17.atsp"), "performance.txt");
        // the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_point_to_file("arr48.tsp"), 0, "perform_rnn.txt"), "perform_rnn.txt");
        //the_salesman_problem.find_min_route(the_salesman_problem.read_point_to_file("arr48.tsp"), "performance.txt");
        //the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_from_file("br17.atsp"), 0, "perform_rnn.txt"), "perform_rnn.txt");
        //vector<int> saves = the_salesman_problem.nearest_neighbour(the_salesman_problem.read_from_file("br17.atsp"), 0, "perform_nn.txt");
        //saves.pop_back();

        //int result = the_salesman_problem.twoOpt(the_salesman_problem.read_from_file("br17.atsp"), saves, the_salesman_problem.get_path_length(the_salesman_problem.read_from_file("br17.atsp"), saves, 17));
        //cout << result;
        //the_salesman_problem.read_point_to_file("arr48.tsp");
        //the_salesman_problem.read_from_file("br17.atsp");




        string answer = "", answer1 = "";
        int start_point = 0;

        while(answer != "end"){

            cout << "name of file ";

            cin >> answer;

            if(answer == "end") break;

            cout << "type of file matrix/points ";

            cin >> answer1;

            cout << "from what point want to start ";

            cin >> start_point;

            if( answer1 == "matrix"){

                the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_matrix_from_file(answer), start_point, "perform.txt"), "perform.txt");
                vector<int> saves = the_salesman_problem.nearest_neighbour(the_salesman_problem.read_matrix_from_file(answer), start_point, "perform.txt");
                int lenght = saves.back();
                saves.pop_back();
                cout << endl << "result from 2 opt algo " << the_salesman_problem.two_opt(the_salesman_problem.read_matrix_from_file(answer), saves, lenght, "perform_opt2.txt") << endl;

            } else if( answer1 == "points"){

                the_salesman_problem.print(the_salesman_problem.nearest_neighbour(the_salesman_problem.read_point_to_file(answer), start_point, "perform.txt"), "perform.txt");
                vector<int> saves = the_salesman_problem.nearest_neighbour(the_salesman_problem.read_point_to_file(answer), start_point, "perform.txt");
                int lenght = saves.back();
                saves.pop_back();
                cout << endl << "result from 2 opt algo " << the_salesman_problem.two_opt(the_salesman_problem.read_point_to_file(answer), saves, lenght, "perform_opt.txt") << endl;

            } else {

                cout << "invalid type of file" << endl;
            }



        }


}