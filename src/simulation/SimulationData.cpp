#include "SimulationData.h"

#include "ElementGraphics.h"
#include "ElementDefs.h"
#include "ElementClasses.h"

#include "BuiltinGOL.h"
#include "WallType.h"
#include "MenuSection.h"

#include "graphics/SimulationRenderer.h"
#include "graphics/Pix.h"

#include <algorithm>
#include <cmath>
#include <cstring>

// * TODO-REDO_UI: (re)move
const char *flmData = nullptr;
const char *plasmaData = nullptr;
std::vector<char> flmVec;
std::vector<char> plasmaVec;
unsigned int fireAlpha[CELL * 3][CELL * 3];

// * TODO-REDO_UI: (re)move
struct GradientStop
{
	int color;
	float point;

	bool operator <(const GradientStop &other) const
	{
		return point < other.point;
	}
};

// * TODO-REDO_UI: (re)move
std::vector<char> Gradient(std::vector<GradientStop> stops, int resolution)
{
	std::vector<char> table(resolution * 3, 0);
	if (stops.size() >= 2)
	{
		std::sort(stops.begin(), stops.end());
		auto stop = -1;
		for (auto i = 0; i < resolution; ++i)
		{
			auto point = i / (float)resolution;
			while (stop < (int)stops.size() - 1 && stops[stop + 1].point <= point)
			{
				++stop;
			}
			if (stop < 0 || stop >= (int)stops.size() - 1)
			{
				continue;
			}
			auto &left = stops[stop];
			auto &right = stops[stop + 1];
			auto f = (point - left.point) / (right.point - left.point);
			table[i * 3    ] = char(PixR(left.color) * (1.0f - f) + PixR(right.color) * f);
			table[i * 3 + 1] = char(PixG(left.color) * (1.0f - f) + PixG(right.color) * f);
			table[i * 3 + 2] = char(PixB(left.color) * (1.0f - f) + PixB(right.color) * f);
		}
	}
	return table;
}

// * TODO-REDO_UI: (re)move
void InitGraphicsTables()
{
	static bool initDone = false;
	if (!initDone)
	{
		initDone = true;
		flmVec = Gradient({
			{ 0xAF9F0F, 1.0f },
			{ 0xDFBF6F, 0.9f },
			{ 0x60300F, 0.5f },
			{ 0x000000, 0.0f },
		}, 200);
		flmData = flmVec.data();
		plasmaVec = Gradient({
			{ 0xAFFFFF,  1.0f },
			{ 0xAFFFFF,  0.9f },
			{ 0x301060,  0.5f },
			{ 0x301040, 0.25f },
			{ 0x000000,  0.0f },
		}, 200);
		plasmaData = plasmaVec.data();

		float temp[CELL * 3][CELL * 3] = {};
		float multiplier = 255.0f;
		for (int x = 0; x < CELL; ++x)
		{
			for (int y = 0; y < CELL; ++y)
			{
				for (int i = -CELL; i < CELL; ++i)
				{
					for (int j = -CELL; j < CELL; ++j)
					{
						temp[y + CELL + j][x + CELL + i] += expf(-0.1f * (i * i + j * j));
					}
				}
			}
		}
		for (int x = 0; x < CELL * 3; ++x)
		{
			for (int y = 0; y < CELL * 3; ++y)
			{
				fireAlpha[y][x] = (int)(multiplier * temp[y][x] / (CELL * CELL));
			}
		}
	}
}

// * TODO-REDO_UI: (re)move
const BuiltinGOL builtinGol[NGOL] = {
	// * Ruleset:
	//   * bits x = 8..0: stay if x neighbours present
	//   * bits x = 16..9: begin if x-8 neighbours present
	//   * bits 20..17: 4-bit unsigned int encoding the number of states minus 2; 2 states is
	//     encoded as 0, 3 states as 1, etc.
	//   * states are kind of long until a cell dies; normal ones use two states (living and dead),
	//     for others the intermediate states live but do nothing
	//   * the ruleset constants below look 20-bit, but rulesets actually consist of 21
	//     bits of data; bit 20 just happens to not be set for any of the built-in types,
	//     as none of them have 10 or more states
	{  "GOL",  "GOL", GT_GOL , 0x0080C, 0x0CAC00, 0x0CAC00, NGT_GOL  },
	{ "HLIF", "HLIF", GT_HLIF, 0x0480C, 0xFF0000, 0xFF0000, NGT_HLIF },
	{ "ASIM", "ASIM", GT_ASIM, 0x038F0, 0x0000FF, 0x0000FF, NGT_ASIM },
	{  "2X2",  "2X2", GT_2x2 , 0x04826, 0xFFFF00, 0xFFFF00, NGT_2x2  },
	{ "DANI", "DANI", GT_DANI, 0x1C9D8, 0x00FFFF, 0x00FFFF, NGT_DANI },
	{ "AMOE", "AMOE", GT_AMOE, 0x0A92A, 0xFF00FF, 0xFF00FF, NGT_AMOE },
	{ "MOVE", "MOVE", GT_MOVE, 0x14834, 0xFFFFFF, 0xFFFFFF, NGT_MOVE },
	{ "PGOL", "PGOL", GT_PGOL, 0x0A90C, 0xE05010, 0xE05010, NGT_PGOL },
	{ "DMOE", "DMOE", GT_DMOE, 0x1E9E0, 0x500000, 0x500000, NGT_DMOE },
	{   "34",  "3-4", GT_34  , 0x01818, 0x500050, 0x500050, NGT_34   },
	{ "LLIF", "LLIF", GT_LLIF, 0x03820, 0x505050, 0x505050, NGT_LLIF },
	{ "STAN", "STAN", GT_STAN, 0x1C9EC, 0x5000FF, 0x5000FF, NGT_STAN },
	{ "SEED", "SEED", GT_SEED, 0x00400, 0xFBEC7D, 0xFBEC7D, NGT_SEED },
	{ "MAZE", "MAZE", GT_MAZE, 0x0083E, 0xA8E4A0, 0xA8E4A0, NGT_MAZE },
	{ "COAG", "COAG", GT_COAG, 0x189EC, 0x9ACD32, 0x9ACD32, NGT_COAG },
	{ "WALL", "WALL", GT_WALL, 0x1F03C, 0x0047AB, 0x0047AB, NGT_WALL },
	{ "GNAR", "GNAR", GT_GNAR, 0x00202, 0xE5B73B, 0xE5B73B, NGT_GNAR },
	{ "REPL", "REPL", GT_REPL, 0x0AAAA, 0x259588, 0x259588, NGT_REPL },
	{ "MYST", "MYST", GT_MYST, 0x139E1, 0x0C3C00, 0x0C3C00, NGT_MYST },
	{ "LOTE", "LOTE", GT_LOTE, 0x48938, 0xFF0000, 0xFFFF00, NGT_LOTE },
	{ "FRG2", "FRG2", GT_FRG2, 0x20816, 0x006432, 0x00FF5A, NGT_FRG2 },
	{ "STAR", "STAR", GT_STAR, 0x98478, 0x000040, 0x0000E6, NGT_STAR },
	{ "FROG", "FROG", GT_FROG, 0x21806, 0x006400, 0x00FF00, NGT_FROG },
	{ "BRAN", "BRAN", GT_BRAN, 0x25440, 0xFFFF00, 0x969600, NGT_BRAN },
};

// * TODO-REDO_UI: (re)move
std::vector<wall_type> LoadWalls()
{
	return
	std::vector<wall_type>{
		{0x808080, 0x000000, wall_type::SPECIAL  , String("ERASE"),           "DEFAULT_WL_ERASE",  String("Erases walls.")},
		{0xC0C0C0, 0x101010, wall_type::SOLIDDOT , String("CONDUCTIVE WALL"), "DEFAULT_WL_CNDTW",  String("Blocks everything. Conductive.")},
		{0x808080, 0x808080, wall_type::POWERED1 , String("EWALL"),           "DEFAULT_WL_EWALL",  String("E-Wall. Becomes transparent when electricity is connected.")},
		{0xFF8080, 0xFF2008, wall_type::HEXAGONAL, String("DETECTOR"),        "DEFAULT_WL_DTECT",  String("Detector. Generates electricity when a particle is inside.")},
		{0x808080, 0x000000, wall_type::SPECIAL  , String("STREAMLINE"),      "DEFAULT_WL_STRM",   String("Streamline. Set start point of a streamline.")},
		{0x8080FF, 0x000000, wall_type::HEXAGONAL, String("FAN"),             "DEFAULT_WL_FAN",    String("Fan. Accelerates air. Use the line tool to set direction and strength.")},
		{0xC0C0C0, 0x101010, wall_type::SPARSEDOT, String("LIQUID WALL"),     "DEFAULT_WL_LIQD",   String("Allows liquids, blocks all other particles. Conductive.")},
		{0x808080, 0x000000, wall_type::HEXAGONAL, String("ABSORB WALL"),     "DEFAULT_WL_ABSRB",  String("Absorbs particles but lets air currents through.")},
		{0x808080, 0x000000, wall_type::SOLIDDOT , String("WALL"),            "DEFAULT_WL_WALL",   String("Basic wall, blocks everything.")},
		{0x3C3C3C, 0x000000, wall_type::HEXAGONAL, String("AIRONLY WALL"),    "DEFAULT_WL_AIR",    String("Allows air, but blocks all particles.")},
		{0x575757, 0x000000, wall_type::HEXAGONAL, String("POWDER WALL"),     "DEFAULT_WL_POWDR",  String("Allows powders, blocks all other particles.")},
		{0xFFFF22, 0x101010, wall_type::SPARSEDOT, String("CONDUCTOR"),       "DEFAULT_WL_CNDTR",  String("Conductor. Allows all particles to pass through and conducts electricity.")},
		{0x242424, 0x101010, wall_type::POWERED0 , String("EHOLE"),           "DEFAULT_WL_EHOLE",  String("E-Hole. absorbs particles, releases them when powered.")},
		{0x579777, 0x000000, wall_type::HEXAGONAL, String("GAS WALL"),        "DEFAULT_WL_GAS",    String("Allows gases, blocks all other particles.")},
		{0xFFEE00, 0xAA9900, wall_type::DIAGONAL , String("GRAVITY WALL"),    "DEFAULT_WL_GRVTY",  String("Gravity wall. Newtonian Gravity has no effect inside a box drawn with this.")},
		{0xFFAA00, 0xAA5500, wall_type::DIAGONAL , String("ENERGY WALL"),     "DEFAULT_WL_ENRGY",  String("Allows energy particles, blocks all other particles.")},
		{0xDCDCDC, 0x000000, wall_type::HEXAGONAL, String("AIRBLOCK WALL"),   "DEFAULT_WL_NOAIR",  String("Allows all particles, but blocks air.")},
		{0x808080, 0x000000, wall_type::SPECIAL  , String("ERASEALL"),        "DEFAULT_WL_ERASEA", String("Erases walls, particles, and signs.")},
		{0x800080, 0x000000, wall_type::POWERED0 , String("STASIS WALL"),     "DEFAULT_WL_STASIS", String("Freezes particles inside the wall in place until powered.")},
	};
}

// * TODO-REDO_UI: (re)move
std::vector<menu_section> LoadMenus()
{
	return
	std::vector<menu_section>{
		{0xE041, String("Walls"), 0, 1},
		{0xE042, String("Electronics"), 0, 1},
		{0xE056, String("Powered Materials"), 0, 1},
		{0xE019, String("Sensors"), 0, 1},
		{0xE062, String("Force"), 0, 1},
		{0xE043, String("Explosives"), 0, 1},
		{0xE045, String("Gases"), 0, 1},
		{0xE044, String("Liquids"), 0, 1},
		{0xE050, String("Powders"), 0, 1},
		{0xE051, String("Solids"), 0, 1},
		{0xE046, String("Radioactive"), 0, 1},
		{0xE04C, String("Special"), 0, 1},
		{0xE052, String("Game Of Life"), 0, 1},
		{0xE057, String("Tools"), 0, 1},
		{0xE067, String("Favorites"), 0, 1},
		{0xE064, String("Decoration tools"), 0, 1},
		{0xE048, String("Cracker"), 0, 0},
		{0xE048, String("Cracker!"), 0, 0},
	};
}

std::vector<unsigned int> LoadLatent()
{
	return
	std::vector<unsigned int>{
		/* NONE */ 0,
		/* DUST */ 0,
		/* WATR */ 7500,
		/* OIL  */ 0,
		/* FIRE */ 0,
		/* STNE */ 0,
		/* LAVA */ 0,
		/* GUN  */ 0,
		/* NITR */ 0,
		/* CLNE */ 0,
		/* GAS  */ 0,
		/* C-4  */ 0,
		/* GOO  */ 0,
		/* ICE  */ 1095,
		/* METL */ 919,
		/* SPRK */ 0,
		/* SNOW */ 1095,
		/* WOOD */ 0,
		/* NEUT */ 0,
		/* PLUT */ 0,
		/* PLNT */ 0,
		/* ACID */ 0,
		/* VOID */ 0,
		/* WTRV */ 0,
		/* CNCT */ 0,
		/* DSTW */ 7500,
		/* SALT */ 0,
		/* SLTW */ 7500,
		/* DMND */ 0,
		/* BMTL */ 0,
		/* BRMT */ 0,
		/* PHOT */ 0,
		/* URAN */ 0,
		/* WAX  */ 0,
		/* MWAX */ 0,
		/* PSCN */ 0,
		/* NSCN */ 0,
		/* LN2  */ 0,
		/* INSL */ 0,
		/* VACU */ 0,
		/* VENT */ 0,
		/* RBDM */ 0,
		/* LRBD */ 0,
		/* NTCT */ 0,
		/* SAND */ 0,
		/* GLAS */ 0,
		/* PTCT */ 0,
		/* BGLA */ 0,
		/* THDR */ 0,
		/* PLSM */ 0,
		/* ETRD */ 0,
		/* NICE */ 0,
		/* NBLE */ 0,
		/* BTRY */ 0,
		/* LCRY */ 0,
		/* STKM */ 0,
		/* SWCH */ 0,
		/* SMKE */ 0,
		/* DESL */ 0,
		/* COAL */ 0,
		/* LO2  */ 0,
		/* O2   */ 0,
		/* INWR */ 0,
		/* YEST */ 0,
		/* DYST */ 0,
		/* THRM */ 0,
		/* GLOW */ 0,
		/* BRCK */ 0,
		/* CFLM */ 0,
		/* FIRW */ 0,
		/* FUSE */ 0,
		/* FSEP */ 0,
		/* AMTR */ 0,
		/* BCOL */ 0,
		/* PCLN */ 0,
		/* HSWC */ 0,
		/* IRON */ 0,
		/* MORT */ 0,
		/* LIFE */ 0,
		/* DLAY */ 0,
		/* CO2  */ 0,
		/* DRIC */ 0,
		/* CBNW */ 7500,
		/* STOR */ 0,
		/* STOR */ 0,
		/* FREE */ 0,
		/* FREE */ 0,
		/* FREE */ 0,
		/* FREE */ 0,
		/* FREE */ 0,
		/* SPNG */ 0,
		/* RIME */ 0,
		/* FOG  */ 0,
		/* BCLN */ 0,
		/* LOVE */ 0,
		/* DEUT */ 0,
		/* WARP */ 0,
		/* PUMP */ 0,
		/* FWRK */ 0,
		/* PIPE */ 0,
		/* FRZZ */ 0,
		/* FRZW */ 0,
		/* GRAV */ 0,
		/* BIZR */ 0,
		/* BIZRG*/ 0,
		/* BIZRS*/ 0,
		/* INST */ 0,
		/* ISOZ */ 0,
		/* ISZS */ 0,
		/* PRTI */ 0,
		/* PRTO */ 0,
		/* PSTE */ 0,
		/* PSTS */ 0,
		/* ANAR */ 0,
		/* VINE */ 0,
		/* INVS */ 0,
		/* EQVE */ 0,
		/* SPWN2*/ 0,
		/* SPAWN*/ 0,
		/* SHLD1*/ 0,
		/* SHLD2*/ 0,
		/* SHLD3*/ 0,
		/* SHLD4*/ 0,
		/* LOlZ */ 0,
		/* WIFI */ 0,
		/* FILT */ 0,
		/* ARAY */ 0,
		/* BRAY */ 0,
		/* STKM2*/ 0,
		/* BOMB */ 0,
		/* C-5  */ 0,
		/* SING */ 0,
		/* QRTZ */ 0,
		/* PQRT */ 0,
		/* EMP  */ 0,
		/* BREL */ 0,
		/* ELEC */ 0,
		/* ACEL */ 0,
		/* DCEL */ 0,
		/* TNT  */ 0,
		/* IGNP */ 0,
		/* BOYL */ 0,
		/* GEL  */ 0,
		/* FREE */ 0,
		/* FREE */ 0,
		/* FREE */ 0,
		/* FREE */ 0,
		/* WIND */ 0,
		/* H2   */ 0,
		/* SOAP */ 0,
		/* NBHL */ 0,
		/* NWHL */ 0,
		/* MERC */ 0,
		/* PBCN */ 0,
		/* GPMP */ 0,
		/* CLST */ 0,
		/* WIRE */ 0,
		/* GBMB */ 0,
		/* FIGH */ 0,
		/* FRAY */ 0,
		/* REPL */ 0,
	};
}
