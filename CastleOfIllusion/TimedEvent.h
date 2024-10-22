#ifndef _TIMED_EVENT_INCLUDE
#define _TIMED_EVENT_INCLUDE

#include <functional>
#include <type_traits>
#include <memory>
#include <list>

// Represents an event that will execute when a certain amount of time (in milliseconds) has passed from its creation
// Move-only
class TimedEvent 
{
public:
	template <typename Callable, typename = std::enable_if< std::is_invocable_v<Callable>> >
	TimedEvent(int target_time, Callable&& c)
		: m_callable(std::forward<Callable>(c)), m_target_time(target_time) { }

	TimedEvent(TimedEvent const& other) = delete;
	TimedEvent(TimedEvent&& other) noexcept
		: m_callable(std::move(other.m_callable)), m_target_time(other.m_target_time), m_counter(other.m_counter) {}

	TimedEvent& operator= (TimedEvent const& other) = delete;
	TimedEvent& operator= (TimedEvent&& other) noexcept
	{
		m_counter = other.m_counter;
		m_callable = other.m_callable;
		m_callable = std::move(other.m_callable);
		return *this;
	}

	// Returns true and calls the callable if the time specified at object construction has passed
	// Returns false otherwise
	bool update(int delta_time) 
	{
		m_counter += delta_time;
		if (m_counter >= m_target_time) 
		{
			m_callable();
			return true;
		}

		return false;
	}

private:
	// The piece of code that will be called after the specified time
	std::function<void()> m_callable;

	// The target time to wait for
	int m_target_time;

	// The internal counter
	int m_counter = 0;
};

class TimedEvents
{
public:
	// Pushes an event to the system
	static void pushEvent(std::unique_ptr<TimedEvent>&& event)
	{
		auto& m_events = instance().m_events;
		m_events.emplace_back(std::move(event));
	}

	// Ticks all events, deleting fulfilled ones
	static void updateEvents(int delta_time)
	{
		auto& m_events = instance().m_events;

		auto it = m_events.begin();
		while (it != m_events.end()) 
		{
			if ((*it)->update(delta_time))
			{
				if (it != m_events.begin())
				{
					auto it2 = it;
					it--;
					m_events.erase(it2);
					it++;
				}
				else 
				{
					auto it2 = it;
					m_events.erase(it2);
					it = m_events.begin();
				}
			}
			else
				it++;
		}
	}

private:
	static TimedEvents& instance() 
	{
		static TimedEvents instance;
		return instance;
	}
	TimedEvents() = default;

	// Stores the events
	std::list<std::unique_ptr<TimedEvent>> m_events;
};

#endif // _TIMED_EVENT_INCLUDE

