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
// File created on: 2017.03.29

// WsContext.hpp


#pragma once

#include "../../utils/Context.hpp"
#include "../http/HttpRequest.hpp"
#include "WsFrame.hpp"
#include <memory>

class WsContext: public Context
{
private:
	HttpRequest::Ptr _request;
	bool _established;
	WsFrame::Ptr _frame;

public:
	WsContext(): _established(false) {};
	virtual ~WsContext() {};

	void setEstablished()
	{
		_established = true;
		_frame.reset();
	}
	bool established()
	{
		return _established;
	}

	void setRequest(HttpRequest::Ptr& request)
	{
		_request = request;
	}
	HttpRequest::Ptr& getRequest()
	{
		return _request;
	}

	void setFrame(WsFrame::Ptr& frame)
	{
		_frame = frame;
	}
	WsFrame::Ptr& getFrame()
	{
		return _frame;
	}
};