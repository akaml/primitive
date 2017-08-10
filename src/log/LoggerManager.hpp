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
// File created on: 2017.04.21

// LoggerManager.hpp


#pragma once

#include <P7_Client.h>
#include <P7_Trace.h>
#include <mutex>
#include "Log.hpp"

class LoggerManager final
{
public:
	LoggerManager(const LoggerManager&) = delete;
	void operator=(LoggerManager const&) = delete;
	LoggerManager(LoggerManager&& tmp) = delete;
	LoggerManager& operator=(LoggerManager&& tmp) = delete;

private:
	LoggerManager();
	~LoggerManager();

	static LoggerManager &getInstance()
	{
		static LoggerManager instance;
		return instance;
	}

	Log::Detail _defaultLogLevel;

	std::recursive_mutex _mutex;

	IP7_Client *_logClient;
	IP7_Trace *_logTrace;

public:
	static bool enabled;

	static bool regThread(std::string threadName);
	static bool unregThread();

	static void setDefaultLogLevel(Log::Detail level)
	{
		getInstance()._defaultLogLevel = level;
	};

	static Log::Detail defaultLogLevel()
	{
		return getInstance()._defaultLogLevel;
	}

	static IP7_Trace *getLogTrace();
};
