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
    //AddWidgets();
    //wxStatusBar* statusBar = CreateStatusBar();
    //statusBar->SetDoubleBuffered(true);

    Mk2();
    
    //TestWidgets();
}

void MainFrame::TestWidgets()
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxMenuBar* m_menubar1 = new wxMenuBar( 0 );
	wxMenu* m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("New") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );

	m_menubar1->Append( m_menu1, _("File") );

	this->SetMenuBar( m_menubar1 );

	wxStatusBar* m_statusBar1 = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
    (void)m_statusBar1;
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	wxSplitterWindow* hSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	hSplitter->SetSashGravity( 0 );

	wxPanel* leftPanel = new wxPanel( hSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxPanel* rightPanel = new wxPanel( hSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	wxButton* m_button21 = new wxButton( rightPanel, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button21, 1, wxALL|wxEXPAND, 5 );


	rightPanel->SetSizer( bSizer8 );
	rightPanel->Layout();
	bSizer8->Fit( rightPanel );
	hSplitter->SplitVertically( leftPanel, rightPanel, 250 );
	bSizer6->Add( hSplitter, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer6 );
	this->Layout();

	this->Centre( wxBOTH );
}

void MainFrame::AddWidgets()
{
    // Note that wxWidgets takes care of destroying all heap objects created here,
    // and creating these objects on the heap is the recommended/necessary method!

    mainPanel = std::make_shared<wxPanel>(this);
    //mainPanel->Bind(wxEVT_PAINT, &MainFrame::OnPaint, this);
    //mainPanel->Bind(wxEVT_ERASE_BACKGROUND, &MainFrame::OnEraseBG, this);
    //wxEVT_LEFT_DOWN, wxEVT_LEFT_DOWN, wxEVT_MIDDLE_DOWN, wxEVT_LEFT_DCLICK
    //mainPanel->Bind(wxEVT_MOTION, &MainFrame::OnMouseEvent, this);
    mainPanel->SetDoubleBuffered(true);
    //mainPanel->SetMinSize(wxSize(400,400));

    // Left panel
    wxPanel* leftPanel = new wxPanel(mainPanel.get());
    wxButton* button = new wxButton(leftPanel, wxID_ANY, "Load image", wxDefaultPosition, wxDefaultSize, wxEXPAND);//, wxPoint(100,100));
    button->Bind(wxEVT_BUTTON, &MainFrame::OnLoadImageButton, this);

    //this->Bind(wxEVT_MOTION, &MainFrame::OnMouseEvent, this);
    
    spriteScrolledPanel = std::make_shared<SpriteScrolledPanel>(mainPanel.get());
    spriteScrolledPanel->LoadSprite("X:/Beads/Clean/Abysswolf - X - FFXIII - Lightning.png");
    spriteScrolledPanel->SetMinSize({100,100});

    wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
    hSizer->Add(leftPanel, wxSizerFlags().Proportion(0).Expand());
    hSizer->Add(spriteScrolledPanel.get(), wxSizerFlags().Proportion(1).Expand().Border(wxALL, 25));

    mainPanel->SetSizer(hSizer);
    hSizer->SetSizeHints(this);
    //mainPanel->Lower();
}

void MainFrame::Mk2()
{
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

    scrolledWindow->LoadSprite("X:/Beads/Clean/Abysswolf - X - FFXIII - Lightning.png");
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

void MainFrame::OpenFilePicker()
{
    wxFileDialog openFileDialog(mainPanel.get(), "Open image file", "", "", "*.png;*.jpg;*.jpeg;*.bmp;*.gif", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    const int pickerResult = openFileDialog.ShowModal();
    if (pickerResult == wxID_CANCEL)
    {
        //wxLogError("Closed file picker");
        return;
    }

    //wxLogStatus("Selected file '%s'.", openFileDialog.GetPath());

    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
    }

    spritePanel->LoadSprite(openFileDialog.GetPath().ToStdString());

    this->Refresh();
}