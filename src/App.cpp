#include <App.hpp>
#include <MainFrame.hpp>
#include <ColorData.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

#include <git2/global.h>
#include <git2/repository.h>
#include <git2/clone.h>
#include <git2/errors.h>

#include <fmt/format.h>

/**
 * @brief Custom certificate verification callback for libgit2. Always allows the certificate to proceed.
 */
static int R_git_transport_certificate_check_cb(git_cert *cert, int valid, const char *host, void *payload)
{
    (void)cert;
    (void)valid;
    (void)host;
    (void)payload;
    return 0;
}

bool App::OnInit()
{
    MakeColorDatabaseAvailable();
    LoadColorDatabase();

    MainFrame* mainFrame = new MainFrame("Beadler");
    mainFrame->SetClientSize(1280,720);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}

bool App::MakeColorDatabaseAvailable()
{
    namespace fs = std::filesystem;

    m_colorDatabasePath = fs::current_path() / "v3";

    // Check whether the files are already available
    if(fs::exists(m_colorDatabasePath))
    {
        if(!fs::is_empty(m_colorDatabasePath))
        {
            return true;
        }
        
        // Resource directory exists but it's empty. Delete it and reclone.
        fs::remove_all(m_colorDatabasePath);
    }

    // Clone the bead color repository

    // Delete the cloned repository if it already exists
    fs::path _repoPath = fs::current_path() / "beadcolors";
    if(fs::exists(_repoPath))
    {
        fs::remove_all(_repoPath);
    }

    int _initResult = git_libgit2_init();
    if(_initResult < 0)
    {
        const git_error* initErr = git_error_last();
        std::cerr << wxString::Format("Can't initialize libgit2 (%i): %s", initErr->klass, initErr->message) << "\n";
        return false;
    }

    git_checkout_options dummy_opts = GIT_CHECKOUT_OPTIONS_INIT;
	git_fetch_options dummy_fetch = GIT_FETCH_OPTIONS_INIT;
	git_repository* g_repo = NULL;

    static git_clone_options g_options;
	memset(&g_options, 0, sizeof(git_clone_options));
	g_options.version = GIT_CLONE_OPTIONS_VERSION;
	g_options.checkout_opts = dummy_opts;
	g_options.fetch_opts = dummy_fetch;
	g_options.fetch_opts.callbacks.certificate_check = R_git_transport_certificate_check_cb;

    int _cloneResult = git_clone(&g_repo, "https://github.com/maxcleme/beadcolors.git", _repoPath.string().c_str(), &g_options);
    if(_cloneResult < 0)
    {
        const git_error* initErr = git_error_last();
        std::cerr << wxString::Format("Can't clone beadcolors.git (%i): %s", initErr->klass, initErr->message) << "\n";
        return false;
    }

    git_libgit2_shutdown();

    // After the clone, copy the v3 folder out of the repository and delete the repository
    fs::path _v3RepoPath = _repoPath / "gen" / "v3";

    if(!fs::exists(_repoPath) || !fs::exists(_v3RepoPath))
    {
        std::cerr << wxString::Format("Cloning the repository worked but the expected resource path doesn't exist! \"%s\"", _v3RepoPath.string()) << "\n";
        return false;
    }

    std::error_code _copyErr;
    fs::copy(_v3RepoPath, m_colorDatabasePath, fs::copy_options::recursive, _copyErr);
    if(_copyErr)
    {
        std::cerr << wxString::Format("Copying the resource folder from the cloned repository failed! \"%i\"", _copyErr.value()) << "\n";
        return false;
    }

    std::error_code _repoDelErr;
    //fs::remove_all(_repoPath, _repoDelErr);
    if(_repoDelErr)
    {
        std::cerr << wxString::Format("Deleting the repository folder failed! \"%i\"", _repoDelErr.value()) << "\n";
        return false;
    }

    return true;
}

bool App::LoadColorDatabase()
{
    bool _allLoadedSuccessfully = true;

    namespace fs = std::filesystem;

    std::string _line;
    std::string _word;
    std::stringstream _stream;
    std::vector<std::string> _tokens;
    _tokens.reserve(ColorData::m_tokenCount);

    // For each .csv file in the color database, load it in
    for (const fs::directory_entry& _file : fs::recursive_directory_iterator(m_colorDatabasePath))
    {
        if(_file.path().extension() == ".csv")
        {
            std::ifstream _in(_file.path());

            // Check if the file is successfully opened
            if (!_in.is_open()) {
                std::cerr << fmt::format("Error opening file \"{}\"", _file.path().string()) << "\n";
                _allLoadedSuccessfully = false;
                continue;
            }

            
            while(std::getline(_in, _line))
            {
                _stream.clear();
                _stream.str(_line);
                _tokens.clear();

                while(std::getline(_stream, _word, ','))
                {
                    _tokens.emplace_back(_word);
                }

                // After all tokens in the line are read in, convert that to ColorData
                ColorData _data;
                // [reference_code, name, symbol, rgb_r, rgb_g, rgb_b, hsl_h, hsl_s, hsl_l, lab_l, lab_a, lab_b, contributor]
                _data.m_code = _tokens[0];
                _data.m_name = _tokens[1];
                _data.m_symbol = _tokens[2];
                _data.m_rgb_r = std::stoi(_tokens[3]);
                _data.m_rgb_g = std::stoi(_tokens[4]);
                _data.m_rgb_b = std::stoi(_tokens[5]);
                _data.m_hsl_h = std::stof(_tokens[6]);
                _data.m_hsl_s = std::stof(_tokens[7]);
                _data.m_hsl_l = std::stof(_tokens[8]);
                _data.m_lab_l = std::stof(_tokens[9]);
                _data.m_lab_a = std::stof(_tokens[10]);
                _data.m_lab_b = std::stof(_tokens[11]);

                if(!m_colorDatabase.contains(_file.path().stem().string()))
                {
                    m_colorDatabase[_file.path().stem().string()] = {};
                }
                m_colorDatabase.at(_file.path().stem().string()).emplace_back(_data);
            }

            _in.close();
        }
    }

    return _allLoadedSuccessfully;
}
