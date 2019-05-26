/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "nSkinz.hpp"
#include "Hooks/hooks.hpp"
#include "kit_parser.hpp"
#include "config_.hpp"
#include "../Interfaces.h"
#include "../SDK/Engine.h"
#include "../SDK/EntityList.h"

sdk::CBaseClientState**		g_client_state;

void initializeNSkinz() noexcept
{
	//g_localize = get_interface<sdk::ILocalize>("localize.dll", ILOCALIZE_CLIENT_INTERFACE_VERSION);
	g_client_state = *reinterpret_cast<sdk::CBaseClientState***>(get_vfunc<std::uintptr_t>(interfaces.engine, 12) + 0x10);

	game_data::initialize_kits();
}
