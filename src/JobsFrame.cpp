#include <JobsFrame.hpp>

#include <wx/splitter.h>

enum COLUMNS
{
    ID,
    PROGRESS,
    STATE,
    DESCRIPTION
};

JobsFrame::JobsFrame(wxWindow* _parent) : wxFrame(_parent, wxID_ANY, "Jobs")
{
    Dispatcher::Get().sink<Event_JobAdded>().connect<&JobsFrame::OnJobAdded>(this);
    Dispatcher::Get().sink<Event_JobStateChanged>().connect<&JobsFrame::OnJobStateChanged>(this);
    Dispatcher::Get().sink<Event_JobProgressChanged>().connect<&JobsFrame::OnJobProgressChanged>(this);
    this->Bind(wxEVT_CLOSE_WINDOW, &JobsFrame::OnCloseWindow, this);

    SetupWidgets();
}

void JobsFrame::SetupWidgets()
{
    SetDoubleBuffered(true);
    SetClientSize(720,480);
    //================
    //this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* jobsSizer;
	jobsSizer = new wxBoxSizer( wxHORIZONTAL );

	auto m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	//m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( Jobs::m_splitter1OnIdle ), NULL, this );
	m_splitter1->SetMinimumPaneSize( 10 );

	auto m_scrolledWindow3 = new wxScrolledWindow( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow3->SetScrollRate( 5, 5 );
	wxBoxSizer* scrolledSizer;
	scrolledSizer = new wxBoxSizer( wxVERTICAL );

	m_dataViewListCtrl = std::make_shared<wxDataViewListCtrl>(m_scrolledWindow3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES);
	m_dataViewListCtrl->AppendTextColumn( _("ID"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_REORDERABLE|wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListCtrl->AppendProgressColumn( _("Progress"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_REORDERABLE|wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListCtrl->AppendTextColumn( _("State"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_REORDERABLE|wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	m_dataViewListCtrl->AppendTextColumn( _("Description"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_REORDERABLE|wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE );
	scrolledSizer->Add(m_dataViewListCtrl.get(), 1, wxALL|wxEXPAND, 0 );


	m_scrolledWindow3->SetSizer( scrolledSizer );
	m_scrolledWindow3->Layout();
	scrolledSizer->Fit( m_scrolledWindow3 );
	auto controlPanel = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_splitter1->SplitHorizontally( m_scrolledWindow3, controlPanel, 148 );
	jobsSizer->Add( m_splitter1, 1, wxEXPAND, 5 );


	this->SetSizer( jobsSizer );
	this->Layout();

	this->Centre( wxBOTH );
}

bool JobsFrame::AddJobID(int _jobID)
{
    const Job* _job = JobManager::Inst().GetJobByID(_jobID);
    if(_job == nullptr)
    {
        return false;
    }

    wxVector<wxVariant> _data;
    _data.push_back(wxString(std::to_string(_job->m_ID)));
    _data.push_back(static_cast<int>(_job->m_progress));
    _data.push_back(wxString(Job::to_string(_job->m_state)));
    _data.push_back(wxString(_job->m_name));
    m_dataViewListCtrl->AppendItem(_data);

    m_jobIDToRow[_job->m_ID] = m_dataViewListCtrl->GetItemCount() - 1;

    //Layout();

    return true;
}

void JobsFrame::OnCloseWindow(wxCloseEvent& _event)
{
    // Catch the wxEVT_CLOSE_WINDOW event to prevent the frame from deleting.
    // Just hide it instead.
    (void)_event;

    Show(false);
}

void JobsFrame::OnJobAdded(const Event_JobAdded& _event)
{
    AddJobID(_event.m_jobID);
}

void JobsFrame::OnJobStateChanged(const Event_JobStateChanged &_event)
{
    const Job* _job = JobManager::Inst().GetJobByID(_event.m_jobID);

    m_dataViewListCtrl->SetTextValue(Job::to_string(_job->m_state), m_jobIDToRow[_job->m_ID], COLUMNS::STATE);
    m_dataViewListCtrl->SetValue(static_cast<int>(_job->m_progress), m_jobIDToRow[_job->m_ID], COLUMNS::PROGRESS);
}

void JobsFrame::OnJobProgressChanged(const Event_JobProgressChanged &_event)
{
    const Job* _job = JobManager::Inst().GetJobByID(_event.m_jobID);

    m_dataViewListCtrl->SetValue(static_cast<int>(_job->m_progress), m_jobIDToRow[_job->m_ID], COLUMNS::PROGRESS);
}
