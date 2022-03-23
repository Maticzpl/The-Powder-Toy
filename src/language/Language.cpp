#include "Language.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <iterator>
#include <cerrno>
#include <cstring>

#define LANGUAGES_DECLARE
#include "Languages.h"
#undef LANGUAGES_DECLARE

namespace language
{
	bool Language::LoadFromSource(String source, const String &buf)
	{
		String err;
		int errRow;
		int errCol;
		std::map<String, Template> newTemplates;
		if (!Parse(newTemplates, err, errRow, errCol, buf))
		{	
			std::cerr << source.ToUtf8() << ":" << errRow << ":" << errCol << ": " << err.ToUtf8() << std::endl;
			return false;
		}
		templates = newTemplates;
		return true;
	}

	bool Language::LoadExternal(String code)
	{
		auto source = code + ".txt";
		std::ifstream f(source.ToUtf8(), std::ios::binary);
		if (!f)
		{
			std::cerr << "Language::LoadExternal: failed to load " << source.ToUtf8() << ": " << strerror(errno) << std::endl;
			return false;
		}
		String buf;
		try
		{
			buf = ByteString(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()).FromUtf8();
		}
		catch (const ConversionError &ex)
		{
			std::cerr << "Language::LoadExternal: failed to load " << source.ToUtf8() << ": " << ex.what() << std::endl;
			return false;
		}
		return LoadFromSource(source, buf);
	}

	bool Language::LoadInternal(String code)
	{
		struct Lang
		{
			String code;
			const unsigned char *base;
			unsigned int size;
		};
		static const Lang langs[] = {
#define LANGUAGES_ENUMERATE
#include "Languages.h"
#undef LANGUAGES_ENUMERATE
			{ "", nullptr, 0 }
		};
		auto source = "[built-in " + code + "]";
		for (auto *lang = langs; lang->base; ++lang)
		{
			if (lang->code == code)
			{
				String buf;
				try
				{
					buf = ByteString(lang->base, lang->base + lang->size).FromUtf8();
				}
				catch (const ConversionError &ex)
				{
					std::cerr << "Language::LoadInternal: failed to load " << source.ToUtf8() << ": " << ex.what() << std::endl;
					return false;
				}
				return LoadFromSource(source, buf);
			}
		}
		std::cerr << "Language::LoadInternal: failed to load " << source.ToUtf8() << ": not built in" << std::endl;
		return false;
	}

	bool Language::Parse(std::map<String, Template> &templates, String &err, int &errRow, int &errCol, String buf)
	{
		buf += "\n"; // * Easier if this assumption holds (e.g. comments in skipWhitespace).
		auto it = buf.begin();
		auto itRow = it;
		auto row = 1;
		auto end = buf.end();
		auto reportError = [&err, &errRow, &errCol, &row, &it, &itRow](String str) {
			err = str;
			errRow = row;
			errCol = it - itRow + 1;
			return false;
		};
		auto isWhitespace = [&it, &end]() { // * Do not try to skip with bumpIt, use skipWhitespace.
			return it != end && (*it == ' ' || *it == '\t' || *it == '\n' || *it == '\r' || *it == '#');
		};
		auto bumpIt = [&it, &row, &itRow]() {
			auto newLine = *it == '\n';
			++it;
			if (newLine)
			{
				row += 1;
				itRow = it;
			}
		};
		auto skipWhitespace = [isWhitespace, &it, bumpIt]() {
			while (isWhitespace())
			{
				if (*it == '#')
				{
					while (*it != '\n')
					{
						bumpIt();
					}
				}
				bumpIt();
			}
		};
		enum State
		{
			stateExpectId,
			stateExpectIdOrEq,
			stateExpectEq,
			stateExpectStrBegin,
			stateExpectStrEnd,
			stateExpectIndex,
			stateExpectIndexEnd,
			stateExpectEscapeEnd,
			stateExpectUniversal,
		};
		State state = stateExpectId;
		String id;
		String before;
		Template templ;
		size_t index = 0;
		uint32_t universal = 0;
		int universalRemaining = 0;
		auto pushTemplate = [&id, &templ, &before, &templates]() {
			templ.push_back({ before });
			templates.insert(std::make_pair(id, templ));
			id.clear();
			templ.clear();
			before.clear();
		};
		skipWhitespace();
		while (it != end)
		{
			// * Whitespaces must have been skipped by this point.
			switch (state)
			{
			case stateExpectIdOrEq:
				if (*it == '=')
				{
					state = stateExpectEq;
					break;
				}
			case stateExpectId:
				if (!((*it >= '0' && *it <= '9') ||
				      (*it >= 'A' && *it <= 'Z') ||
				      (*it == '_')))
				{
					return reportError("character not allowed in identifiers");
				}
				id.append(1, *it);
				bumpIt();
				if (isWhitespace())
				{
					skipWhitespace();
					state = stateExpectEq;
					break;
				}
				state = stateExpectIdOrEq;
				break;

			case stateExpectEq:
				if (*it != '=')
				{
					return reportError("expected a '=' chacater");
				}
				bumpIt();
				skipWhitespace();
				state = stateExpectStrBegin;
				break;

			case stateExpectStrBegin:
				if (*it != '"')
				{
					return reportError("expected a '\"' chacater");
				}
				bumpIt();
				state = stateExpectStrEnd;
				break;

			case stateExpectStrEnd:
				if (*it == '\\')
				{
					bumpIt();
					state = stateExpectEscapeEnd;
					break;
				}
				if (*it == '{')
				{
					bumpIt();
					state = stateExpectIndex;
					break;
				}
				if (*it == '"')
				{
					if (templates.find(id) != templates.end())
					{
						return reportError("identifier used for the second time");
					}
					pushTemplate();
					bumpIt();
					skipWhitespace();
					state = stateExpectId;
					break;
				}
				if (*it != '\r')
				{
					before.append(1, *it);
				}
				bumpIt();
				break;

			case stateExpectIndexEnd:
				if (*it == '}')
				{
					bumpIt();
					templ.push_back({ before, index });
					index = 0;
					before.clear();
					state = stateExpectStrEnd;
					break;
				}
			case stateExpectIndex:
				if (!(*it >= '0' && *it <= '9'))
				{
					return reportError("character not allowed in indices");
				}
				index = index * 10U + size_t(*it - '0');
				if (index >= 100)
				{
					return reportError("index too large");
				}
				bumpIt();
				state = stateExpectIndexEnd;
				break;

			case stateExpectEscapeEnd:
				if (*it == 'u')
				{
					bumpIt();
					universalRemaining = 4;
					state = stateExpectUniversal;
				}
				else if (*it == 'U')
				{
					bumpIt();
					universalRemaining = 8;
					state = stateExpectUniversal;
				}
				else if (*it == 'n')
				{
					before.append(1, '\n');
					bumpIt();
					state = stateExpectStrEnd;
				}
				else if (*it == 'b')
				{
					before.append(1, '\b');
					bumpIt();
					state = stateExpectStrEnd;
				}
				else if (*it != '\r')
				{
					before.append(1, *it);
					bumpIt();
					state = stateExpectStrEnd;
				}
				break;

			case stateExpectUniversal:
				if (!((*it >= '0' && *it <= '9') ||
				      (*it >= 'A' && *it <= 'F')))
				{
					return reportError("character not allowed in universal escape sequences");
				}
				if (*it >= '0' && *it <= '9')
				{
					universal = universal * 0x10U + uint32_t(*it - '0');
				}
				else
				{
					universal = universal * 0x10U + uint32_t(*it - 'A' + 10);
				}
				bumpIt();
				universalRemaining -= 1;
				if (!universalRemaining)
				{
					before.append(1, int(universal));
					universal = 0;
					state = stateExpectStrEnd;
				}
				break;
			}
		}
		if (state != stateExpectId)
		{
			return reportError("unexpected end of input");
		}
		return true;
	}

	Language::Language(String code)
	{
		if (!LoadExternal(code) &&
		    !LoadInternal(code) &&
		    !LoadInternal("en_US"))
		{
			throw std::runtime_error("failed to load default interface language definition");
		}
	}

	String Language::FormatStrings(String id, const std::vector<String> &argStrings)
	{
		auto it = templates.find(id);
		if (it == templates.end())
		{
			std::cerr << "unknown language template " << id.ToUtf8() << std::endl;
			return String::Build("#", id);
		}
		StringBuilder sb;
		for (auto i = 0; i < int(it->second.size()) - 1; ++i)
		{
			sb << it->second[i].before;
			if (it->second[i].index < argStrings.size())
			{
				sb << argStrings[it->second[i].index];
			}
			else
			{
				sb << "{" << it->second[i].index << "}";
			}
		}
		sb << it->second.back().before;
		return sb.Build();
	}
}
