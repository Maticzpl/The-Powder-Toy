#pragma once

#include "common/Abort.h"

#include <curl/curl.h>

inline void CurlEAssertOk(CURLcode res, const char *where)
{
	if (res != CURLE_OK)
	{
		Abort(where, curl_easy_strerror(res));
	}
}

inline void CurlMAssertOk(CURLMcode res, const char *where)
{
	if (res != CURLM_OK)
	{
		Abort(where, curl_multi_strerror(res));
	}
}

inline void CurlFormAssertOk(CURLFORMcode res, const char *where)
{
	if (res != CURL_FORMADD_OK)
	{
		Abort(where, ("error code " + std::to_string(res)).c_str());
	}
}

template<class Type>
inline Type *CurlAssertPtr(Type *ptr, const char *where)
{
	if (!ptr)
	{
		Abort(where, "CURL says something is very wrong");
	}
	return ptr;
}

#define CURLEASSERTOK(x) CurlEAssertOk(x, __FILE__ ":" MTOS(__LINE__) ": " #x " failed: ")
#define CURLMASSERTOK(x) CurlMAssertOk(x, __FILE__ ":" MTOS(__LINE__) ": " #x " failed: ")
#define CURLFORMASSERTOK(x) CurlFormAssertOk(x, __FILE__ ":" MTOS(__LINE__) ": " #x " failed: ")
#define CURLASSERTPTR(x) CurlAssertPtr(x, __FILE__ ":" MTOS(__LINE__) ": " #x " failed: ")
