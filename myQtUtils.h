#ifndef MYQTUTILS_H
#define MYQTUTILS_H

#endif // MYQTUTILS_H

#include <QVector>

int dvMax(QVector<double> *invec);
int dvMin(QVector<double> *invec);
int Str2vector(std::string input, std::vector<double> *output);
QVector<double> vectorSubstract(QVector<double> *a, QVector<double> *b);
