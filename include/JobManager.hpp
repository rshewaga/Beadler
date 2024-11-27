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
    enum STATE   // Various states the job can be in
    {
        NOT_STARTED,
        WORKING,
        FAILED,
        FINISHED,
        CANCELLED
    };

    Job(int _ID, const std::string& _name);

    int m_ID;                   // Unique job ID
    float m_progress;           // 0 - 100
    std::string m_name;         // Short name of the job
    std::string m_description;  // Long description
    STATE m_state;

    std::jthread m_thread;

    std::chrono::time_point<std::chrono::system_clock> m_startTime;
    std::chrono::time_point<std::chrono::system_clock> m_endTime;

    static std::string to_string(Job::STATE _state);
};


/**
 * @brief Manages parallel jobs starting, polling, and stopping
 */
class JobManager
{
public:
    JobManager();

    const Job* GetJobByID(int _jobID);
    const std::vector<Job>* GetJobs();

private:
    std::vector<Job> m_jobs;    // All jobs that have been created since program start
    int m_nextJobID = 1;
};