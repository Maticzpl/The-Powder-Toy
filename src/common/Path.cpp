#include "Path.h"

String preferredSeparator = PATH_SEP;
#ifdef WIN
String acceptedSeparators = PATH_SEP "/";
#else
String acceptedSeparators = PATH_SEP;
#endif

static bool AcceptedSeparator(int ch)
{
	return acceptedSeparators.find(ch) != acceptedSeparators.npos;
}

static void CheckInvariants(bool relative, const String &root, int parentDepth)
{
	if (relative && root.size())
	{
		throw Path::FormatError("a relative path cannot have a root");
	}
	if (!relative && parentDepth)
	{
		throw Path::FormatError("an absolute path cannot include dot-dots");
	}
}

void Path::Relative(bool newRelative)
{
	CheckInvariants(newRelative, root, parentDepth);
	relative = newRelative;
}

void Path::Root(String newRoot)
{
	CheckInvariants(relative, newRoot, parentDepth);
	root = newRoot;
}

void Path::ParentDepth(int newParentDepth)
{
	CheckInvariants(relative, root, newParentDepth);
	parentDepth = newParentDepth;
}

Path Path::FromString(const String &str)
{
	Path path;
	auto rootEndsAt = 0;
#ifdef WIN
	if (int(str.size()) >= 2 && str[1] == ':' && ((str[0] >= 'A' && str[0] <= 'Z') || (str[0] >= 'a' && str[0] <= 'z')))
	{
		rootEndsAt = 2;
		path.root = str.Substr(0, rootEndsAt);
	}
	if (int(str.size()) >= 3 && str[0] == '\\' && str[1] == '\\')
	{
		rootEndsAt = 3;
		while (rootEndsAt < int(str.size()) && str[rootEndsAt] != '\\')
		{
			rootEndsAt += 1;
		}
		path.root = str.Substr(0, rootEndsAt);
	}
#endif
	path.Relative(rootEndsAt < int(str.size()) && !AcceptedSeparator(str[rootEndsAt]));
	String component;
	auto push_component = [&component, &path]() {
		if (component == "..")
		{
			path = path.Parent();
		}
		else if (component.size() && component != ".")
		{
			path = path.Append(component);
		}
		component.clear();
	};
	for (auto i = rootEndsAt; i < int(str.size()); ++i)
	{
		if (AcceptedSeparator(str[i]))
		{
			push_component();
		}
		else
		{
			component.append(1, str[i]);
		}
	}
	push_component();
	return path;
}

Path Path::Append(const String &component) const
{
	for (auto ch : component)
	{
		if (AcceptedSeparator(ch))
		{
			throw FormatError("component must not have a directory separator");
		}
	}
	Path path = *this;
	path.components.push_back(component);
	return path;
}

Path Path::Append(const Path &path) const
{
	if (!path.relative)
	{
		throw FormatError("an absolute path cannot be appended");
	}
	Path combined = *this;
	for (auto i = 0; i < path.parentDepth; ++i)
	{
		combined = combined.Parent();
	}
	for (auto &component : path.components)
	{
		combined.components.push_back(component);
	}
	return combined;
}

Path Path::Parent() const
{
	Path path = *this;
	if (path.components.size())
	{
		path.components.pop_back();
	}
	else if (path.relative)
	{
		path.parentDepth += 1;
	}
	else
	{
		throw FormatError("a root has no parent");
	}
	return path;
}

Path::operator String() const
{
	String res = root;
	if (!relative)
	{
		res += preferredSeparator;
	}
	for (auto i = 0; i < int(components.size()); ++i)
	{
		if (i > 0)
		{
			res += preferredSeparator;
		}
		res += components[i];
	}
	if (!res.size())
	{
		res += ".";
	}
	return res;
}

bool Path::operator ==(const Path &other) const
{
	if (relative != other.relative ||
	    relative != other.relative ||
	    root != other.root ||
	    parentDepth != other.parentDepth ||
	    components.size() != other.components.size())
	{
		return false;
	}
	for (auto i = 0; i < int(components.size()); ++i)
	{
		if (components[i] != other.components[i])
		{
			return false;
		}
	}
	return true;
}

Path Path::Rebase(const Path &onto) const
{
	if (relative || onto.relative)
	{
		throw FormatError("both paths must be absolute");
	}
	if (root != onto.root)
	{
		throw FormatError("root mismatch");
	}
	int matchesUpTo = 0;
	while (matchesUpTo < int(components.size()) && matchesUpTo < int(onto.components.size()) && components[matchesUpTo] == onto.components[matchesUpTo])
	{
		matchesUpTo += 1;
	}
	Path path;
	path.relative = true;
	path.parentDepth = int(onto.components.size()) - matchesUpTo;
	for (auto i = matchesUpTo; i < int(components.size()); ++i)
	{
		path.components.push_back(components[i]);
	}
	return path;
}
