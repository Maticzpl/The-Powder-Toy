#include "DeleteSave.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	DeleteSave::DeleteSave(String id, bool unpublishOnly)
	{
		auto &b = Backend::Ref();
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << SCHEME SERVER "/Browse/Delete.json?ID=" << format::URLEncode(id.ToUtf8()) << "&Mode=";
			if (unpublishOnly)
			{
				uriBuilder << "Unpublish";
			}
			else
			{
				uriBuilder << "Delete";
			}
			uriBuilder << "&Key=" << b.User().antiCsrf.ToUtf8();
			request->uri = uriBuilder.Build();
		}
		b.AddAuthHeaders(*request);
	}

	bool DeleteSave::Process()
	{
		return PreprocessResponse(responseCheckJson);
	}
}
