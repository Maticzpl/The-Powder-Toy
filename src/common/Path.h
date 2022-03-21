#pragma once
#include "Config.h"

#include "String.h"

#include <vector>
#include <stdexcept>

class Path
{
	bool relative = false;
	String root;
	int parentDepth = 0;
	std::vector<String> components;

public:
	static Path FromString(const String &str);

	bool Relative() const
	{
		return relative;
	}
	void Relative(bool newRelative);

	String Root() const
	{
		return root;
	}
	void Root(String newRoot);

	int ParentDepth() const
	{
		return parentDepth;
	}
	void ParentDepth(int newParentDepth);

	const std::vector<String> &Components() const
	{
		return components;
	}

	struct FormatError : public std::runtime_error
	{
		using runtime_error::runtime_error;
	};

	Path Parent() const;
	Path Append(const String &component) const;
	Path Append(const Path &path) const;
	Path Rebase(const Path &onto) const;

	operator String() const;

	bool operator ==(const Path &other) const;
	bool operator !=(const Path &other) const
	{
		return !(*this == other);
	}

	bool Empty() const
	{
		return components.empty();
	}
};
