#ifndef PLATFORM_H
#define PLATFORM_H
#include "Config.h"

#include "common/String.h"

#include <cstdint>

#ifdef WIN
# include <string>
#endif

namespace Platform
{
	ByteString GetCwd();
	ByteString ExecutableName();
	void DoRestart();

	void OpenURI(ByteString uri);

	void Millisleep(long int t);
	long unsigned int GetTime();

	void LoadFileInResource(int name, int type, unsigned int& size, const char*& data);

	bool WriteFile(std::vector<char> data, ByteString path);
	std::vector<char> ReadFile(ByteString path);

	bool Stat(ByteString filename);
	bool FileExists(ByteString filename);
	bool DirectoryExists(ByteString directory);
	/**
	 * @return true on success
	 */
	bool RemoveFile(ByteString filename);
	bool RenameFile(ByteString from, ByteString to);

	/**
	 * @return true on success
	 */
	bool DeleteDirectory(ByteString folder);

	/**
	 * @return true on success
	 */
	bool MakeDirectory(ByteString dir);
	std::vector<ByteString> DirectorySearch(ByteString directory, ByteString search, std::vector<ByteString> extensions);
	String DoMigration(ByteString fromDir, ByteString toDir);

	ByteString ParentDirectory(ByteString path);

#ifdef WIN
	ByteString WinNarrow(const std::wstring &source);
	std::wstring WinWiden(const ByteString &source);
#endif

	void OpenDataFolder();

	extern std::string originalCwd;
	extern std::string sharedCwd;
}

#endif
