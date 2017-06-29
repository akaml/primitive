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
// File created on: 2017.06.27

// TransportContext.hpp


#pragma once


#include "../utils/Context.hpp"
#include "ServerTransport.hpp"

class TransportContext : public Context
{
protected:
	ServerTransport::Handler _handler;
	ServerTransport::Transmitter _transmitter;

public:
	virtual ~TransportContext()	{};

	void setTransmitter(ServerTransport::Transmitter& transmitter)
	{
		_transmitter = transmitter;
	}

	void transmit(const char* data, size_t size, const std::string& type, bool close)
	{
		_transmitter(data, size, type, close);
	}

	void setHandler(ServerTransport::Handler handler)
	{
		_handler = handler;
	}

	void handle(const char* data, size_t size)
	{
		_handler(ptr(), data, size, _transmitter);
	}
};