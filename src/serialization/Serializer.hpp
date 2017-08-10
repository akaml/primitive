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
// File created on: 2017.04.30

// Serializer.hpp


#pragma once

#include "SVal.hpp"

class Serializer
{
public:
	static const uint32_t STRICT = 1u<<0;

protected:
	uint32_t _flags;

public:
	Serializer() = delete;
	Serializer(const Serializer&) = delete;
	void operator=(Serializer const&) = delete;
	Serializer(Serializer&&) = delete;
	Serializer& operator=(Serializer&&) = delete;

	Serializer(uint32_t flags)
	: _flags(flags)
	{}
	virtual ~Serializer() = default;

	virtual SVal* decode(const std::string &data) = 0;
	virtual std::string encode(const SVal* value) = 0;
};

#include "SerializerFactory.hpp"

#define REGISTER_SERIALIZER(Type,Class) const bool Class::__dummy = \
    SerializerFactory::reg(                                                                     \
        #Type,                                                                                  \
        [](uint32_t flags){                                                                     \
            return std::shared_ptr<Serializer>(new Class(flags));                               \
        }                                                                                       \
    );

#define DECLARE_SERIALIZER(Class) \
public:                                                                                         \
	Class() = delete;                                                                           \
	Class(const Class&) = delete;                                                               \
    void operator=(Class const&) = delete;                                                      \
	Class(Class&&) = delete;                                                                    \
	Class& operator=(Class&&) = delete;                                                         \
                                                                                                \
private:                                                                                        \
    Class(uint32_t flags): Serializer(flags) {}                                                 \
                                                                                                \
public:                                                                                         \
    ~Class() override = default;                                                                \
                                                                                                \
	virtual SVal* decode(const std::string &data) override;                                     \
	virtual std::string encode(const SVal* value) override;                           	        \
                                                                                                \
private:                                                                                        \
    static const bool __dummy;
