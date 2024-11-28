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
    void SetState(STATE _state)
    {
        m_state = _state;
        Dispatcher::Get().trigger<Event_JobStateChanged>(Event_JobStateChanged(m_ID));
    }

    /**
     * @brief Update the progress and notify
     * @param _progress The new completion progress [0-100]
     */
    void SetProgress(float _progress)
    {
        m_progress = std::clamp<float>(_progress, 0.0f, 100.0f);
        Dispatcher::Get().trigger<Event_JobProgressChanged>(Event_JobProgressChanged(m_ID));
    }
};