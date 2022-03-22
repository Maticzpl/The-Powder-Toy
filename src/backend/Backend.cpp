#include "Backend.h"

#include "network/Request.h"
#include "prefs/GlobalPrefs.h"

#include <iostream>

namespace backend
{
	Backend::Backend()
	{
		auto &gpref = prefs::GlobalPrefs::Ref();
		user.id = gpref.Get("User.ID", UINT64_C(0));
		user.name = gpref.Get("User.Username", String(""));
		user.session = gpref.Get("User.SessionID", String(""));
		user.antiCsrf = gpref.Get("User.SessionKey", String(""));
		user.elevation = UserInfo::ElevationFromString(gpref.Get("User.Elevation", String("")));
		if (user.id)
		{
			user.detailLevel = UserInfo::detailLevelSession;
		}
	}

	void Backend::User(UserInfo newUser)
	{
		assert(newUser.detailLevel == UserInfo::detailLevelNone || newUser.detailLevel == UserInfo::detailLevelSession);
		user = newUser;
		auto &gpref = prefs::GlobalPrefs::Ref();
		if (newUser.detailLevel == UserInfo::detailLevelNone)
		{
			gpref.Clear("User");
		}
		else
		{
			prefs::Prefs::DeferWrite dw(gpref);
			gpref.Set("User.ID", user.id);
			gpref.Set("User.Username", user.name);
			gpref.Set("User.SessionID", user.session);
			gpref.Set("User.SessionKey", user.antiCsrf);
			switch (user.elevation)
			{
			case UserInfo::elevationAdmin:
				gpref.Set("User.Elevation", String("Admin"));
				break;

			case UserInfo::elevationStaff:
				gpref.Set("User.Elevation", String("Mod"));
				break;

			default:
				gpref.Clear("User.Elevation");
				break;
			}
		}
	}

	void Backend::Tick()
	{
		auto it = tasks.begin();
		while (it != tasks.end())
		{
			auto ptr = it->lock();
			if (ptr)
			{
				if (ptr->request->status == network::Request::statusDone)
				{
					ptr->status = ptr->Process();
					ptr->complete = true;
				}
				if (ptr->request->total > 0)
				{
					ptr->progressIndeterminate = false;
					ptr->progress = ptr->request->done / float(ptr->request->total);
				}
				else
				{
					ptr->progressIndeterminate = true;
				}
			}
			else
			{
				std::cerr << "Backend: task abandoned" << std::endl;
			}
			if (!ptr || ptr->request->status == network::Request::statusDone)
			{
				it = tasks.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Backend::AddAuthHeaders(network::Request &request) const
	{
		if (user.detailLevel)
		{
			if (user.session.size())
			{
				request.headers["X-Auth-User-Id"] = ByteString::Build(user.id);
				request.headers["X-Auth-Session-Key"] = user.session.ToUtf8();
			}
			else
			{
				// * TODO-REDO_UI: Figure out if this is needed.
				request.headers["X-Auth-User"] = ByteString::Build(user.id);
			}
		}
	}

	void Backend::Dispatch(std::shared_ptr<BackendTask> task)
	{
		tasks.push_back(std::weak_ptr(task));
		task->request->Dispatch();
	}
}
