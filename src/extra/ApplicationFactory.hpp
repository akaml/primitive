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
// File created on: 2017.07.02

// ApplicationFactory.hpp


#pragma once


#include "Application.hpp"

#include <map>

class ApplicationFactory
{
private:
	ApplicationFactory() {};

	virtual ~ApplicationFactory() {};

	ApplicationFactory(ApplicationFactory const&) = delete;

	void operator=(ApplicationFactory const&) = delete;

	static ApplicationFactory& getInstance()
	{
		static ApplicationFactory instance;
		return instance;
	}

	std::map<std::string, std::shared_ptr<Application>(*)(const Setting&)> _creators;

public:
	static bool reg(const std::string& type, std::shared_ptr<Application>(*)(const Setting&));
	static std::shared_ptr<Application> create(const Setting& setting);
};