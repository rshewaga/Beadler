#pragma once
#include <wx/wx.h>
#include <string>

#include <SpritePanel.hpp>
#include <ScrolledWindow.hpp>
#include <JobManager.hpp>
#include <JobsFrame.hpp>

class MainFrame : public wxFrame
{
public:
    MainFrame(const std::string& _title);
    
private:
    void AddMenuBar();
    void AddWidgets();

    /**
     * @brief Callback for the button to load a new image
     * @param _event
     */
    void OnLoadImageButton(wxCommandEvent& _event);

    /**
     * @brief Callback for the File menu items
     * @param _event
     */
    void OnMenuFile(wxCommandEvent& _event);

    /**
     * @brief Callback for the Windows menu items
     * @param _event
     */
    void OnMenuWindows(wxCommandEvent& _event);
    
    /**
     * @brief Open the file picker to select an image
     * @return Whether a valid file was picked
     */
    bool OpenFilePicker();

    std::shared_ptr<wxMenuBar> menuBar;

    std::shared_ptr<ScrolledWindow> scrolledWindow;
    std::shared_ptr<SpritePanel> spritePanel;

    std::shared_ptr<JobsFrame> jobsFrame;
    std::shared_ptr<JobManager> jobManager;

    // IDs for File menu bar items
    enum ID_MENU_FILE
    {
        NEW = 1000,
        OPEN,
        EXPORT,
        EXIT
    };

    // IDs for Windows menu bar items
    enum ID_MENU_WINDOWS
    {
        JOBS = 1100
    };
};