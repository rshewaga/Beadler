#pragma once

namespace ColorDifference
{
    /**
     * @brief Uses a weighted Euclidean distance function.
     * https://www.compuphase.com/cmetric.htm
     * @param _r1 Red component of first color, (0-255)
     * @param _g1 Green component of first color, (0-255)
     * @param _b1 Blue component of first color, (0-255)
     * @param _r2 Red component of second color, (0-255)
     * @param _g2 Green component of second color, (0-255)
     * @param _b2 Blue component of second color, (0-255)
     * @return (0-765), lower = more similar, higher = more different
     */
    float Redmean(float _r1, float _g1, float _b1, 
                  float _r2, float _g2, float _b2);
}