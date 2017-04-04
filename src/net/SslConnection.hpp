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
// File created on: 2017.02.25

// SslConnection.hpp


#pragma once

#include "Connection.hpp"

#include "../utils/Buffer.hpp"
#include "ReaderConnection.hpp"
#include "WriterConnection.hpp"
#include "TcpConnection.hpp"

#include <netinet/in.h>
#include <openssl/ssl.h>

/**
 * Подключение
 */
class SslConnection: public TcpConnection
{
private:
	std::string _name;

	std::shared_ptr<SSL_CTX> _sslContext;
	SSL *_sslConnect;

	bool _sslEstablished;

	bool readFromSocket() override;
	bool writeToSocket() override;

public:
	SslConnection() = delete;
	SslConnection(const SslConnection&) = delete;
	void operator= (SslConnection const&) = delete;

	SslConnection(Transport::Ptr& transport, int fd, const sockaddr_in& cliaddr, std::shared_ptr<SSL_CTX> sslContext);
	virtual ~SslConnection();

	virtual const std::string& name();

	virtual bool processing();

	static void InitSSL();
	static void DestroySSL();
};