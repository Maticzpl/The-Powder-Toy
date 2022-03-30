#include "GetSaveData.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	GetSaveData::GetSaveData(SaveIDV idv)
	{
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << STATICSCHEME STATICSERVER "/" << format::URLEncode(idv.id.ToUtf8());
			if (idv.version)
			{
				uriBuilder << "_" << idv.version;
			}
			uriBuilder << ".cps";
			request->uri = uriBuilder.Build();
		}
	}

	common::Task::Status GetSaveData::Process()
	{
		auto prep = PreprocessResponse(responseCheckBasic);
		if (!prep)
		{
			return prep;
		}
		if (!request->responseBody.size())
		{
			return { false, "no data", "Save data empty" };
		}
		data.assign(request->responseBody.begin(), request->responseBody.end());
		return { true };
	}
}
