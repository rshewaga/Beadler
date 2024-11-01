#include "ColorDifference.hpp"
#include <cmath>

float ColorDifference::Redmean(float _r1, float _g1, float _b1, float _r2, float _g2, float _b2)
{
    float rmean = (_r1 + _r2) / 2.0f;
    float dR = _r1 - _r2;
    float dG = _g1 - _g2;
    float dB = _b1 - _b2;
    
    return std::sqrtf((2.0f + rmean / 256.0f) * dR * dR + 4.0f * dG * dG + (2.0f + (255.0f - rmean) / 256.0f) * dB * dB);
}