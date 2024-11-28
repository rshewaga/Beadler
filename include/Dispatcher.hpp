#pragma once

#include <entt/signal/dispatcher.hpp>

/**
 * @brief Globally accessible singleton which holds the entt::dispatcher
 */
class Dispatcher
{
  public:
    /**
     * @brief Get the global instance, creating it if necessary
     * @return The Dispatcher singleton
     */
    static Dispatcher& Inst()
    {
        static Dispatcher inst;
        return inst;
    }

    /**
     * @brief Get the entt::dispatcher
     * @return The entt::dispatcher
     */
    static entt::dispatcher& Get()
    {
        return Inst().m_dispatcher;

        //Dispatcher::Get().sink<Event_Test>().connect<&App::TestEvent>(this);
        //Dispatcher::Get().trigger<Event_Test>(Event_Test{});
    }

  private:
    // Make constructor private. Only Inst() method will create an instance.
    Dispatcher() {}

    entt::dispatcher m_dispatcher;
};