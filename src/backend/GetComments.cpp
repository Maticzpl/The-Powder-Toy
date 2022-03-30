#include "GetComments.h"

#include "network/Request.h"
#include "Format.h"

namespace backend
{
	GetComments::GetComments(SaveIDV idv, int start, int count)
	{
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << SCHEME SERVER "/Browse/Comments.json?ID=" << format::URLEncode(idv.id.ToUtf8()) << "&Start=" << start << "&Count=" << count;
			request->uri = uriBuilder.Build();
		}
	}

	common::Task::Status GetComments::Process()
	{
		auto prep = PreprocessResponse(responseCheckJson);
		if (!prep)
		{
			return prep;
		}
		try
		{
			for (auto &sub : root)
			{
				comments.push_back(CommentInfo{
					UserInfo{
						UserInfo::detailLevelBasic,
						ByteString(sub["UserID"].asString()).FromUtf8().ToNumber<uint64_t>(), // * A string for some reason???
						ByteString(sub["Username"].asString()).FromUtf8(),
					},
					ByteString(sub["Gravatar"].asString()).FromUtf8(),
					ByteString(sub["Text"].asString()).FromUtf8(),
					time_t(ByteString(sub["Timestamp"].asString()).FromUtf8().ToNumber<uint64_t>()), // * A string for some reason???
					ByteString(sub["CommentID"].asString()).FromUtf8().ToNumber<uint64_t>(), // * A string for some reason???
				});
				if (sub.isMember("Elevation"))
				{
					comments.back().user.elevation = UserInfo::ElevationFromString(ByteString(sub["Elevation"].asString()).FromUtf8());
				}
			}
		}
		catch (const std::exception &e) // * TODO-REDO_UI: Stupid, should be an exception specific to our json lib.
		{
			return { false, "invalid JSON", "Could not read response: " + ByteString(e.what()).FromUtf8() };
		}
		return { true };
	}
}
