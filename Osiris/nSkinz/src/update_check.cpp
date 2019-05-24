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
#include "update_check.hpp"
#include "Utilities/platform.hpp"
#include "SDK/declarations.hpp"

#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace cx
{
	constexpr auto cx_strcmp_part(const char* const a, const char* const b) -> bool
	{
		return *b ? (*a == *b && cx_strcmp_part(a + 1, b + 1)) : true;
	}

	constexpr auto month_str_to_int(const char* const str) -> unsigned
	{
		return
			cx_strcmp_part(str, "Jan") ? 1 :
			cx_strcmp_part(str, "Feb") ? 2 :
			cx_strcmp_part(str, "Mar") ? 3 :
			cx_strcmp_part(str, "Apr") ? 4 :
			cx_strcmp_part(str, "May") ? 5 :
			cx_strcmp_part(str, "Jun") ? 6 :
			cx_strcmp_part(str, "Jul") ? 7 :
			cx_strcmp_part(str, "Aug") ? 8 :
			cx_strcmp_part(str, "Sep") ? 9 :
			cx_strcmp_part(str, "Oct") ? 10 :
			cx_strcmp_part(str, "Nov") ? 11 : 12;
	}

	constexpr auto char_to_int(const unsigned char c) -> unsigned
	{
		return (c == ' ' ? '0' : c) - '0';
	}

	constexpr auto twochar_to_int(const char* const str) -> unsigned
	{
		return (char_to_int(str[0])) * 10 + char_to_int(str[1]);
	}

	constexpr auto year_str_to_int(const char* const str) -> unsigned
	{
		return (char_to_int(str[0])) * 1000 + (char_to_int(str[1])) * 100 + (char_to_int(str[2])) * 10 + char_to_int(str[3]);
	}

	constexpr auto days_from_civil(const int y, const int m, const int d) -> unsigned
	{
		const auto yc = y - (m <= 2 ? 1 : 0);
		const auto era = (yc >= 0 ? yc : yc - 399) / 400;
		const auto yoe = unsigned(yc - era * 400);									// [0, 399]
		const auto doy = unsigned((153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1);	// [0, 365]
		const auto doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;						// [0, 146096]
		return era * 146097 + int(doe) - 719468;
	}

	constexpr auto date_str_to_days(const char* const str) -> unsigned
	{
		return days_from_civil(year_str_to_int(str + 7), month_str_to_int(str), twochar_to_int(str + 4));
	}

	constexpr auto time_str_to_second(const char* const str) -> unsigned
	{
		return (twochar_to_int(str) * 60 + twochar_to_int(str + 3)) * 60 + twochar_to_int(str + 6);
	}

	constexpr auto unix_time() -> std::uint64_t
	{
		return std::uint64_t(date_str_to_days(__DATE__)) * 24 * 60 * 60 + time_str_to_second(__TIME__);
	}
}

using HTTPRequestHandle = std::uint32_t;
enum : HTTPRequestHandle { INVALID_HTTPREQUEST_HANDLE = 0 };

using SteamAPICall_t = std::uint64_t;
enum : SteamAPICall_t { k_uAPICallInvalid = 0 };

enum class EHTTPMethod
{
	Invalid = 0,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	OPTIONS,
	PATCH
};

class ISteamHTTP
{
public:
	virtual HTTPRequestHandle CreateHTTPRequest(EHTTPMethod eHTTPRequestMethod, const char* pchAbsoluteURL) = 0;
	virtual bool SetHTTPRequestContextValue(HTTPRequestHandle hRequest, std::uint64_t ulContextValue) = 0;
	virtual bool SetHTTPRequestNetworkActivityTimeout(HTTPRequestHandle hRequest, std::uint32_t unTimeoutSeconds) = 0;
	virtual bool SetHTTPRequestHeaderValue(HTTPRequestHandle hRequest, const char* pchHeaderName, const char* pchHeaderValue) = 0;
	virtual bool SetHTTPRequestGetOrPostParameter(HTTPRequestHandle hRequest, const char* pchParamName, const char* pchParamValue) = 0;
	virtual bool SendHTTPRequest(HTTPRequestHandle hRequest, SteamAPICall_t* pCallHandle) = 0;
	virtual bool SendHTTPRequestAndStreamResponse(HTTPRequestHandle hRequest, SteamAPICall_t* pCallHandle) = 0;
	virtual bool DeferHTTPRequest(HTTPRequestHandle hRequest) = 0;
	virtual bool PrioritizeHTTPRequest(HTTPRequestHandle hRequest) = 0;
	virtual bool GetHTTPResponseHeaderSize(HTTPRequestHandle hRequest, const char* pchHeaderName, std::uint32_t* unResponseHeaderSize) = 0;
	virtual bool GetHTTPResponseHeaderValue(HTTPRequestHandle hRequest, const char* pchHeaderName, std::uint8_t* pHeaderValueBuffer, std::uint32_t unBufferSize) = 0;
	virtual bool GetHTTPResponseBodySize(HTTPRequestHandle hRequest, std::uint32_t* unBodySize) = 0;
	virtual bool GetHTTPResponseBodyData(HTTPRequestHandle hRequest, std::uint8_t* pBodyDataBuffer, std::uint32_t unBufferSize) = 0;
	virtual bool GetHTTPStreamingResponseBodyData(HTTPRequestHandle hRequest, std::uint32_t cOffset, std::uint8_t* pBodyDataBuffer, std::uint32_t unBufferSize) = 0;
	virtual bool ReleaseHTTPRequest(HTTPRequestHandle hRequest) = 0;
	virtual bool GetHTTPDownloadProgressPct(HTTPRequestHandle hRequest, float* pflPercentOut) = 0;
	virtual bool SetHTTPRequestRawPostBody(HTTPRequestHandle hRequest, const char* pchContentType, std::uint8_t* pubBody, std::uint32_t unBodyLen) = 0;
};

ISteamHTTP* s_steam_http;

#pragma pack(push, 8)

struct HTTPRequestCompleted_t
{
	enum { k_iCallback = 2101 };
	HTTPRequestHandle	m_hRequest;
	std::uint64_t		m_ulContextValue;
	bool				m_bRequestSuccessful;
	int					m_eStatusCode;
	std::uint32_t		m_unBodySize;
};

#pragma pack(pop)

class VersionCheckCallback
{
public:
	virtual void Run(HTTPRequestCompleted_t *pvParam);
	virtual void Run(HTTPRequestCompleted_t *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall) { Run(pvParam); };
	virtual int GetCallbackSizeBytes() { return sizeof(HTTPRequestCompleted_t); }
protected:
	std::uint8_t m_nCallbackFlags = 0;
	int m_iCallback = HTTPRequestCompleted_t::k_iCallback;
};

static VersionCheckCallback s_check_callback;

bool g_update_needed = false;
std::vector<commit_entry> g_commits_since_compile;

void from_json(const json& j, commit_entry& commit)
{
	commit.author = j["commit"]["author"]["name"].get<std::string>();
	commit.date = j["commit"]["author"]["date"].get<std::string>();
	commit.message = j["commit"]["message"].get<std::string>();
}

void VersionCheckCallback::Run(HTTPRequestCompleted_t* pvParam)
{
	if(pvParam->m_eStatusCode == 200 && pvParam->m_unBodySize > 50)
	{

		std::string body;

		body.resize(pvParam->m_unBodySize + 1);

		const auto body_cstr = reinterpret_cast<std::uint8_t*>(const_cast<char*>(body.data()));

		s_steam_http->GetHTTPResponseBodyData(pvParam->m_hRequest, body_cstr, pvParam->m_unBodySize);

		body_cstr[pvParam->m_unBodySize] = 0;

		try
		{
			g_commits_since_compile = json::parse(body).get<std::vector<commit_entry>>();
			g_update_needed = true;
		}
		catch(const std::exception&)
		{
			// Do nothing. Something is malformed.
		}
	}

	s_steam_http->ReleaseHTTPRequest(pvParam->m_hRequest);
}

static auto SteamAPI_RegisterCallResult(VersionCheckCallback* callback, const SteamAPICall_t handle) -> void
{
	static auto addr = platform::get_export("steam_api.dll", "SteamAPI_RegisterCallResult");
	reinterpret_cast<void(__cdecl*)(VersionCheckCallback*, SteamAPICall_t)>(addr)(callback, handle);
}

static auto GetISteamHTTP(const char* version) -> ISteamHTTP*
{
	const auto steamclient_addr = platform::get_export("steam_api.dll", "SteamClient");
	const auto gethsteamuser_addr = platform::get_export("steam_api.dll", "SteamAPI_GetHSteamUser");
	const auto gethsteampipe_addr = platform::get_export("steam_api.dll", "SteamAPI_GetHSteamPipe");

	const auto thisptr = reinterpret_cast<void*(__cdecl*)()>(steamclient_addr)();
	const auto steamuser = reinterpret_cast<std::uint32_t(__cdecl*)()>(gethsteamuser_addr)();
	const auto steampipe = reinterpret_cast<std::uint32_t(__cdecl*)()>(gethsteampipe_addr)();

	const auto vfunc = get_vfunc<ISteamHTTP*(__thiscall*)(void*, std::uint32_t, std::uint32_t, const char*)>(thisptr, 23);

	return vfunc(thisptr, steamuser, steampipe, version);
}

auto run_update_check() -> void
{
	s_steam_http = GetISteamHTTP("STEAMHTTP_INTERFACE_VERSION002");

	const auto handle = s_steam_http->CreateHTTPRequest(EHTTPMethod::GET, "http://api.github.com/repos/namazso/nSkinz/commits");

	constexpr static auto build_time = std::integral_constant<time_t, time_t(cx::unix_time())>::value;

	tm build_time_tm;
	gmtime_s(&build_time_tm, &build_time);
	char build_time_str[32];
	strftime(build_time_str, sizeof build_time_str, "%FT%TZ", &build_time_tm);

	s_steam_http->SetHTTPRequestGetOrPostParameter(handle, "since", build_time_str);

	SteamAPICall_t api_call;
	s_steam_http->SendHTTPRequest(handle, &api_call);

	SteamAPI_RegisterCallResult(&s_check_callback, api_call);
}

