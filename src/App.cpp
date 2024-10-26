#include <App.hpp>
#include <MainFrame.hpp>

bool App::OnInit()
{
    MainFrame* mainFrame = new MainFrame("C++ GUI");
    mainFrame->SetClientSize(1280,720);
    mainFrame->Center();
    mainFrame->Show();
    return true;
}
