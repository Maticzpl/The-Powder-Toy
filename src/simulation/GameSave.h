#ifndef The_Powder_Toy_GameSave_h
#define The_Powder_Toy_GameSave_h
#include "Config.h"

#include <vector>
#include "common/String.h"
#include "Misc.h"

#include "bson/BSON.h"
#include "json/json.h"

struct sign;
struct Particle;

struct ParseException: public std::exception {
	enum ParseResult { OK = 0, Corrupt, WrongVersion, InvalidDimensions, InternalError, MissingElement };
	ByteString message;
	ParseResult result;
public:
	ParseException(ParseResult result, String message): message(message.ToUtf8()), result(result) {}
	const char * what() const throw() override
	{
		return message.c_str();
	}
	~ParseException() throw() {}
};

struct BuildException: public std::exception {
	ByteString message;
public:
	BuildException(String message): message(message.ToUtf8()) {}
	const char * what() const throw() override
	{
		return message.c_str();
	}
	~BuildException() throw() {}
};

class StkmData
{
public:
	bool rocketBoots1 = false;
	bool rocketBoots2 = false;
	bool fan1 = false;
	bool fan2 = false;
	std::vector<unsigned int> rocketBootsFigh = std::vector<unsigned int>();
	std::vector<unsigned int> fanFigh = std::vector<unsigned int>();

	StkmData() = default;

	StkmData(const StkmData & stkmData):
		rocketBoots1(stkmData.rocketBoots1),
		rocketBoots2(stkmData.rocketBoots2),
		fan1(stkmData.fan1),
		fan2(stkmData.fan2),
		rocketBootsFigh(stkmData.rocketBootsFigh),
		fanFigh(stkmData.fanFigh)
	{

	}

	bool hasData() const
	{
		return rocketBoots1 || rocketBoots2 || fan1 || fan2
		        || rocketBootsFigh.size() || fanFigh.size();
	}
};

class GameSave
{
public:

	int blockWidth, blockHeight;
	bool fromNewerVersion;
	int majorVersion, minorVersion;
	bool hasPressure;
	bool hasAmbientHeat;

	//Simulation data
	//int ** particleMap;
	int particlesCount;
	Particle * particles;
	unsigned char ** blockMap;
	float ** fanVelX;
	float ** fanVelY;
	float ** pressure;
	float ** velocityX;
	float ** velocityY;
	float ** ambientHeat;

	//Simulation Options
	bool waterEEnabled;
	bool legacyEnable;
	bool gravityEnable;
	bool aheatEnable;
	bool paused;
	int gravityMode;
	int airMode;
	float ambientAirTemp;
	int edgeMode;

	//Signs
	std::vector<sign> signs;
	StkmData stkm;

	//Element palette
	typedef std::pair<ByteString, int> PaletteItem;
	std::vector<PaletteItem> palette;

	// author information
	Json::Value authors;

	int pmapbits;

	GameSave();
	GameSave(const GameSave & save);
	GameSave(int width, int height);
	GameSave(char * data, int dataSize);
	GameSave(std::vector<char> data);
	GameSave(std::vector<unsigned char> data);
	~GameSave();
	void setSize(int width, int height);
	char * Serialise(unsigned int & dataSize) const;
	std::vector<char> Serialise() const;

	void Extract(uint32_t rotflip, int nudgeX, int nudgeY, int rectBX, int rectBY, int rectBW, int rectBH);
	void BlockSizeAfterExtract(uint32_t rotflip, int nudgeX, int nudgeY, int &rectBW, int &rectBH) const;

	void Expand();
	void Collapse();
	bool Collapsed() const
	{
		return !expanded;
	}

	static bool TypeInCtype(int type, int ctype);
	static bool TypeInTmp(int type);
	static bool TypeInTmp2(int type, int tmp2);
	static bool PressureInTmp3(int type);

	GameSave& operator << (Particle &v);
	GameSave& operator << (sign &v);

private:
	bool expanded;
	bool hasOriginalData;

	std::vector<char> originalData;

	void InitData();
	void InitVars();
	void CheckBsonFieldUser(bson_iterator iter, const char *field, unsigned char **data, unsigned int *fieldLen);
	void CheckBsonFieldBool(bson_iterator iter, const char *field, bool *flag);
	void CheckBsonFieldInt(bson_iterator iter, const char *field, int *setting);
	void CheckBsonFieldFloat(bson_iterator iter, const char *field, float *setting);
	template <typename T> T ** Allocate2DArray(int blockWidth, int blockHeight, T defaultVal);
	template <typename T> void Deallocate2DArray(T ***array, int blockHeight);
	void dealloc();
	void read(char * data, int dataSize);
	void readOPS(char * data, int dataLength);
	void readPSv(char * data, int dataLength);
	char * serialiseOPS(unsigned int & dataSize) const;
	static void ConvertJsonToBson(bson *b, Json::Value j, int depth = 0);
	static void ConvertBsonToJson(bson_iterator *b, Json::Value *j, int depth = 0);
};

#endif
