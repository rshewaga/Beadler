#pragma once
#include <filesystem>
#include <map>

#include <ColorData.hpp>

#include <wx/wx.h>


class App : public wxApp {
public:
    bool OnInit();

    /**
     * @brief Downloads the color database files
     * This clones the Beadifier color database GitHub repository to the executable directory
     * if the .csv files don't already exist from a previous run.
     * @return Whether the database files are available to load
     */
    bool MakeColorDatabaseAvailable();

    /**
     * @brief Loads the color database .csv files in m_colorDatabasePath
     * @return Whether all files were loaded successfully
     */
    bool LoadColorDatabase();

private:
    std::filesystem::path m_colorDatabasePath;  // Path to the folder container bead color csv files
    std::map<std::string, std::vector<ColorData>> m_colorDatabase;    // <brand skew, color entries>
};

wxIMPLEMENT_APP(App);