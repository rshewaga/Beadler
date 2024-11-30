#include <Job.hpp>
#include <Dispatcher.hpp>

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
        case Job::STATE::WAITING:
            return "waiting";
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

void Job::SetState(STATE _state)
{
    m_state = _state;
    Dispatcher::Get().trigger<Event_JobStateChanged>(Event_JobStateChanged(m_ID));
}

void Job::SetProgress(float _progress)
{
    m_progress = std::clamp<float>(_progress, 0.0f, 100.0f);
    Dispatcher::Get().trigger<Event_JobProgressChanged>(Event_JobProgressChanged(m_ID));
}

void Job::End(STATE _endState)
{
    m_endTime = std::chrono::system_clock::now();

    SetState(_endState);
}