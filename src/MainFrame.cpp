#include <MainFrame.hpp>
#include <wx/wfstream.h>
#include <wx/dcbuffer.h>
#include <wx/splitter.h>

MainFrame::MainFrame(const std::string& _title) : wxFrame(nullptr, wxID_ANY, _title)
{
    this->SetDoubleBuffered(true);
    //this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxInitAllImageHandlers();
    
    AddMenuBar();
    AddWidgets();
    //wxStatusBar* statusBar = CreateStatusBar();
    //statusBar->SetDoubleBuffered(true);

    jobsFrame = std::make_shared<JobsFrame>(this);
    JobManager::Inst().CreateTestJobs();

    scrolledWindow->LoadSprite("C:/Proj/cpp/Beadler/assets/largeSparseTest10Colours.png");
}

void MainFrame::AddWidgets()
{
    // Note that wxWidgets takes care of destroying all heap objects created here,
    // and creating these objects on the heap is the recommended/necessary method!
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxStatusBar* m_statusBar1 = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
    (void)m_statusBar1;
	wxBoxSizer* MainSizer;
	MainSizer = new wxBoxSizer( wxHORIZONTAL );

	wxPanel* LeftPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	LeftPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	LeftPanel->SetMinSize( wxSize( 200,200 ) );

	MainSizer->Add( LeftPanel, 0, wxALL|wxEXPAND, 5 );

	scrolledWindow = std::make_shared<ScrolledWindow>(this);
	//scrolledWindow->SetScrollRate( 5, 5 );
	wxGridSizer* CanvasSizer;
	CanvasSizer = new wxGridSizer( 0, 1, 0, 0 );

	spritePanel = std::make_shared<SpritePanel>(scrolledWindow.get());//, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    scrolledWindow->SetChildSpritePanel(spritePanel);
	spritePanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW ) );
	//spritePanel->SetMinSize( wxSize( 300,300 ) );

	CanvasSizer->Add(spritePanel.get(), 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );


	scrolledWindow->SetSizer( CanvasSizer );
	scrolledWindow->Layout();
	CanvasSizer->Fit( scrolledWindow.get() );
	MainSizer->Add( scrolledWindow.get(), 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( MainSizer );
	this->Layout();

	this->Centre( wxBOTH );
}

void MainFrame::AddMenuBar()
{
    menuBar = std::make_shared<wxMenuBar>();
    this->SetMenuBar(menuBar.get());

    wxMenu* menuFile = new wxMenu();
    menuFile->Bind(wxEVT_MENU, &MainFrame::OnMenuFile, this);
    menuFile->Append(ID_MENU_FILE::NEW, "New", "Clear the project");
    menuFile->Append(ID_MENU_FILE::OPEN, "Open...", "Open an image");    
    menuFile->Append(ID_MENU_FILE::EXPORT, "Export...", "Export pattern as .PDF");
    menuFile->Append(ID_MENU_FILE::EXIT, "Exit", "Bye!");
    menuBar->Append(menuFile, "File");

    wxMenu* menuWindows = new wxMenu();
    menuWindows->Bind(wxEVT_MENU, &MainFrame::OnMenuWindows, this);
    menuWindows->Append(ID_MENU_WINDOWS::JOBS, "Jobs", "View background jobs");
    menuBar->Append(menuWindows, "Windows");
}

void MainFrame::OnLoadImageButton(wxCommandEvent& _event)
{
    (void)_event;
    OpenFilePicker();
}

void MainFrame::OnMenuFile(wxCommandEvent &_event)
{
    switch(_event.GetId())
    {
        case ID_MENU_FILE::NEW:
            wxLogError("New");
            break;
        case ID_MENU_FILE::OPEN:
            OpenFilePicker();
            break;
        case ID_MENU_FILE::EXPORT:
            wxLogError("EXPORT");
            break;
        case ID_MENU_FILE::EXIT:
            Close();
            break;
    }
}

void MainFrame::OnMenuWindows(wxCommandEvent &_event)
{
    switch(_event.GetId())
    {
        case ID_MENU_WINDOWS::JOBS:
            jobsFrame->Show();
            break;
    }
}

bool MainFrame::OpenFilePicker()
{
    wxFileDialog openFileDialog(this, "Open image file", "", "", "*.png;*.jpg;*.jpeg;*.bmp;*.gif", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    const int pickerResult = openFileDialog.ShowModal();
    if (pickerResult == wxID_CANCEL)
    {
        //wxLogError("Closed file picker");
        return false;
    }

    //wxLogStatus("Selected file '%s'.", openFileDialog.GetPath());

    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
        return false;
    }

    scrolledWindow->LoadSprite(openFileDialog.GetPath().ToStdString());

    this->Refresh();
    return true;
}