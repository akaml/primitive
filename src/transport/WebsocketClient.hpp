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
// File created on: 2017.10.06

// WebsocketClient.hpp


#pragma once


#include "ClientTransport.hpp"

class WebsocketClient : public ClientTransport
{
private:
	std::shared_ptr<Transport::Handler> _handler;

public:
	WebsocketClient(const WebsocketClient&) = delete;
	void operator=(WebsocketClient const&) = delete;
	WebsocketClient(WebsocketClient&&) = delete;
	WebsocketClient& operator=(WebsocketClient&&) = delete;

	WebsocketClient(const std::shared_ptr<Handler>& handler)
	: _handler(handler)
	{

		_log.setName("WebsocketClient");
		_log.debug("Transport '%s' created", name().c_str());
	}

	~WebsocketClient() override
	{
		_log.debug("Transport '%s' destroyed", name().c_str());
	}

	bool processing(const std::shared_ptr<Connection>& connection) override;
};
