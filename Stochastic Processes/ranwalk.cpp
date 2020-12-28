#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

void writeResults(vector<vector<int> > data) {
    int n = data.size(); int m = data[0].size();

    ofstream outFile;
    outFile.open("results.csv", ios::out);

    outFile << "T" << ',' << "X" << endl;;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            outFile << data[i][j] << ',';
        outFile << endl;
    }

    outFile.close();
}

//read probability p.
double pInput() {
    string input;
    cout << "Please enter the left barrier(probability), end up with Enter." << endl;
    cin >> input;
    double res = stod(input);
    input.clear();
    while (res >= 1.0) {
        cout << "Error! The barrier must be less than 1 and larger than 0!" << endl;
        cout << "Please enter the barrier again." << endl;
        cin >> input;
        res = stod(input);
    }
    return res;
}

//input time T
int tInput() {
    int res;
    cout << "Please enter the desired time T, T is an integer and 0 < T <= 10000, end up with Enter." << endl;
    cin >> res;
    while (res <= 0 || res > 10000) {
        cout << "Error! Time T is out of range!" << endl;
        cout << "Please enter T again." << endl;
        cin >> res;
    }
    return res;
}

int main() {
    double p = pInput();
    double q = 1.0 - p;
    int T = tInput();
    srand((int)time(0));//random seed using time
    int X = 1;
    vector<vector<int> > data;
    vector<int> tmp;
    for (int i = 1; i <=T; i++) {

        tmp.clear();
        tmp.push_back(i);
        tmp.push_back(X);
        data.push_back(tmp);

        if (X == 1)
            X = 2;
        else {
            double r = rand() / double(RAND_MAX);//generate a random number smaller than 1.0, larger than 0.0.
            if (r < q)
                X += -1;
            else if (r >= q)
                X += 1;
        }
    }

    writeResults(data);
}
