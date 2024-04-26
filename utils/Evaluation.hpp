#ifndef EVALUATION_H
#define EVALUATION_H
#include "../simulator/Dataplane.hpp"
#include <utility>

double BloomFilterEvaluation(const char* trace, Dataplane* dp);

std::pair<double, double> KMVSuperSpreaderEvaluation(const char* trace, Dataplane* dp);


#endif // EVALUATION_H