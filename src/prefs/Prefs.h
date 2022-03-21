#pragma once

#include "Config.h"
#include "common/String.h"

#include "json/json.h"

namespace prefs
{
	class Prefs
	{
	public:
		class DeferWrite
		{
			Prefs &prefs;

			DeferWrite(const DeferWrite &) = delete;
			DeferWrite &operator =(const DeferWrite &) = delete;

		public:
			DeferWrite(Prefs &newPrefs) : prefs(newPrefs)
			{
				prefs.deferWriteLevel += 1;
			}

			~DeferWrite()
			{
				prefs.deferWriteLevel -= 1;
				prefs.Write();
			}
		};

	private:
		Json::Value root;
		static Json::Value GetJson(const Json::Value &node, ByteString path);
		static void SetJson(Json::Value &node, ByteString path, Json::Value value);

		template<class Type>
		static Json::Value Pack(const Type &value);

		template<class Type>
		static Type Unpack(const Json::Value &value);

		void Read();
		void Write();
		int deferWriteLevel = 0;

		ByteString prevPath;
		ByteString currPath;
		ByteString nextPath;

		Prefs(const Prefs &) = delete;
		Prefs &operator =(const Prefs &) = delete;

	public:
		Prefs(ByteString path);

		template<class Type>
		Type Get(ByteString path, Type defaultValue) const
		{
			auto value = GetJson(root, path);
			if (value != Json::nullValue)
			{
				try
				{
					return Unpack<Type>(value);
				}
				catch (const std::exception &e)
				{
				}
			}
			return defaultValue;
		}

		template<class Enum>
		Enum Get(ByteString path, Enum maxValue, Enum defaultValue) const
		{
			int value = Get(path, int(defaultValue));
			if (value < 0 || value >= int(maxValue))
			{
				value = int(defaultValue);
			}
			return Enum(value);
		}

		template<class Type>
		void Set(ByteString path, Type value)
		{
			SetJson(root, path, Pack(value));
			Write();
		}

		void Clear(ByteString path)
		{
			SetJson(root, path, Json::nullValue);
			Write();
		}

		friend class DeferWrite;
	};
}
