#include <JobManager.hpp>

JobManager::JobManager()
{
    m_jobs.reserve(10);
    m_jobs.emplace_back(m_nextJobID++, "Test job 1");
    m_jobs.emplace_back(m_nextJobID++, "Test job 2");
    m_jobs.emplace_back(m_nextJobID++, "Test job 3");
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

Job::Job(int _ID, const std::string &_name)
{
    m_ID = _ID;
    m_progress = 0;
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

