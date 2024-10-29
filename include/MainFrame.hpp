#pragma once
#include <wx/wx.h>
#include <string>

#include <SpritePanel.hpp>
#include <ScrolledWindow.hpp>

class MainFrame : public wxFrame
{
public:
    MainFrame(const std::string& _title);
    
private:
    void AddMenuBar();
    void AddWidgets();

    void OnLoadImageButton(wxCommandEvent& _event);
    void OnMenuFile(wxCommandEvent& _event);
    
    /**
     * @brief Open the file picker to select an image
     * @return Whether a valid file was picked
     */
    bool OpenFilePicker();

    std::shared_ptr<wxMenuBar> menuBar;

    std::shared_ptr<ScrolledWindow> scrolledWindow;
    std::shared_ptr<SpritePanel> spritePanel;

    // IDs for File menu bar items
    enum ID_MENU_FILE
    {
        NEW = 1000,
        OPEN,
        EXPORT,
        EXIT
    };
};