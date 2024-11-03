#include <JobsFrame.hpp>

JobsFrame::JobsFrame(wxWindow* _parent, std::shared_ptr<JobManager> _jobManager) : wxFrame(_parent, wxID_ANY, "Jobs")
{
    SetDoubleBuffered(true);
    SetClientSize(720,480);
    Center();
    Show();

    m_jobManager = _jobManager;

    m_mainSizer = std::make_shared<wxBoxSizer>(wxVERTICAL);
    SetSizer(m_mainSizer.get());

	Layout();
	Centre(wxBOTH);

    auto _jobs = m_jobManager->GetJobs();
    for(int i = 0; i < _jobs->size(); ++i)
        AddJobID((*_jobs)[0].m_ID);
}

bool JobsFrame::AddJobID(int _jobID)
{
    std::shared_ptr<const Job> _job = m_jobManager->GetJobID(_jobID);
    if(_job == nullptr)
    {
        return false;
    }

    wxStaticText* m_staticText1 = new wxStaticText(this, wxID_ANY, _job->m_name);
	m_staticText1->Wrap(-1);
	m_mainSizer->Add(m_staticText1, 0, wxALL, 5);

    return true;
}
