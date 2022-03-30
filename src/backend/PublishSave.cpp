#include "PublishSave.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	PublishSave::PublishSave(String id)
	{
		auto &b = Backend::Ref();
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << SCHEME SERVER "/Browse/View.json?ID=" << format::URLEncode(id.ToUtf8()) << "&Key=" << b.User().antiCsrf.ToUtf8();
			request->uri = uriBuilder.Build();
			request->postFields["ActionPublish"] = "bagels";
		}
		b.AddAuthHeaders(*request);
	}

	common::Task::Status PublishSave::Process()
	{
		return PreprocessResponse(responseCheckJson);
	}
}
