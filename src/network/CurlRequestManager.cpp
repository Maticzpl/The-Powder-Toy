#include "RequestManager.h"

#include "Request.h"
#include "common/tpt-compat.h"
#include "CurlAssert.h"

#include <algorithm>
#include <iostream>
#include <cassert>

#if defined(CURL_AT_LEAST_VERSION) && CURL_AT_LEAST_VERSION(7, 55, 0)
# define REQUEST_USE_CURL_OFFSET_T
#endif
#if defined(CURL_AT_LEAST_VERSION) && CURL_AT_LEAST_VERSION(7, 56, 0)
# define REQUEST_USE_CURL_MIMEPOST
#endif
#if defined(CURL_AT_LEAST_VERSION) && CURL_AT_LEAST_VERSION(7, 61, 0)
# define REQUEST_USE_CURL_TLSV13CL
#endif

namespace network
{
	constexpr long maxConnections = 6;

	const char userAgent[] =
		"PowderToy/"
		MTOS(SAVE_VERSION) "."
		MTOS(MINOR_VERSION) " "
		"("
			IDENT_PLATFORM "; "
			IDENT_BUILD "; "
			"M" MTOS(MOD_ID) "; "
			IDENT
		") "
		"TPTPP/"
		MTOS(SAVE_VERSION) "."
		MTOS(MINOR_VERSION) "."
		MTOS(BUILD_NUM)
		IDENT_RELTYPE "."
		MTOS(SNAPSHOT_ID);

	struct RequestInfo
	{
		char errorBuffer[CURL_ERROR_SIZE];
		Request *request;
		CURL *easy;
		struct curl_slist *headers = NULL;
#ifdef REQUEST_USE_CURL_MIMEPOST
		curl_mime *postFields = NULL;
#else
		curl_httppost *postFieldsFirst = NULL;
		curl_httppost *postFieldsLast = NULL;
#endif
	};

	void SetupCurlEasyCiphers(CURL *easy)
	{
#ifdef SECURE_CIPHERS_ONLY
		curl_version_info_data *version_info = curl_version_info(CURLVERSION_NOW);
		ByteString ssl_type = version_info->ssl_version;
		if (ssl_type.Contains("OpenSSL"))
		{
			CURLEASSERTOK(curl_easy_setopt(
				easy,
				CURLOPT_SSL_CIPHER_LIST,
				"ECDHE-ECDSA-AES256-GCM-SHA384" ":"
				"ECDHE-ECDSA-AES128-GCM-SHA256" ":"
				"ECDHE-ECDSA-AES256-SHA384"     ":"
				"DHE-RSA-AES256-GCM-SHA384"     ":"
				"ECDHE-RSA-AES256-GCM-SHA384"   ":"
				"ECDHE-RSA-AES128-GCM-SHA256"   ":"
				"ECDHE-ECDSA-AES128-SHA"        ":"
				"ECDHE-ECDSA-AES128-SHA256"     ":"
				"ECDHE-RSA-CHACHA20-POLY1305"   ":"
				"ECDHE-RSA-AES256-SHA384"       ":"
				"ECDHE-RSA-AES128-SHA256"       ":"
				"ECDHE-ECDSA-CHACHA20-POLY1305" ":"
				"ECDHE-ECDSA-AES256-SHA"        ":"
				"ECDHE-RSA-AES128-SHA"          ":"
				"DHE-RSA-AES128-GCM-SHA256"
			));
# ifdef REQUEST_USE_CURL_TLSV13CL
			CURLEASSERTOK(curl_easy_setopt(
				easy,
				CURLOPT_TLS13_CIPHERS,
				"TLS_AES_256_GCM_SHA384"       ":"
				"TLS_CHACHA20_POLY1305_SHA256" ":"
				"TLS_AES_128_GCM_SHA256"       ":"
				"TLS_AES_128_CCM_8_SHA256"     ":"
				"TLS_AES_128_CCM_SHA256"
			));
# endif
		}
		else if (ssl_type.Contains("Schannel"))
		{
			// TODO: add more cipher algorithms
			CURLEASSERTOK(curl_easy_setopt(easy, CURLOPT_SSL_CIPHER_LIST, "CALG_ECDH_EPHEM"));
		}
#endif
		// TODO: Find out what TLS1.2 is supported on, might need to also allow TLS1.0
		CURLEASSERTOK(curl_easy_setopt(easy, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2));
#if defined(CURL_AT_LEAST_VERSION) && CURL_AT_LEAST_VERSION(7, 70, 0)
		CURLEASSERTOK(curl_easy_setopt(easy, CURLOPT_SSL_OPTIONS, CURLSSLOPT_REVOKE_BEST_EFFORT));
#elif defined(CURL_AT_LEAST_VERSION) && CURL_AT_LEAST_VERSION(7, 44, 0)
		CURLEASSERTOK(curl_easy_setopt(easy, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE));
#elif defined(WIN)
# error "That's unfortunate."
#endif
	}

	static size_t writeFunction(char *ptr, size_t size, size_t count, void *userdata)
	{
		Request *req = reinterpret_cast<Request *>(userdata);
		auto written = size * count;
		req->responseBody.append(ptr, written);
		return written;
	}

	RequestManager::RequestManager()
	{
		CURLEASSERTOK(curl_global_init(CURL_GLOBAL_DEFAULT));
		auto multi = CURLASSERTPTR(curl_multi_init());
		CURLMASSERTOK(curl_multi_setopt(multi, CURLMOPT_MAX_HOST_CONNECTIONS, maxConnections));
		opaque = reinterpret_cast<void *>(multi);
	}

	RequestManager::~RequestManager()
	{
		auto *multi = reinterpret_cast<CURLM *>(opaque);
		assert(!requests.size());
		CURLMASSERTOK(curl_multi_cleanup(multi));
		curl_global_cleanup();
	}

	void RequestManager::Tick()
	{
		auto *multi = reinterpret_cast<CURLM *>(opaque);
		int dontcare;
		CURLMASSERTOK(curl_multi_perform(multi, &dontcare));
		while (true)
		{
			struct CURLMsg *msg = curl_multi_info_read(multi, &dontcare);
			if (!msg)
			{
				break;
			}
			if (msg->msg == CURLMSG_DONE)
			{
				Request *request;
				CURLEASSERTOK(curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &request));
				int responseCode = 600;
				switch (msg->data.result)
				{
				case CURLE_OK:
					{
						long code;
						CURLEASSERTOK(curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &code));
						responseCode = int(code);
					}
					break;

				case CURLE_UNSUPPORTED_PROTOCOL:     responseCode = 601; break;
				case CURLE_COULDNT_RESOLVE_HOST:     responseCode = 602; break;
				case CURLE_OPERATION_TIMEDOUT:       responseCode = 605; break;
				case CURLE_URL_MALFORMAT:            responseCode = 606; break;
				case CURLE_COULDNT_CONNECT:          responseCode = 607; break;
				case CURLE_COULDNT_RESOLVE_PROXY:    responseCode = 608; break;
				case CURLE_SSL_INVALIDCERTSTATUS:    responseCode = 609; break;
				case CURLE_TOO_MANY_REDIRECTS:       responseCode = 611; break;
				case CURLE_SSL_CONNECT_ERROR:        responseCode = 612; break;
				case CURLE_SSL_ENGINE_NOTFOUND:      responseCode = 613; break;
				case CURLE_SSL_ENGINE_SETFAILED:     responseCode = 614; break;
				case CURLE_SSL_CERTPROBLEM:          responseCode = 615; break;
				case CURLE_SSL_CIPHER:               responseCode = 616; break;
				case CURLE_SSL_ENGINE_INITFAILED:    responseCode = 617; break;
				case CURLE_SSL_CACERT_BADFILE:       responseCode = 618; break;
				case CURLE_SSL_CRL_BADFILE:          responseCode = 619; break;
				case CURLE_SSL_ISSUER_ERROR:         responseCode = 620; break;
				case CURLE_SSL_PINNEDPUBKEYNOTMATCH: responseCode = 621; break;

				case CURLE_HTTP2:
				case CURLE_HTTP2_STREAM:
				case CURLE_FAILED_INIT:
				case CURLE_NOT_BUILT_IN:
				default:
					break;
				}
				if (responseCode >= 600)
				{
					auto it = std::find_if(requests.begin(), requests.end(), [request](std::unique_ptr<RequestInfo> &ri) {
						return ri->request == request;
					});
					std::cerr << &(*it)->errorBuffer[0] << std::endl;
				}
				request->responseCode = responseCode;
				request->status = Request::statusDone;
				Remove(request);
			}
		}
		for (auto &ri : requests)
		{
#ifdef REQUEST_USE_CURL_OFFSET_T
			curl_off_t totalDown, doneDown, totalUp, doneUp;
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &totalDown));
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_SIZE_DOWNLOAD_T, &doneDown));
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_CONTENT_LENGTH_UPLOAD_T, &totalUp));
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_SIZE_UPLOAD_T, &doneUp));
#else
			double totalDown, doneDown, totalUp, doneUp;
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &totalDown));
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_SIZE_DOWNLOAD, &doneDown));
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_CONTENT_LENGTH_UPLOAD, &totalUp));
			CURLEASSERTOK(curl_easy_getinfo(ri->easy, CURLINFO_SIZE_UPLOAD, &doneUp));
#endif
			ri->request->total = int(totalDown) + int(totalUp);
			ri->request->done = int(doneDown) + int(doneUp);
		}
	}

	void RequestManager::Add(Request *request)
	{
		if (disableNetwork)
		{
			request->responseCode = 622;
			request->status = Request::statusDone;
			return;
		}
		auto *multi = reinterpret_cast<CURLM *>(opaque);
		auto it = std::find_if(requests.begin(), requests.end(), [request](std::unique_ptr<RequestInfo> &ri) {
			return ri->request == request;
		});
		if (it == requests.end())
		{
			requests.emplace_back(std::make_unique<RequestInfo>());
			auto &ri = requests.back();
			ri->request = request;
			ri->easy = CURLASSERTPTR(curl_easy_init());
			for (auto header : request->headers)
			{
				ri->headers = CURLASSERTPTR(curl_slist_append(ri->headers, (header.first + ": " + header.second).c_str()));
			}
			if (!request->postFields.empty())
			{
#ifdef REQUEST_USE_CURL_MIMEPOST
				ri->postFields = curl_mime_init(ri->easy);
				assert(ri->postFields);
				for (auto &field : request->postFields)
				{
					curl_mimepart *part = CURLASSERTPTR(curl_mime_addpart(ri->postFields));
					CURLEASSERTOK(curl_mime_data(part, &field.second[0], field.second.size()));
					if (auto split = field.first.SplitBy(':'))
					{
						CURLEASSERTOK(curl_mime_name(part, split.Before().c_str()));
						CURLEASSERTOK(curl_mime_filename(part, split.After().c_str()));
					}
					else
					{
						CURLEASSERTOK(curl_mime_name(part, field.first.c_str()));
					}
				}
				CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_MIMEPOST, ri->postFields));
#else
				for (auto &field : request->postFields)
				{
					if (auto split = field.first.SplitBy(':'))
					{
						CURLFORMASSERTOK(curl_formadd(&ri->postFieldsFirst, &ri->postFieldsLast,
							CURLFORM_COPYNAME, split.Before().c_str(),
							CURLFORM_BUFFER, split.After().c_str(),
							CURLFORM_BUFFERPTR, &field.second[0],
							CURLFORM_BUFFERLENGTH, field.second.size(),
						CURLFORM_END));
					}
					else
					{
						CURLFORMASSERTOK(curl_formadd(&ri->postFieldsFirst, &ri->postFieldsLast,
							CURLFORM_COPYNAME, field.first.c_str(),
							CURLFORM_PTRCONTENTS, &field.second[0],
							CURLFORM_CONTENTLEN, field.second.size(),
						CURLFORM_END));
					}
				}
				CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_HTTPPOST, ri->postFieldsFirst));
#endif
			}
			else if (request->post)
			{
				CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_POST, 1L));
				CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_POSTFIELDS, ""));
			}
			else
			{
				CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_HTTPGET, 1L));
			}
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_FOLLOWLOCATION, 1L));
#ifdef ENFORCE_HTTPS
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTPS));
#else
			CURLEASSERTOK(curl_easy_setopt(easy, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS | CURLPROTO_HTTP));
			CURLEASSERTOK(curl_easy_setopt(easy, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTPS | CURLPROTO_HTTP));
#endif
			SetupCurlEasyCiphers(ri->easy);
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_MAXREDIRS, 10L));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_ERRORBUFFER, &ri->errorBuffer[0]));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_CONNECTTIMEOUT, long(timeout)));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_HTTPHEADER, ri->headers));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_URL, request->uri.c_str()));
			if (proxy.size())
			{
				CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_PROXY, proxy.c_str()));
			}
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_PRIVATE, (void *)request));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_USERAGENT, userAgent));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_WRITEDATA, (void *)request));
			CURLEASSERTOK(curl_easy_setopt(ri->easy, CURLOPT_WRITEFUNCTION, writeFunction));
			CURLMASSERTOK(curl_multi_add_handle(multi, ri->easy));
		}
	}

	void RequestManager::Remove(Request *request)
	{
		auto *multi = reinterpret_cast<CURLM *>(opaque);
		auto it = std::find_if(requests.begin(), requests.end(), [request](std::unique_ptr<RequestInfo> &ri) {
			return ri->request == request;
		});
		if (it != requests.end())
		{
			CURLMASSERTOK(curl_multi_remove_handle(multi, (*it)->easy));
			curl_easy_cleanup((*it)->easy);
#ifdef REQUEST_USE_CURL_MIMEPOST
			curl_mime_free((*it)->postFields);
#else
			curl_formfree((*it)->postFieldsFirst);
#endif
			curl_slist_free_all((*it)->headers);
			if (requests.end() - it > 1)
			{
				std::swap(*it, requests.back());
			}
			requests.pop_back();
		}
	}
}
