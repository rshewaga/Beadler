#pragma once

#include <vector>

#include <Dispatcher.hpp>
#include <Job.hpp>
#include <JobEvents.hpp>

/**
 * @brief Manages parallel jobs starting, polling, and stopping
 */
class JobManager
{
public:
    JobManager();

    const Job* GetJobByID(int _jobID);
    const std::vector<Job>* GetJobs();

    void CreateTestJobs();

private:
    std::vector<Job> m_jobs;    // All jobs that have been created since program start
    int m_nextJobID = 1;
};