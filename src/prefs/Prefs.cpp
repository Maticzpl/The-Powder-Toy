#include "Prefs.h"

#include "common/Platform.h"
#include "common/tpt-rand.h"

#include <fstream>
#include <iostream>

namespace prefs
{
	Prefs::Prefs(ByteString path) :
		prevPath(path + ".pref.prev"),
		currPath(path + ".pref"),
		nextPath(path + ".pref.next")
	{
		Read();
	}

	void Prefs::Read()
	{
		if (Platform::FileExists(nextPath) || Platform::FileExists(prevPath))
		{
			// * Something else is writing config, wait a bit. This should be very unlikely.
			Platform::Millisleep(500 + RNG::Ref().gen() % 500);
			if (Platform::FileExists(nextPath) || Platform::FileExists(prevPath))
			{
				// * At this point, we don't really have a choice.
				//   This will likely result in corruption if we're racing with something. This should be even more unlikely.
				std::cerr << "restoring stray backup config" << std::endl;
				Platform::RemoveFile(nextPath);
				Platform::RemoveFile(currPath);
				Platform::RenameFile(prevPath, currPath);
			}
		}

		auto ok = false;
		{
			std::ifstream f(currPath);
			try
			{
				if (f)
				{
					root.clear();
					f >> root;
					ok = true;
				}
			}
			catch (const std::exception &e)
			{
			}
		}
		if (!ok)
		{
			std::cerr << "failed to read " << currPath << std::endl;
		}
	}

	void Prefs::Write()
	{
		if (deferWriteLevel)
		{
			return;
		}

		if (Platform::FileExists(nextPath) || Platform::FileExists(prevPath))
		{
			// * Something else is writing config, wait a bit. This should be very unlikely.
			Platform::Millisleep(500 + RNG::Ref().gen() % 500);
			if (Platform::FileExists(nextPath) || Platform::FileExists(prevPath))
			{
				// * At this point, we don't really have a choice.
				//   This will likely result in corruption if we're racing with something. This should be even more unlikely.
				std::cerr << "deleting stray temporary config" << std::endl;
				Platform::RemoveFile(nextPath);
				Platform::RemoveFile(prevPath);
			}
		}

		auto ok = false;
		{
			std::ofstream f(nextPath, std::ios::trunc);
			try
			{
				if (f)
				{
					f << root;
				}
				if (f)
				{
					ok = true;
				}
			}
			catch (const std::exception &e)
			{
			}
		}
		if (!ok)
		{
			std::cerr << "failed to write " << currPath << std::endl;
		}
		Platform::RenameFile(currPath, prevPath);
		Platform::RenameFile(nextPath, currPath);
		Platform::RemoveFile(prevPath);
	}

	Json::Value Prefs::GetJson(const Json::Value &node, ByteString path)
	{
		if (node.type() != Json::objectValue)
		{
			return Json::nullValue;
		}
		auto split = path.SplitBy('.');
		if (!split)
		{
			return node[path];
		}
		return GetJson(node[split.Before()], split.After());
	}

	void Prefs::SetJson(Json::Value &node, ByteString path, Json::Value value)
	{
		if (node.type() != Json::objectValue)
		{
			node = Json::objectValue;
		}
		auto split = path.SplitBy('.');
		if (!split)
		{
			node[path] = value;
			return;
		}
		SetJson(node[split.Before()], split.After(), value);
	}

	template<>
	Json::Value Prefs::Pack<int>(const int &value)
	{
		return Json::Value(value);
	}

	template<>
	Json::Value Prefs::Pack<uint64_t>(const uint64_t &value)
	{
		return Json::Value(Json::UInt64(value));
	}

	template<>
	Json::Value Prefs::Pack<float>(const float &value)
	{
		return Json::Value(value);
	}

	template<>
	Json::Value Prefs::Pack<bool>(const bool &value)
	{
		return Json::Value(value);
	}

	template<>
	Json::Value Prefs::Pack<ByteString>(const ByteString &value)
	{
		return Json::Value(value);
	}

	template<>
	Json::Value Prefs::Pack<String>(const String &value)
	{
		return Json::Value(value.ToUtf8());
	}

	template<>
	Json::Value Prefs::Pack<std::vector<ByteString>>(const std::vector<ByteString> &value)
	{
		Json::Value array = Json::arrayValue;
		for (auto item : value)
		{
			array.append(Json::Value(item));
		}
		return array;
	}

	template<>
	Json::Value Prefs::Pack<std::vector<String>>(const std::vector<String> &value)
	{
		Json::Value array = Json::arrayValue;
		for (auto item : value)
		{
			array.append(Json::Value(item.ToUtf8()));
		}
		return array;
	}

	template<>
	int Prefs::Unpack<int>(const Json::Value &value)
	{
		return value.asInt();
	}

	template<>
	uint64_t Prefs::Unpack<uint64_t>(const Json::Value &value)
	{
		return value.asUInt64();
	}

	template<>
	float Prefs::Unpack<float>(const Json::Value &value)
	{
		return value.asFloat();
	}

	template<>
	bool Prefs::Unpack<bool>(const Json::Value &value)
	{
		return value.asBool();
	}

	template<>
	ByteString Prefs::Unpack<ByteString>(const Json::Value &value)
	{
		return value.asString();
	}

	template<>
	String Prefs::Unpack<String>(const Json::Value &value)
	{
		return ByteString(value.asString()).FromUtf8();
	}

	template<>
	std::vector<ByteString> Prefs::Unpack(const Json::Value &value)
	{
		std::vector<ByteString> array;
		if (value.type() != Json::arrayValue)
		{
			throw std::exception();
		}
		for (auto &item : value)
		{
			array.push_back(item.asString());
		}
		return array;
	}

	template<>
	std::vector<String> Prefs::Unpack(const Json::Value &value)
	{
		std::vector<String> array;
		if (value.type() != Json::arrayValue)
		{
			throw std::exception();
		}
		for (auto &item : value)
		{
			array.push_back(ByteString(item.asString()).FromUtf8());
		}
		return array;
	}
}
