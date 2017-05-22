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

// main.cpp


#include <iostream>
#include "../src/thread/ThreadPool.hpp"
#include "../src/options/Options.hpp"
#include "../src/configs/Config.hpp"
#include "../src/server/Server.hpp"
#include "../src/utils/Daemon.hpp"
#include "../src/utils/ShutdownManager.hpp"

int main(int argc, char *argv[])
{
	Log log("Main");

	log.info("Start daemon");

	SetProcessName();
	StartManageSignals();

	auto options = std::make_shared<Options>(argc, argv);

	auto configs = std::make_shared<Config>(options);

	auto server = std::make_shared<Server>(configs);

//	SetDaemonMode();

	server->start();

	ShutdownManager::doAtShutdown([server](){
		server->stop();
	});

	server->wait();

	log.info("Stop daemon");

	P7_Exceptional_Flush();

	return 0;
}
