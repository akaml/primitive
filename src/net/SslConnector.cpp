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
// File created on: 2017.06.06

// SslConnector.cpp


#include <openssl/ossl_typ.h>
#include "SslConnector.hpp"
#include "SslConnection.hpp"
#include "ConnectionManager.hpp"

SslConnector::SslConnector(
	const std::shared_ptr<ClientTransport>& transport,
	const std::string& host,
	std::uint16_t port,
	const std::shared_ptr<SSL_CTX>& context
)
: TcpConnector(transport, host, port)
, _sslContext(context)
{
}

void SslConnector::createConnection(int sock, const sockaddr_in &cliaddr)
{
	std::shared_ptr<Transport> transport = _transport.lock();
	if (!transport)
	{
		return;
	}

	auto newConnection = std::shared_ptr<TcpConnection>(new SslConnection(transport, sock, cliaddr, _sslContext, true));

	newConnection->setTtl(std::chrono::seconds(15));

	onConnect(newConnection);

	ConnectionManager::remove(ptr());
	ConnectionManager::add(newConnection);
}
