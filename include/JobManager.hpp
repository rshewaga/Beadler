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
    /**
     * @brief Get the global instance, creating it if necessary
     * @return The JobManager singleton
     */
    static JobManager& Inst()
    {
        static JobManager inst;
        return inst;
    }

    const Job* GetJobByID(int _jobID);
    const std::vector<Job>* GetJobs();

    void CreateTestJobs();

    /**
     * @brief Start a job with the given function and arguments
     * @param f The function to run in the thread
     * @param args The function arguments to pass
     * @return The created job's ID
     */
    template<class F, class... Args>
    int CreateJob(const std::string& _name, F&& f, Args&&... args)
    {
        m_jobs.emplace_back(m_nextJobID, _name);
        Dispatcher::Get().trigger<Event_JobAdded>(Event_JobAdded(m_nextJobID));

        m_jobs.back().Begin(f, args...);

        m_nextJobID++;
        return m_nextJobID - 1;
    }

private:
    // Make constructor private. Only Inst() method will create an instance.
    JobManager();
    
    std::vector<Job> m_jobs;    // All jobs that have been created since program start
    int m_nextJobID = 1;
};