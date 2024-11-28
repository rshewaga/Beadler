#pragma once

#include <JobManager.hpp>

#include <wx/wx.h>
#include <wx/dataview.h>

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

    /**
     * @brief wxEVT_CLOSE_WINDOW is caught so the frame isn't destroyed on X, it's just hidden
     * @param _event
     */
    void OnCloseWindow(wxCloseEvent& _event);

    /**
     * @brief JobManager added a new job
     */
    void OnJobAdded(const Event_JobAdded& _event);

    std::shared_ptr<JobManager> m_jobManager;   // Global JobManager to watch and display
    std::shared_ptr<wxDataViewListCtrl> m_dataViewListCtrl;
};