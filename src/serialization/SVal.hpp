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
// File created on: 2017.04.08

// SVal.hpp


#pragma once

#include <utility>
#include <iostream>

class SVal
{
private:
	template<typename T>
	static constexpr bool is_derived = std::is_base_of<SVal, T>::value;

public:
	SVal();
	virtual ~SVal();

	SVal(SVal&& tmp);

//	template<class T, class=std::enable_if_t<is_derived<T>>>
//	SVal(T&& tmp)
//	{
//		std::cout << "SVal(&&:) ";
//		static_cast<SVal>(*this) = std::move(tmp);
//	}
//
//	template<class T, class=std::enable_if_t<is_derived<T>>>
//	T& operator=(T&& tmp)
//	{
//		std::cout << "SVal(=:) ";
//		return *this;
//	}
};
