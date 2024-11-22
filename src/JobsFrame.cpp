#include <JobsFrame.hpp>

JobsFrame::JobsFrame(wxWindow* _parent, std::shared_ptr<JobManager> _jobManager) : wxFrame(_parent, wxID_ANY, "Jobs")
{
    SetDoubleBuffered(true);
    SetClientSize(720,480);
    Center();
    //Show();

    this->Bind(wxEVT_CLOSE_WINDOW, &JobsFrame::OnCloseWindow, this);

    m_jobManager = _jobManager;

    //m_mainSizer = std::make_shared<wxBoxSizer>(wxVERTICAL);
    //SetSizer(m_mainSizer.get());

    wxBoxSizer* testing = new wxBoxSizer(wxVERTICAL);
    SetSizer(testing);

	Layout();
	Centre(wxBOTH);

    auto _jobs = m_jobManager->GetJobs();
    for(int i = 0; i < _jobs->size(); ++i)
    {
        AddJobID((*_jobs)[i].m_ID);
    }
}

bool JobsFrame::AddJobID(int _jobID)
{
    (void)_jobID;

    std::shared_ptr<const Job> _job = m_jobManager->GetJobByID(_jobID);
    if(_job == nullptr)
    {
        return false;
    }

    wxStaticText* m_staticText1 = new wxStaticText(this, wxID_ANY, _job->m_name);
	//m_staticText1->Wrap(-1);
    GetSizer()->Add(m_staticText1, 0, wxALL, 5);

    Layout();

    return true;
}

void JobsFrame::OnCloseWindow(wxCloseEvent& _event)
{
    // Catch the wxEVT_CLOSE_WINDOW event to prevent the frame from deleting.
    // Just hide it instead.
    (void)_event;

    Show(false);
}
