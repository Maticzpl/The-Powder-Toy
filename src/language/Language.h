#pragma once
#include "Config.h"

#include "common/ExplicitSingleton.h"
#include "common/String.h"

#include <map>
#include <vector>

namespace language
{
	class Language : public common::ExplicitSingleton<Language>
	{
		Language(const Language &) = delete;
		Language &operator =(const Language &) = delete;

		struct TemplateEntry
		{
			String before;
			size_t index;
			// size_t precision;
		};
		using Template = std::vector<TemplateEntry>;
		std::map<String, Template> templates;

		bool LoadFromSource(String source, const String &buf);
		bool LoadExternal(String code);
		bool LoadInternal(String code);
		static bool Parse(std::map<String, Template> &templates, String &err, int &errRow, int &errCol, String buf);

		template<class Arg, class ...Args>
		static void ArgStrings(std::vector<String> &argStrings, size_t index, Arg &&arg, Args &&...args)
		{
			argStrings[index] = String::Build(std::forward<Arg>(arg));
			ArgStrings(argStrings, index + 1U, std::forward<Args>(args)...);
		}

		static void ArgStrings(std::vector<String> &argStrings, size_t index)
		{
		}

		String FormatStrings(String id, const std::vector<String> &argStrings);

	public:
		Language(String code);

		template<class ...Args>
		String Format(String id, Args &&...args)
		{
			std::vector<String> argStrings(sizeof...(Args));
			ArgStrings(argStrings, 0U, std::forward<Args>(args)...);
			return FormatStrings(id, argStrings);
		}
	};

	struct Template
	{
		String id;

		template<class ...Args>
		String operator ()(Args &&...args)
		{
			return Language::Ref().Format(id, std::forward<Args>(args)...);
		}
	};
}

inline language::Template operator "" _Ls(const char *S, size_t N)
{
	// * S is supposed to be ASCII (the set of characters allowed in
	//   identifiers is a subset of ASCII), no need to convert from UTF-8.
	return language::Template{ String(S, S + N) };
}
