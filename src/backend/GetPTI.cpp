#include "GetPTI.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	GetPTI::GetPTI(String path)
	{
		request->uri = (STATICSCHEME STATICSERVER + path).ToUtf8();
	}

	common::Task::Status GetPTI::Process()
	{
		auto prep = PreprocessResponse(responseCheckBasic);
		if (!prep)
		{
			return prep;
		}
		try
		{
			image = gui::Image::FromPTI(&request->responseBody[0], request->responseBody.size());
		}
		catch (const std::exception &e)
		{
			return { false, "invalid PTI", "Could not read response: " + ByteString(e.what()).FromUtf8() };
		}
		return { true };
	}
}
