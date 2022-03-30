#include "BackendTask.h"

#include "network/Request.h"
#include "backend/Backend.h"

#include <string.h>

namespace backend
{
	static String StatusText(int ret);

	BackendTask::BackendTask()
	{
		request = std::make_shared<network::Request>();
	}

	void BackendTask::Start(std::shared_ptr<common::Task> self)
	{
		backend::Backend::Ref().Dispatch(std::static_pointer_cast<BackendTask>(self));
	}

	common::Task::Status BackendTask::PreprocessResponse(ResponseCheckMode responseCheckMode)
	{
		if (request->responseCode == 200 && !request->responseBody.size())
		{
			// * No server response, malformed response.
			// * TODO-REDO_UI: Ask jacob about this.
			request->responseCode = 603;
		}
		if (request->responseCode == 302)
		{
			// * TODO-REDO_UI: Ask jacob about this.
			return { true };
		}
		if (request->responseCode == 200)
		{
			switch (responseCheckMode)
			{
			case responseCheckJson:
				try
				{
					std::istringstream datastream(request->responseBody);
					datastream >> root;
					// * Everything is fine if an array is returned.
					if (root.type() == Json::arrayValue)
					{
						return { true };
					}
					int status = root.get("Status", 1).asInt();
					if (status != 1)
					{
						return { false, "failed", ByteString(root.get("Error", "Unspecified Error").asString()).FromUtf8() };
					}
				}
				catch (const std::exception &e)
				{
					// * Sometimes the server returns a 200 with the text "Error: 401" or similar.
					if (!memcmp(&request->responseBody[0], "Error: ", 7))
					{
						return { false, "invalid JSON", "Could not read response: " + ByteString(e.what()).FromUtf8() };
					}
					request->responseCode = ByteString(request->responseBody.begin() + 7, request->responseBody.end()).ToNumber<int>(true);
				}
				break;

			case responseCheckOk:
				if (request->responseBody.Substr(0, 2) != ByteString("OK"))
				{
					return { false, "failed", request->responseBody.FromUtf8() };
				}
				break;

			default:
				break;
			}
		}
		if (request->responseCode != 200)
		{
			return { false, String::Build("HTTP ", request->responseCode), String::Build("HTTP Error ", request->responseCode, ": ", StatusText(request->responseCode)) };
		}
		return { true };
	}

	static String StatusText(int ret)
	{
		switch (ret)
		{
		case 0:   return "Status code 0 (bug?)";
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 102: return "Processing";
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 207: return "Multi-Status";
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 306: return "Switch Proxy";
		case 307: return "Temporary Redirect";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Request Entity Too Large";
		case 414: return "Request URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Requested Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 418: return "I'm a teapot";
		case 422: return "Unprocessable Entity";
		case 423: return "Locked";
		case 424: return "Failed Dependency";
		case 425: return "Unordered Collection";
		case 426: return "Upgrade Required";
		case 444: return "No Response";
		case 450: return "Blocked by Windows Parental Controls";
		case 499: return "Client Closed Request";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 509: return "Bandwidth Limit Exceeded";
		case 510: return "Not Extended";
		case 600: return "Internal Client Error";
		case 601: return "Unsupported Protocol";
		case 602: return "Server Not Found";
		case 603: return "Malformed Response";
		case 604: return "Network Not Available";
		case 605: return "Request Timed Out";
		case 606: return "Malformed URL";
		case 607: return "Connection Refused";
		case 608: return "Proxy Server Not Found";
		case 609: return "SSL: Invalid Certificate Status";
		case 610: return "Cancelled by Shutdown";
		case 611: return "Too Many Redirects";
		case 612: return "SSL: Connect Error";
		case 613: return "SSL: Crypto Engine Not Found";
		case 614: return "SSL: Failed to Set Default Crypto Engine";
		case 615: return "SSL: Local Certificate Issue";
		case 616: return "SSL: Unable to Use Specified Cipher";
		case 617: return "SSL: Failed to Initialise Crypto Engine";
		case 618: return "SSL: Failed to Load CACERT File";
		case 619: return "SSL: Failed to Load CRL File";
		case 620: return "SSL: Issuer Check Failed";
		case 621: return "SSL: Pinned Public Key Mismatch";
		default: break;;
		}
		return "Unknown Status Code";
	}
}
