#pragma once

#include <string>
#include <thread>
#include <chrono>
#include <algorithm>

#include <JobEvents.hpp>

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
        WAITING,
        FAILED,
        FINISHED,
        CANCELLED
    };

    Job(int _ID, const std::string& _name);

    int m_ID;                   // Unique job ID
    float m_progress;           // 0 - 100
    std::string m_name;         // Short name of the job
    std::string m_description;  // Long description
    STATE m_state;              // Current state

    std::jthread m_thread;

    std::chrono::time_point<std::chrono::system_clock> m_startTime; // Timestamp when job was started
    std::chrono::time_point<std::chrono::system_clock> m_endTime;   // Timestamp when job was ended

    static std::string to_string(Job::STATE _state);

    /**
     * @brief Start the thread with the given function and arguments
     * @param f The function to run in the thread
     * @param args The function arguments to pass
     */
    template<class F, class... Args>
    void Begin(F&& f, Args&&... args)
    {
        m_thread = std::jthread(f, args...);

        m_startTime = std::chrono::system_clock::now();

        SetState(Job::STATE::WORKING);
        SetProgress(0.0f);
    }

    /**
     * @brief Update the state and notify
     * @param _state The new state
     */
    void SetState(STATE _state);

    /**
     * @brief Update the progress and notify
     * @param _progress The new completion progress [0-100]
     */
    void SetProgress(float _progress);

    /**
     * @brief End the job with the given state. Does not stop the thread! Use RequestStop instead.
     * @param _endState Ideally CANCELLED, FINISHED, or FAILED
     */
    void End(STATE _endState);

    /**
     * @brief Trigger the thread's stop source to internally handle how to stop
     * @return Whether the stop request was successfully made
     */
    bool RequestStop();
};