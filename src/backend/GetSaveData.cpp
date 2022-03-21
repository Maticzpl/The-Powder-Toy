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

	bool GetSaveData::Process()
	{
		if (!PreprocessResponse(responseCheckBasic))
		{
			return false;
		}
		if (!request->responseBody.size())
		{
			error = "Save data empty";
			shortError = "no data";
			return false;
		}
		data.assign(request->responseBody.begin(), request->responseBody.end());
		return true;
	}
}
