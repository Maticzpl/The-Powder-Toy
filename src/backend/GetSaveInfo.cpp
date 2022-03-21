#include "GetSaveInfo.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	GetSaveInfo::GetSaveInfo(SaveIDV idv)
	{
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << SCHEME SERVER "/Browse/View.json?ID=" << format::URLEncode(idv.id.ToUtf8());
			if (idv.version)
			{
				uriBuilder << "&Date=" << idv.version;
			}
			request->uri = uriBuilder.Build();
		}
		Backend::Ref().AddAuthHeaders(*request);
	}

	bool GetSaveInfo::Process()
	{
		if (!PreprocessResponse(responseCheckJson))
		{
			return false;
		}
		try
		{
			info = SaveInfo{
				SaveInfo::detailLevelExtended,
				SaveIDV{
					ByteString(root["ID"].asString()).FromUtf8(),
					root["Version"].asInt64(),
				},
				time_t(root["DateCreated"].asInt64()),
				time_t(root["Date"].asInt64()),
				root["ScoreUp"].asInt(),
				root["ScoreDown"].asInt(),
				ByteString(root["Username"].asString()).FromUtf8(),
				ByteString(root["Name"].asString()).FromUtf8(),
				root["Published"].asBool(),
				root["ScoreMine"].asInt(),
				ByteString(root["Description"].asString()).FromUtf8(),
				root["Favourite"].asBool(),
				root["Comments"].asInt(),
				root["Views"].asInt(),
			};
			for (auto &tag : root["Tags"])
			{
				info.tags.insert(ByteString(tag.asString()).FromUtf8());
			}
		}
		catch (const std::exception &e) // * TODO-REDO_UI: Stupid, should be an exception specific to our json lib.
		{
			error = "Could not read response: " + ByteString(e.what()).FromUtf8();
			shortError = "invalid JSON";
			return false;
		}
		return true;
	}
}
