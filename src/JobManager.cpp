#include <JobManager.hpp>

#include <fmt/format.h>

JobManager::JobManager()
{
    m_jobs.reserve(10);
}

const Job* JobManager::GetJobByID(int _jobID)
{
    for(const auto& _job : m_jobs)
    {
        if(_job.m_ID == _jobID)
        {
            return &_job;
        }
    }

    return nullptr;
}

const std::vector<Job>* JobManager::GetJobs()
{
    return &m_jobs;
}

void Thing3(Job* _job)
{
    for(float i = 1; i <= 100; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        _job->SetProgress(i);
    }

    _job->m_endTime = std::chrono::system_clock::now();
    _job->SetState(Job::STATE::FINISHED);
}

void JobManager::CreateTestJobs()
{
    for(int i = 0; i < 3; ++i)
    {
        m_jobs.emplace_back(m_nextJobID, fmt::format("Test job ID {}", m_nextJobID));
        Dispatcher::Get().trigger<Event_JobAdded>(Event_JobAdded(m_nextJobID));

        m_jobs[i].Begin(&Thing3, &(m_jobs[i]));
        
        m_nextJobID++;
    }
}

Job::Job(int _ID, const std::string &_name)
{
    m_ID = _ID;
    m_progress = 50.0f;
    m_state = Job::STATE::NOT_STARTED;
    m_name = _name;
    m_startTime = std::chrono::system_clock::now();
}

std::string Job::to_string(Job::STATE _state)
{
    switch(_state)
    {
        case Job::STATE::NOT_STARTED:
            return "not started";
        case Job::STATE::WORKING:
            return "working";
        case Job::STATE::FAILED:
            return "failed";
        case Job::STATE::FINISHED:
            return "finished";
        case Job::STATE::CANCELLED:
            return "cancelled";
        default:
            return "ERROR: Conversion from Job::STATE to string not provided!";
    }
}

