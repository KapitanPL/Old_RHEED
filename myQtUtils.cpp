#include "myQtUtils.h"

int dvMax(QVector<double> *invec){
    if (invec->size()>0){
        int ret = 0;
        double val = invec->at(0);
        for (int i=0;i<invec->size();i++){
            if (invec->at(i)>val){
                ret=i;
            }
        }
        return ret;
       }
    else{
        return 0;
    }
}

int dvMin(QVector<double> *invec){
    if (invec->size()>0){
        int ret = 0;
        double val = invec->at(0);
        for (int i=0;i<invec->size();i++){
            if (invec->at(i)<val){
                ret=i;
            }
        }
        return ret;
       }
    else{
        return 0;
        }
}

int Str2vector(std::string input, std::vector<double> *output){ //assumes space is used as delimiter
    std::size_t pos;
    std::string num;
    while (!input.empty()){
        pos = input.find(" ");
        if (pos!=std::string::npos){
            num = input.substr(0,pos);
            input.erase(0,pos+1);
        }
        else{
            num = input;
            input.erase(0,input.size());
        }
        pos = num.find("NaN");
        if (pos==std::string::npos){
            output->push_back(::atof(num.c_str()));
        }
    }
    return 0;
}

QVector<double> vectorSubstract(QVector<double> *a, QVector<double> *b){
    QVector<double> ret;
    ret.append(0.0);
    if (a->size()==b->size()){
        ret.resize(a->size());
        for (int i =0; i<ret.size();++i){
            ret[i]=a->at(i)-b->at(i);
        }
    }
    return ret;
}
