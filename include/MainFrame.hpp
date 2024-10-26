#pragma once
#include <wx/wx.h>
#include <string>

#include <SpritePanel.hpp>
#include <SpriteScrolledPanel.hpp>
#include <ScrolledWindow.hpp>

class MainFrame : public wxFrame
{
public:
    MainFrame(const std::string& _title);
    
private:
    void TestWidgets();
    void AddWidgets();
    void AddMenuBar();
    void Mk2();

    void OnLoadImageButton(wxCommandEvent& _event);
    void OnMenuFile(wxCommandEvent& _event);
    void OpenFilePicker();

    std::shared_ptr<wxMenuBar> menuBar;
    std::shared_ptr<wxPanel> mainPanel;

    std::shared_ptr<ScrolledWindow> scrolledWindow;
    std::shared_ptr<SpritePanel> spritePanel;
    std::shared_ptr<SpriteScrolledPanel> spriteScrolledPanel;

    // IDs for File menu bar items
    enum ID_MENU_FILE
    {
        NEW = 1000,
        OPEN,
        EXPORT,
        EXIT
    };
};