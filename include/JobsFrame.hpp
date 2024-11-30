#pragma once

#include <unordered_map>

#include <JobManager.hpp>

#include <wx/wx.h>
#include <wx/dataview.h>

/**
 * @brief Window to display parallel jobs
 */
class JobsFrame : public wxFrame
{
public:
    JobsFrame(wxWindow* _parent);

private:
    /**
     * @brief Set up the wxWidgets for this frame
     */
    void SetupWidgets();

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

    /**
     * @brief A Job has a state change
     */
    void OnJobStateChanged(const Event_JobStateChanged& _event);
    
    /**
     * @brief A Job has a progress change
     */
    void OnJobProgressChanged(const Event_JobProgressChanged& _event);

    std::shared_ptr<wxDataViewListCtrl> m_dataViewListCtrl;

    std::unordered_map<int, int> m_jobIDToRow;  // Maps a job ID to the m_dataViewListCtrl's rows
};