#ifndef KERNELS_HPP
#define KERNELS_HPP

#include <vector>
using namespace std;

// Noyau Sobel pour le gradient X
const vector<vector<float>> SOBEL_X = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

// Noyau Sobel pour le gradient Y
const vector<vector<float>> SOBEL_Y = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

#endif // KERNELS_HPP
