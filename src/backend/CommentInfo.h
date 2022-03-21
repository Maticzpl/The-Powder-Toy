#pragma once
#include "Config.h"

#include "UserInfo.h"

#include <ctime>
#include <cstdint>

namespace backend
{
	struct CommentInfo
	{
		UserInfo user;
		String userAvatar;
		String text;
		time_t timestamp;
		uint64_t id;
	};
}
