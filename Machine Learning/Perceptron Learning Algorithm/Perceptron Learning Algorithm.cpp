#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#define trainingSample "training.csv"//Select your training dataset
#define testSample "test.csv"//Select your test dataset

using namespace std;

void writeResults(vector<vector<int> > teX, vector<int> teY, vector<int> prY, double err) {
    int n = teX.size(); int m = teX[0].size() - 1;

    ofstream outFile;
    outFile.open("results.csv", ios::out);

    for (int i = 0; i < m; i++)
        outFile << "x" + to_string(i + 1) << ',';
    outFile << "Prediction" << ',' << "Truth" << endl;

    for (int i = 0; i < n; i++) {
        for (int j = 1; j < m + 1; j++) 
            outFile << teX[i][j] << ',';
        outFile << prY[i] << ',' << teY[i] << endl;
    }

    outFile << "Simple Error" << ',' << err << endl;

    outFile.close();
}

vector<vector<int> >readdata(string filename) {
    ifstream inFile(filename, ios::in);
    string lineStr;
    vector<vector<int> >intArray;
    while (getline(inFile, lineStr))
    {
        stringstream ss(lineStr);
        string str;
        vector<int> lineArray;

        while (getline(ss, str, ',')) {
            if (str[0]=='-'||(str[0] >= 48 && str[0] <= 57))//to ignore ledgends of the table
                lineArray.push_back(std::stoi(str));
        }
        if (lineArray.size())
            intArray.push_back(lineArray);
    }
    inFile.close();
    return intArray;
}

//given a sample and weight, classify the sample
int classify(vector<int> x, vector<int> W) {
    int m = x.size();
    int res = 0;
    for (size_t i = 0; i < m; i++)
        res += x[i] * W[i];
    if (res < 0)
        return -1;
    else
        return 1;
}

//Check whether two vectors(sample, weight, etc) are the same
bool vectorSame(vector<int> a, vector<int> b) {
    int na = a.size(); int nb = b.size();
    if (na != nb)
        return false;
    for (int i = 0; i < na; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

//Copy vector X to Y
void vectorCopy(vector<int>& x, vector<int>& y) {
    int n = x.size();
    for (int i = 0; i < n; i++) {
        y[i] = x[i];
    }
}

vector<int> PLA(vector<vector<int> > X, vector<int> Y) {
    int n = X.size(); int m = X[0].size();

    vector<vector<int> >x;
    vector<int> tmp;
    for (int i = 0; i < n; i++) {
        tmp.clear();
        tmp.push_back(1);
        for (int j = 0; j < m; j++)
            tmp.push_back(X[i][j]);
        x.push_back(tmp);
    }

    //initialize the current weight.
    vector<int> curW(m + 1, 0);
    //initialize and store the weight for each sample after the last iteration.
    vector<vector<int> >preW(n, vector<int>(m + 1, 0));

    bool convergency = false;
    while (!convergency) {

        //use a counter to check whehter the weight changes during each iteration.
        int cnt = 0;
        for (int i = 0; i < n; i++) {

            //if the current weight can classify this sample correctly
            if (classify(x[i], curW) == Y[i]) {

                //check if the current weight is the same as the previous one.
                if (!vectorSame(curW, preW[i])) {
                    vectorCopy(curW, preW[i]);

                    //add 1 to the counter if the weight changes.
                    cnt++;
                }
            }
            else {
                //if the current weight can not classify the sample.
                for (int j = 0; j < m + 1; j++) {

                    //update the current weight according to the sample.
                    curW[j] += Y[i] * x[i][j];
                }
                vectorCopy(curW, preW[i]);
                cnt++;
            }
        }

        // if after iterating all 4 training samples, the weight remains the same, it means the weight has converge.
        if (cnt == 0)

            //stop the loop
            convergency = true;
    }

    return curW;
}

//Calculate the sample error
double SimpleErrClassifier(vector<int> prY, vector<int> Y) {
    double err = 0;
    for (int i = 0; i < prY.size(); i++) {
        if (prY[i] != Y[i])
            err += 1.0;
    }
    return err / prY.size();
}

int main() {

    //read training data.
    vector<vector<int> > training;
    training = readdata(trainingSample);
    int n = training.size(); int m = training[0].size();
    vector<vector<int> >trX; vector<int> trY;
    vector<int> tmp;
    for (int i = 0; i < n; i++) {
        tmp.clear();
        for (int j = 0; j < m-1; j++) {
            tmp.push_back(training[i][j]);
        }
        trX.push_back(tmp);
        trY.push_back(training[i][m - 1]);
    }

    //store the weight obtained.
    vector<int> W;
    W=PLA(trX, trY);

    //read test data.
    vector<vector<int> >test;
    test = readdata(testSample);
    n = test.size(); m = test[0].size();
    vector<vector<int> >teX; vector<int>teY; vector<int>prY;
    for (int i = 0; i < n; i++) {
        tmp.clear();
        tmp.push_back(1);
        for (int j = 0; j < m - 1; j++)
            tmp.push_back(test[i][j]);
        teX.push_back(tmp);
        teY.push_back(test[i][m - 1]);
    }

    //classify the test data with the weight.
    for (int i = 0; i < n; i++)
        prY.push_back(classify(teX[i], W));

    //compute the error.
    double error = SimpleErrClassifier(prY, teY);

    //output the processed test data.
    writeResults(teX, teY, prY, error);

    return 0;
}