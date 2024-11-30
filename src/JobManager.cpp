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

    _job->End(Job::STATE::FINISHED);
}

void JobManager::CreateTestJobs()
{
    for(int i = 0; i < 3; ++i)
    {
        m_jobs.emplace_back(m_nextJobID, fmt::format("Test job ID {}", m_nextJobID));
        Dispatcher::Get().trigger<Event_JobAdded>(Event_JobAdded(m_nextJobID));

        //m_jobs[i].Begin(&Thing3, &(m_jobs[i]));
        
        m_nextJobID++;
    }

    m_jobs[0].Begin(&Thing3, &(m_jobs[0]));
}
