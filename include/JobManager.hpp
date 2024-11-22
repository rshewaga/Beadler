#pragma once

#include <thread>
#include <string>
#include <chrono>
#include <vector>

/**
 * @brief Encapsulates a single parallel job
 */
class Job
{
public:
    Job(int _ID, const std::string& _name);

    int m_ID;   // Unique job ID
    std::string m_name;
    //std::jthread m_thread;

    std::chrono::time_point<std::chrono::system_clock> m_startTime;
    std::chrono::time_point<std::chrono::system_clock> m_endTime;
    
};

/**
 * @brief Manages parallel jobs starting, polling, and stopping
 */
class JobManager
{
public:
    JobManager();

    std::shared_ptr<const Job> GetJobByID(int _jobID);
    const std::vector<Job>* GetJobs();

private:
    std::vector<Job> m_jobs;    // All jobs that have been created since program start
    int m_nextJobID = 1;
};