// Copyright © 2017-2018 Dmitriy Khaustov
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
// File created on: 2017.02.25

// Config.hpp


#pragma once

#include <memory>
#include "Options.hpp"
#include <libconfig.h++>

class Config: public libconfig::Config
{
private:
	std::shared_ptr<Options> _options;

public:
	Config(const Config&) = delete;
	Config& operator=(Config const&) = delete;
	Config(Config&&) noexcept = delete;
	Config& operator=(Config&&) noexcept = delete;

	explicit Config(const std::shared_ptr<Options>& options);
	~Config() override = default;
};
