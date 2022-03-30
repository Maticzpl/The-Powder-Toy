#include "ExecVote.h"

#include "Backend.h"
#include "network/Request.h"

namespace backend
{
	ExecVote::ExecVote(SaveIDV idv, int direction)
	{
		auto &b = Backend::Ref();
		{
			request->uri = SCHEME SERVER "/Vote.api";
			request->postFields["ID"] = idv.id.ToUtf8();
			request->postFields["Action"] = direction == 1 ? "Up" : "Down";
		}
		b.AddAuthHeaders(*request);
	}

	common::Task::Status ExecVote::Process()
	{
		return PreprocessResponse(responseCheckOk);
	}
}
