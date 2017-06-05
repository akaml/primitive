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
// File created on: 2017.06.05

// TcpConnector.hpp


#pragma once


#include "Connector.hpp"

#include <mutex>
#include <netdb.h>

struct sockaddr_in;

class TcpConnector : public Connector
{
protected:
	std::string _host;
	std::uint16_t _port;
	std::mutex _mutex;

private:
	char* _buff;
	size_t _buffSize;
	hostent _hostbuf;
	hostent* _hostptr;
	char **_addrIterator;
	sockaddr_in _sockaddr;

public:
	TcpConnector(std::shared_ptr<ClientTransport>& transport, std::string& host, std::uint16_t port);
	virtual ~TcpConnector();

	virtual void watch(epoll_event& ev);

	virtual void createConnection(int sock, const sockaddr_in& cliaddr);

	virtual bool processing();

	static std::shared_ptr<TcpConnector> create(std::shared_ptr<ClientTransport>& transport, std::string& host, std::uint16_t port)
	{
		return std::make_shared<TcpConnector>(transport, host, port);
	}
};
