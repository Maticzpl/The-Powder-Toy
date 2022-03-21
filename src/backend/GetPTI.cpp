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

	bool GetPTI::Process()
	{
		if (!PreprocessResponse(responseCheckBasic))
		{
			return false;
		}
		try
		{
			image = gui::Image::FromPTI(&request->responseBody[0], request->responseBody.size());
		}
		catch (const std::exception &e)
		{
			error = "Could not read response: " + ByteString(e.what()).FromUtf8();
			shortError = "invalid PTI";
			return false;
		}
		return true;
	}
}
