// PorousStructures.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Author: Albina Klepach. Date: 25/05/2021

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>
//#include <mpi.h>

using namespace std;

int ProcessFile(string filename, int n_of_rank)
{
    string line;
    int fill_cluster;
    bool flag;
    int neighbour_value;
    int cluster_value;
    int i1, i2, j1, j2;

    const double pi = 2 * acos(0.0); // pi value
    //array for neighbours
    const unsigned int DIM1 = 4;
    const unsigned int DIM2 = 2;
    int neighbours[DIM1][DIM2];

    // read file
    ifstream readFile(filename);
    filename = filename.substr(0, filename.size()-1);
    if (readFile.is_open())
    {
        cout << "File " << filename << " was opened in " << n_of_rank << " \n";
    }
    else
    {
        cout << "File " << filename << " was NOT opened in " << n_of_rank << " \n";
        return 0;
    }
    // create outputfile
    filename = filename.substr(0, filename.size()-4);
    //filename.erase(4);
    filename += ".txt";
    ofstream writeFile(filename);

    vector<int> square;
    vector<int> perimeter;
    vector<double> radius;

    //int l[3];
    int l[3];
    int laa;

    for (int i = 0; i < 3; i++) {

        readFile >> l[i];
        cout << l[i] << " ";
        //readFile >> laa;
        //cout << laa << endl;
    }
    cout << endl;
    writeFile << l[0] * l[1] * l[2] << endl;
    const int nword = int(l[0] * l[1] * l[2] / 16);
    //const int nword = int(192 * 192 * 192 / 16);
    cout << "nword is " << nword << " in " << n_of_rank << endl;
    int masks[17];
    masks[0] = 1;

    for (int i = 1; i < 17; i++) {

        masks[i] = masks[i - 1] * 2;

    }
    vector<int> words; //size (nwords)

    for (int i = 0; i < nword; i++) {

        readFile >> laa;
        words.push_back(laa);
    }

    int m, n, out, pos1, pos2;
    for (int k = 0; k < l[2]; k++) {

        vector<vector<int>> slice;
        cout << "slice " << k << " is in process in " << n_of_rank  << endl;

        for (int j = 0; j < l[1]; j++) {

            vector<int> temp;

            for (int i = 0; i < l[0]; i++) {

                m = k * l[0] * l[1] + j * l[0] + i;
                //n = m % 16;
                pos1 = m / 16; //l
                pos2 = int(m - 16 * pos1); //pos
                //pos variable in python code is n = m % 16
                // l variable in python code is ( m / 16 )
                out = 1 - int(words[pos1] - int(words[pos1] / masks[pos2 + 1]) * masks[pos2 + 1]) / masks[pos2];
                //writeFile << out << endl;
                temp.push_back(out);

            }
            slice.push_back(temp);
        }
        //now we have one slice in slice

        //write slice to file to check if it is ok
        /*for (int j = 0; j < 20; j++) {

            for (int i = 0; i < 20; i++) {

                tempFile << slice[j][i] << " ";
            }
            tempFile << endl;
        }*/
        //create cluster matrix
        vector<vector<int>> cluster;

        fill_cluster = 0;
        for (int j = 0; j < l[1]; j++) {

            vector<int> temp;

            for (int i = 0; i < l[0]; i++) {

                if (slice[j][i] == 1) {

                    fill_cluster += 1; //k value in python code
                    temp.push_back(fill_cluster);
                }
                else {
                    temp.push_back(0);
                }
            }
            cluster.push_back(temp);
        }
        cout << "fill_cluster = " << fill_cluster << " in " << n_of_rank  << endl;
        /*tempFile << endl << endl << "====================================" << endl << endl;
        for (int j = 0; j < 20; j++) {

            for (int i = 0; i < 20; i++) {

                tempFile << cluster[j][i] << " ";
            }
            tempFile << endl;
        }*/
        // loop for merging clusters

        for (int j = 0; j < l[1]; j++) {

            for (int i = 0; i < l[0]; i++) {

                if (cluster[j][i] != 0) {
                    //cout << "we found a pore " << j << " " << i << " " << cluster[j][i] << endl;

                    //periodic boundary condiotions
                    neighbours[0][0] = (j != l[1] - 1) ? (j + 1) : 0;
                    neighbours[0][1] = i;

                    neighbours[1][0] = (j != 0) ? (j - 1) : (l[1] - 1);
                    neighbours[1][1] = i;

                    neighbours[2][0] = j;
                    neighbours[2][1] = (i != l[0] - 1) ? (i + 1) : 0;

                    neighbours[3][0] = j;
                    neighbours[3][1] = (i != 0) ? (i - 1) : (l[0] - 1);

                    for (int p = 0; p < 4; p++) {

                        neighbour_value = cluster[neighbours[p][0]][neighbours[p][1]];
                        if ((neighbour_value > 0) && (cluster[j][i] != neighbour_value)) {

                            for (int qj = 0; qj < l[1]; qj++) {

                                for (int qi = 0; qi < l[0]; qi++) {

                                    if (cluster[qj][qi] == neighbour_value) {
                                        cluster[qj][qi] = cluster[j][i];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        /*tempFile << endl << endl << "====================================" << endl << endl;

        for (int j = 0; j < 20; j++) {

            for (int i = 0; i < 20; i++) {

                tempFile << cluster[j][i] << " ";
            }
            tempFile << endl;
        }
        tempFile << endl << endl << "====================================" << endl << endl;*/

        vector<int> taken;
        vector<vector<int>> clusters;
        for (int j = 0; j < l[1]; j++) {

            for (int i = 0; i < l[0]; i++) {
                flag = true;
                if (cluster[j][i] > 0) {

                    cluster_value = cluster[j][i];

                    for (int p = 0; p < taken.size(); p++) {

                        if (cluster_value == taken[p]) {
                            flag = false;
                            break; //check if this cluster is already processed
                        }

                    }

                }
                else {
                    flag = false;
                }
                if (flag) {
                    taken.push_back(cluster_value);
                    vector<int> temp;
                    for (int qj = 0; qj < l[1]; qj++) {
                        for (int qi = 0; qi < l[0]; qi++) {
                            if (cluster[qj][qi] == cluster_value) {
                                temp.push_back(qj * l[0] + qi);
                                cluster[qj][qi] = 0 - taken.size();
                            }
                        }
                    }
                    //cout << "for cluster " << taken.size() << " we have " << temp.size() << " in " << n_of_rank << endl;
                    clusters.push_back(temp);
                }
            }
        }
        for (int j = 0; j < l[1]; j++) {

            for (int i = 0; i < l[0]; i++) {

                cluster[j][i] = -cluster[j][i];
            }
        }

        /*for (int j = 0; j < 20; j++) {

            for (int i = 0; i < 20; i++) {

                tempFile << cluster[j][i] << " ";
            }
            tempFile << endl;
        }*/

        for (int j = 0; j < l[1]; j++) {

            for (int i = 0; i < l[0]; i++) {

                j1 = (j == 0) ? (l[1] - 1) : j - 1;
                j2 = (j == l[1] - 1) ? 0 : j + 1;
                i1 = (i == 0) ? (l[0] - 1) : i - 1;
                i2 = (i == l[0] - 1) ? 0 : i + 1;

                if ((slice[j1][i] > 0) && (slice[j2][i] > 0)) {
                    if ((slice[j][i1] > 0) && (slice[j][i2] > 0)) {
                        slice[j][i] = 2;
                    }
                }
            }
        }
        /*for (int j = 0; j < 20; j++) {

            for (int i = 0; i < 20; i++) {

                tempFile << slice[j][i] << " ";
            }
            tempFile << endl;
        }*/

        int SIZE = clusters.size();
        vector<int> square;
        vector<int> perimeter;
        vector<double> radius;

        for (int j = 0; j < SIZE; j++) {

            int inner_SIZE = clusters[j].size();
            radius.push_back(sqrt(double(inner_SIZE) / pi));
            square.push_back(inner_SIZE);

            int counter = 0;
            for (int i = 0; i < inner_SIZE; i++) {

                j1 = clusters[j][i] / l[0];
                i1 = clusters[j][i] - j1 * l[0];
                if (slice[j1][i1] == 1) {
                    counter += 1;
                }
            }
            perimeter.push_back(counter);
            //cout << 1 << endl;
            writeFile << radius[j] << ' ' << square[j] << ' ' << perimeter[j] << ' ' << endl;
        }
    }
    readFile.close();
    writeFile.close();

    return 0;
}

string getFileName(int id, string file_with_names = "filenames.txt")
{
    //string name_of_out = "some_output.txt";
    string str; // for reading file_with_names line by line
    vector<string> filenames; //vector of names of files.map

    // read file with filenames
    ifstream readFileNames(file_with_names);
    if (readFileNames.is_open())
    {
        cout << "File " << file_with_names << " was opened\n";
    }
    else
    {
        cout << "File " << file_with_names << " was NOT opened\n";
        return 0;
    }
    // create outputfile
    //ofstream outFile(name_of_out);
    while (getline(readFileNames, str)) {
        filenames.push_back(str);
    }
    cout << filenames[id] << endl;
    return filenames[id];
}

int main(int argc, char ** argv) {

    int psize, prank;
    //MPI_Status status;
    string file_with_names = "filemapnames.txt";
    int n_of_files = 1731;
    int n_of_loops, loop_step; // loop variables
    int file_id; // id of currently processing file
    string filename;

    psize = 24;
    sscanf(argv[1], "%d", &prank);

    //MPI_Init(&argc, &argv);
    //MPI_Comm_rank(MPI_COMM_WORLD, &prank);
    //MPI_Comm_size(MPI_COMM_WORLD, &psize);

    n_of_loops = (int)(n_of_files / psize);
    for (loop_step=0; loop_step < n_of_loops; loop_step++){
        file_id = loop_step * psize + prank;
        filename = getFileName(file_id, file_with_names);
        cout << filename << " is in process on " << prank << endl;
        ProcessFile(filename, prank);
        cout << filename << " finished on " << prank << endl;
        // to wait for all processes send an receive a message
        
    }
    
    //MPI_Finalize();

    return 0;
}
