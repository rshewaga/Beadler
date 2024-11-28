#pragma once

/**
 * @brief JobManager added a new job
 */
class Event_JobAdded
{
public:
    Event_JobAdded(int _jobID)
    {
        m_jobID = _jobID;
    }

    int m_jobID;
};

/**
 * @brief A Job's state has changed
 */
class Event_JobStateChanged
{
public:
    Event_JobStateChanged(int _jobID)
    {
        m_jobID = _jobID;
    }

    int m_jobID;
};

/**
 * @brief A Job's progress has changed
 */
class Event_JobProgressChanged
{
public:
    Event_JobProgressChanged(int _jobID)
    {
        m_jobID = _jobID;
    }

    int m_jobID;
};