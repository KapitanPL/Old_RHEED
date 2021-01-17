# include "fitter.h"
# include <cmath>
# include <vector>
# include "simplex.h"

#ifndef M_PI
#define M_PI 3.141592
#endif

using namespace std;

vector<double> oscilations(vector<double> params, vector<double> x){
    vector<double> ret(x.size());
    for (unsigned i=0;i<x.size();++i){
        ret[i]=params[0]+params[1]*x[i]+exp(params[2]*x[i])*sin(2*M_PI*params[3]*x[i]+params[4])*params[5];
    }
    return ret;
}

vector<double> linear(vector<double> params, vector<double> x){
    vector<double> ret(x.size());
    for (unsigned i=0;i<x.size();++i){
        ret[i]=params[0]+params[1]*x[i];
    }
    return ret;
}

fitter::fitter(vector<double> x, vector<double> y, vector<double> (*fitted)(vector<double>,vector<double>)){
    if (x.size() == y.size()){
        this->ex=x;
        this->yp=y;
        this->size=x.size();
        this->fits.resize(size);
        this->function = fitted;
    }
}

fitter::~fitter(){
}

double fitter::operator()(vector<double> params){
    double ret=0.0;
    this->fits=this->function(params,this->ex);
    for (int i=0;i<size;++i){
        ret+= pow(this->fits[i]-this->yp[i],2);
    }
    return ret;
}

vector<double> optimizeM(fitter obj, vector<double> gues){
    vector<double> outgues(gues.size());
    using BT::Simplex;
    outgues = Simplex(obj, gues);
    return outgues;
}
