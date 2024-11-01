#pragma once
#include <string>

/**
 * @brief Contains color data for a single skew of bead
 * See '/v3' row in https://github.com/maxcleme/beadcolors?tab=readme-ov-file#format
 * [reference_code, name, symbol, rgb_r, rgb_g, rgb_b, hsl_h, hsl_s, hsl_l, lab_l, lab_a, lab_b, contributor]
 */
struct ColorData
{
public:
    std::string m_code = "";
    std::string m_name = "";
    std::string m_symbol = "";
    int m_rgb_r, m_rgb_g, m_rgb_b = 0;
    float m_hsl_h, m_hsl_s, m_hsl_l = 0;
    float m_lab_l, m_lab_a, m_lab_b = 0;

    const static int m_tokenCount = 13;    // Number of tokens in a single .csv line
};