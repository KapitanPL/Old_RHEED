#ifndef FITTER_H
#define FITTER_H
#include <vector>

using namespace std;

vector<double> oscilations(vector<double> params, vector<double> x); //main fitting function tilted and dumped sinus
vector<double> linear(vector<double> params, vector<double> x); // oxiliary fitting function


class fitter
{
public:
    ~fitter();
    fitter(vector<double> x, vector<double> y, vector<double> (*fitted)(vector<double>,vector<double>));
    double operator()(vector<double> params);
private:
    vector<double> (*function)(vector<double>, vector<double>);
    vector<double> ex, yp;
    vector<double> fits;
    int size;
};

vector<double> optimizeM(fitter obj, vector<double> gues);

#endif // FITTER_H
