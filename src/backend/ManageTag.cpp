#include "ManageTag.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	ManageTag::ManageTag(String id, String tag, bool add)
	{
		auto &b = Backend::Ref();
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << SCHEME SERVER "/Browse/EditTag.json?ID=" << format::URLEncode(id.ToUtf8()) << "&Tag=" << tag.ToUtf8() << "&Key=" << b.User().antiCsrf.ToUtf8();
			if (add)
			{
				uriBuilder << "&Op=add";
			}
			else
			{
				uriBuilder << "&Op=delete";
			}
			request->uri = uriBuilder.Build();
		}
		b.AddAuthHeaders(*request);
	}

	common::Task::Status ManageTag::Process()
	{
		auto prep = PreprocessResponse(responseCheckJson);
		if (!prep)
		{
			return prep;
		}
		try
		{
			for (auto &tag : root["Tags"])
			{
				tags.insert(ByteString(tag.asString()).FromUtf8());
			}
		}
		catch (const std::exception &e) // * TODO-REDO_UI: Stupid, should be an exception specific to our json lib.
		{
			return { false, "invalid JSON", "Could not read response: " + ByteString(e.what()).FromUtf8() };
		}
		return { true };
	}
}
