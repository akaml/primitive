// Copyright © 2017 Dmitriy Khaustov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2017.03.09

// Connection.hpp


#pragma once

#include <mutex>
#include <sys/epoll.h>
#include "../utils/Shareable.hpp"
#include "../utils/Named.hpp"
#include "../log/Log.hpp"
#include "../transport/Transport.hpp"
#include "../utils/Context.hpp"
#include "TimeoutWatcher.hpp"
#include "ConnectionEvent.hpp"
#include <unistd.h>

class Connection: public Shareable<Connection>, public Named
{
private:
	bool _captured;
	uint32_t _events;
	uint32_t _postponedEvents;

protected:
	Log _log;

	std::weak_ptr<Transport> _transport;
	std::shared_ptr<Context> _context;

	int _sock;

	/// Соединение готово
	bool _ready;

	/// Таймаут соединения
	bool _timeout;

	/// Соединение закрыто
	bool _closed;

	/// Ошибка соединения
	bool _error;

	// для обработки таймаутов
	mutable std::recursive_mutex _mutex;
	std::chrono::steady_clock::time_point _realExpireTime;
	std::chrono::steady_clock::time_point _nextExpireTime;
	std::shared_ptr<TimeoutWatcher> _timeoutWatcher;

public:
	Connection() = delete;
	Connection(const Connection&) = delete;
	void operator=(Connection const&) = delete;
	Connection(Connection&& tmp) = delete;
	Connection& operator=(Connection&& tmp) = delete;

	explicit Connection(const std::shared_ptr<Transport>& transport);
	~Connection() override;

	inline int fd() const
	{
		return _sock;
	}

	std::recursive_mutex& mutex() const
	{
		return _mutex;
	}
	void setTtl(std::chrono::milliseconds ttl);
	const bool expired() const
	{
		std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
		return _realExpireTime <= std::chrono::steady_clock::now();
	}
	const auto& expireTime() const
	{
		std::lock_guard<std::recursive_mutex> lockGuard(_mutex);
		return _realExpireTime;
	}

	std::shared_ptr<Context>& getContext()
	{
		return _context;
	}
	void setContext(const std::shared_ptr<Context>& context)
	{
		_context = context;
	}
	void resetContext()
	{
		_context.reset();
	}

	virtual bool isClosed() const
	{
		return _closed;
	}

	inline void setReleased()
	{
		_captured = false;
		_events = 0;
	}
	inline void setCaptured()
	{
		_captured = true;
	}
	inline bool isCaptured() const
	{
		return _captured;
	}

	uint32_t rotateEvents()
	{
		_events = _postponedEvents;
		_postponedEvents = 0;
		return _events;
	}

	void appendEvents(uint32_t events)
	{
		if (_captured)
		{
			_postponedEvents |= events;
		}
		else
		{
			_events |= events;
		}
	}

	inline bool isReadyForRead() const
	{
		return static_cast<bool>(_events & static_cast<uint32_t>(ConnectionEvent::READ));
	}

	inline bool isReadyForWrite() const
	{
		return static_cast<bool>(_events & static_cast<uint32_t>(ConnectionEvent::WRITE));
	}

	inline bool isHalfHup() const
	{
		return static_cast<bool>(_events & static_cast<uint32_t>(ConnectionEvent::HALFHUP));
	}

	inline bool isHup() const
	{
		return static_cast<bool>(_events & static_cast<uint32_t>(ConnectionEvent::HUP));
	}

	inline bool wasFailure() const
	{
		return static_cast<bool>(_events & static_cast<uint32_t>(ConnectionEvent::ERROR));
	}

	inline bool timeIsOut() const
	{
		return static_cast<bool>(_events & static_cast<uint32_t>(ConnectionEvent::TIMEOUT));
	}

	virtual void watch(epoll_event &ev) = 0;

	virtual bool processing() = 0;

	void close();
};
