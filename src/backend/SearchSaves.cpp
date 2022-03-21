#include "SearchSaves.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	SearchSaves::SearchSaves(String query, int start, int count, SearchDomain searchDomain, SearchSort searchSort)
	{
		auto &b = Backend::Ref();
		{
			ByteStringBuilder uriBuilder;
			uriBuilder << SCHEME SERVER "/Browse.json?Start=" << start << "&Count=" << count;
			if (searchDomain == searchDomainFavorites)
			{
				uriBuilder << "&Category=Favourites";
			}
			else if (searchDomain == searchDomainOwn && b.User().detailLevel)
			{
				uriBuilder << "&Category=by:" << format::URLEncode(b.User().name.ToUtf8());
			}
			{
				ByteStringBuilder queryBuilder;
				if (query.size())
				{
					queryBuilder << query.ToUtf8();
				}
				if (searchSort == searchSortByDate)
				{
					if (queryBuilder.Size())
					{
						queryBuilder << " ";
					}
					queryBuilder << "sort:date";
				}
				if (queryBuilder.Size())
				{
					uriBuilder << "&Search_Query=" << format::URLEncode(queryBuilder.Build());
				}
			}
			request->uri = uriBuilder.Build();
		}
		b.AddAuthHeaders(*request);
	}

	bool SearchSaves::Process()
	{
		if (!PreprocessResponse(responseCheckJson))
		{
			return false;
		}
		try
		{
			saveCount = root["Count"].asInt();
			for (auto &sub : root["Saves"])
			{
				saves.push_back(SaveInfo{
					SaveInfo::detailLevelBasic,
					SaveIDV{
						ByteString(sub["ID"].asString()).FromUtf8(),
						sub["Version"].asInt64(),
					},
					time_t(sub["Created"].asInt64()),
					time_t(sub["Updated"].asInt64()),
					sub["ScoreUp"].asInt(),
					sub["ScoreDown"].asInt(),
					ByteString(sub["Username"].asString()).FromUtf8(),
					ByteString(sub["Name"].asString()).FromUtf8(),
					sub["Published"].asBool(),
				});
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
