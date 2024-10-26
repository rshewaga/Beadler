#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <lyra/lyra.hpp>
#include <nlohmann/json.hpp>
#include <wx/wx.h>

#include <MainFrame.hpp>

/**
 * Parse command line inputs.
 * Exit on any issues.
 * 
 * @param argc From main
 * @param argv From main
 * @return The input absolute file path
 */
std::filesystem::path ParseCLI(const int argc, char** argv)
{
    bool show_help = false;
    std::string inputPath = "";

    lyra::cli cli = lyra::cli()
                | lyra::help(show_help).description("Convert a TubiTV downloaded subtitle file into a regular formatted .srt file. The output is in the same folder as the input file.")
                | lyra::arg(inputPath, "input file path")("Absolute file path to the TubiTV downloaded subtitle file.").required();

    auto result = cli.parse({argc, argv});
    if(!result)
    {
        std::cerr << "Error in command line: " << result.message() << std::endl;
        exit(EXIT_FAILURE);
    }

    if(show_help)
    {
        std::cout << cli << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Check if the input path is actually an absolute file path
    std::filesystem::path asPath(inputPath);
    if(!asPath.is_absolute())
    {
        std::cerr << "Input file path isn't absolute!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(!std::filesystem::exists(asPath))
    {
        std::cerr << "The input file path doesn't point to an existing file!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(!std::filesystem::is_regular_file(asPath))
    {
        std::cerr << "The input file path is pointing to a non-file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return asPath;
}

