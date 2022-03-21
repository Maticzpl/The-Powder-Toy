#include "FavorSave.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	FavorSave::FavorSave(String id, bool favor)
	{
		auto &b = Backend::Ref();
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << SCHEME SERVER "/Browse/Favourite.json?ID=" << format::URLEncode(id.ToUtf8()) << "&Key=" << b.User().antiCsrf.ToUtf8();
			if (!favor)
			{
				uriBuilder << "&Mode=Remove";
			}
			request->uri = uriBuilder.Build();
		}
		b.AddAuthHeaders(*request);
	}

	bool FavorSave::Process()
	{
		return PreprocessResponse(responseCheckJson);
	}
}
