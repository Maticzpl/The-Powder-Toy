#include "PutSave.h"

#include "Backend.h"
#include "network/Request.h"

namespace backend
{
	PutSave::PutSave(String name, String description, bool publish, const std::vector<char> saveData)
	{
		auto &b = Backend::Ref();
		request->uri = SCHEME SERVER "/Save.api";
		request->postFields["Name"] = name.ToUtf8();
		request->postFields["Description"] = description.ToUtf8();
		request->postFields["Data:save.bin"] = ByteString(saveData.begin(), saveData.end());
		request->postFields["Publish"] = publish ? "Public" : "Private";
		b.AddAuthHeaders(*request);
	}

	common::Task::Status PutSave::Process()
	{
		auto prep = PreprocessResponse(responseCheckOk);
		if (!prep)
		{
			return prep;
		}
		if (request->responseBody.Substr(0, 3) != "OK ")
		{
			return { false, "invalid ID", "Server did not return an ID" };
		}
		id = request->responseBody.Substr(3).FromUtf8();
		return { true };
	}
}
