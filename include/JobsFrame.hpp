#pragma once

#include <wx/wx.h>
#include <JobManager.hpp>

/**
 * @brief Window to display parallel jobs
 */
class JobsFrame : public wxFrame
{
public:
    JobsFrame(wxWindow* _parent, std::shared_ptr<JobManager> _jobManager);

private:
    /**
     * @brief Adds a job to the displayed list
     * @param _jobID ID of the job to add
     * @return Whether the job was found and added
     */
    bool AddJobID(int _jobID);

    std::shared_ptr<JobManager> m_jobManager;   // Global JobManager to watch and display

    std::shared_ptr<wxBoxSizer> m_mainSizer;    // Vertical list of jobs
};