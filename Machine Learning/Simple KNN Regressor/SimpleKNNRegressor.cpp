#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#define trainingSample "training.csv"//Select your training dataset
#define testSample "test.csv"//Select your test dataset

using namespace std;

//auxilary function for sorting neighbors
bool compareDist(vector<double> a, vector<double> b) {
    int n = a.size();
    return(a[n - 1] < b[n - 1]);
}

double EuclDist(vector<double> x, vector<double> y) {
    int n = x.size();
    double D = 0.0;
    for (int i = 0; i < n; i++)
        D += (x[i] - y[i]) * (x[i] - y[i]);
    return sqrt(D);
}

double ManDist(vector<double> x, vector<double> y) {
    int n = x.size();
    double D = 0.0;
    for (int i = 0; i < n; i++)
        D += abs(x[i] - y[i]);
    return D;
}

//output format:
//{ {x1, x2,...,xn, y, distance},
//.............................}
vector<vector<double> >KneighborFinder(vector<vector<double> > training, vector<double> tex, int dm, int K) {
    int n = training.size(); int m = training[0].size();
    vector<vector<double> >neis;
    for (int i = 0; i < n; i++) {
        vector<double> tmp;
        double distance = 0.0;
        //to align with the format of the test data, so as to fit in the distance function
        for (int j = 0; j < m - 1; j++)
            tmp.push_back(training[i][j]);
        if (dm == 1)
            distance = EuclDist(tex, tmp);
        else
            distance = ManDist(tex, tmp);
        //append X before appending labels
        tmp.push_back(training[i][m - 1]);
        tmp.push_back(distance);
        neis.push_back(tmp);
    }
    //sort by distance and output the first K results
    sort(neis.begin(),neis.end(),compareDist);
    vector<vector<double> >Kneis;
    for (size_t i = 0; i < K; i++){
        vector<double> tmp(neis[i]);
        Kneis.push_back(tmp);
    }
    return Kneis;
}

double RMSE(vector<vector<double> >test) {
    int n = test.size(), m = test[0].size();
    double R2 = 0.0;
    for (int i = 0; i < n; i++)
        R2 += abs(test[i][m - 1] - test[i][m - 2]) * abs(test[i][m - 1] - test[i][m - 2]);
    return (R2 / n);
}

/*weighted regressor
feed the K neighbors and the current data sample without the label.*/
double regressorWgt(vector<vector<double> > Kneis, int K) {
    int m = Kneis[0].size();
    if (K == 1)
        return (Kneis[0][m - 2] / Kneis[0][m - 1]);
    else
        return ((Kneis[0][m - 1] * Kneis[1][m - 2] + Kneis[1][m - 1] * Kneis[0][m - 2]) / (Kneis[0][m - 1] + Kneis[1][m - 1]));
}

//regressor unweighted
double regressor(vector<vector<double> > Kneis) {
    int k = Kneis.size(), m = Kneis[0].size();
    double pry = 0.0;
    for (int i = 0; i < k; i++)
        pry += Kneis[i][m - 2];
    return (pry / k);
}

vector<vector<double> >readdata(string filename) {
    ifstream inFile(filename, ios::in);
    string lineStr;
    vector<vector<double> >doubleArray;
    while (getline(inFile, lineStr))
    {
        stringstream ss(lineStr);
        string str;
        vector<double> lineArray;

        while (getline(ss, str, ',')) {
            if (str[0] == '-' || (str[0] >= 48 && str[0] <= 57))//to ignore ledgends of the table
                lineArray.push_back(std::stoi(str)*1.0);
        }
        if (lineArray.size())
            doubleArray.push_back(lineArray);
    }
    inFile.close();
    return doubleArray;
}

void writeResults(vector<vector<double> > test, double err) {
    int n = test.size(); int m = test[0].size();

    ofstream outFile;
    outFile.open("results.csv", ios::out);

    for (int i = 0; i < m-2; i++)
        outFile << "x" + to_string(i + 1) << ',';
    outFile << "Truth" << ',' << "Prediction" << endl;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            outFile << test[i][j] << ',';
        outFile << endl;
    }

    outFile << endl;

    outFile << "Simple Error" << ',' << err << endl;

    outFile.close();
}

int main()
{
    //input K
    cout << "Please slect K from 1 or 2" << endl;
    cout << "Confirm your choice pressing 'Enter'." << endl;
    int K;
    cin >> K;
    while (true) {
        if (K == 1 || K == 2)
            break;
        else {
            cout << "Error! K can only be 1 or 2!" << endl;
            cout << "Please select again." << endl;
            cin >> K;
        }
    }

    //select distancing method
    cout << "Please select a distancing method." << endl;
    cout << "Press 1 for Euclidean distance, 2 for Manhattan distance." << endl;
    cout << "Confirm your selection with Enter." << endl;
    int dm;
    cin >> dm;
    while (true) {
        if (dm == 1 || dm == 2)
            break;
        else{
            cout << "Error! Please choose from 1 for Euclidean distance" << endl;
            cout << "or 2 for Manhattan distance." << endl;
            cin >> dm;
        }
    }

    //select weighted or not
    cout << "Weighted? [y/n]" << endl;
    bool isWgt;
    char wgtSel;
    cin >> wgtSel;
    if (wgtSel == 'y'||wgtSel=='Y')
        isWgt = true;
    else
        isWgt = false;

    //read training data
    vector<vector<double> >training;
    training = readdata(trainingSample);

    //read test data
    vector<vector<double> >test;
    test = readdata(testSample);
    vector<vector<double> > teX;
    int n = test.size(); int m = test[0].size() - 1;
    for (int i = 0; i < n; i++) {
        vector<double> tmp;
        for (int j = 0; j < m; j++)
            tmp.push_back(test[i][j]);
        teX.push_back(tmp);
    }

    /*KNN regressing
    vector test after processing:
    { X1{x1, x2,..., xn, truth, prediction},
      ......
    }*/
    for (int i = 0; i < n; i++) {
        vector<vector<double> > Kneis = KneighborFinder(training, teX[i], dm, K);
        if (isWgt)
            test[i].push_back(regressorWgt(Kneis, K));
        else
            test[i].push_back(regressor(Kneis));
    }
    //calculate the test error
    double R = RMSE(test);

    //output results
    writeResults(test, R);
}
