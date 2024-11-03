#include <JobManager.hpp>

JobManager::JobManager()
{
    m_jobs.reserve(10);
    m_jobs.emplace_back(m_nextJobID++, "Test job 1");
    m_jobs.emplace_back(m_nextJobID++, "Test job 2");
    m_jobs.emplace_back(m_nextJobID++, "Test job 3");
}

std::shared_ptr<const Job> JobManager::GetJobID(int _jobID)
{
    for(const auto& _job : m_jobs)
    {
        if(_job.m_ID == _jobID)
        {
            return std::shared_ptr<const Job>(&_job);
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
    m_name = _name;
    m_startTime = std::chrono::system_clock::now();
}
