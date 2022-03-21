#pragma once

#include "Config.h"

#include <cstdint>
#include <vector>

struct playerst;
class Simulation;

struct SimulationGraphicsValues
{
	bool ready = false;
	int pixelMode, colA, colR, colG, colB, firA, firR, firG, firB;
};

class SimulationRenderer
{
	std::vector<uint32_t> mainBuffer, persBuffer, warpBuffer;
	std::vector<uint32_t> fire;

	template<bool Checked>
	uint32_t GetPixel(int x, int y);

	template<bool Checked>
	void SetPixel(int x, int y, uint32_t rgb);

	template<bool Checked>
	void BlendPixel(int x, int y, uint32_t rgba);

	template<bool Checked>
	void AddPixel(int x, int y, uint32_t rgba);

	void DrawBlob(int x, int y, uint32_t rgba);
	void DrawLine(int x0, int y0, int x1, int y1, uint32_t rgb);
	void DrawStickman(int x, int y, int t, const Simulation &simulation, const playerst &stickman, uint32_t headrgb);

	void RenderPersistent();
	void RenderAir(const Simulation &simulation);
	void RenderGravity(const Simulation &simulation);
	void RenderWalls(const Simulation &simulation);
	void RenderGrid();
	void RenderParticles(const Simulation &simulation);
	void UpdatePersistent();
	void RenderFire();
	void RenderEMP(const Simulation &simulation);
	void RenderGravityZones(const Simulation &simulation);
	void RenderSigns(const Simulation &simulation);
	void RenderGravityLensing(const Simulation &simulation);

	bool gravityFieldEnabled, gravityZonesEnabled, decorationsEnabled, blackDecorations;
	uint32_t colorMode, displayMode, renderMode;
	int gridSize;

	std::vector<SimulationGraphicsValues> graphicsCache;

	SimulationRenderer(const SimulationRenderer &) = delete;
	SimulationRenderer &operator =(const SimulationRenderer &) = delete;

public:
	SimulationRenderer();

	void Render(const Simulation &simulation);
	void ClearAccumulation();
	void FlushGraphicsCache(int begin, int end);

	const std::vector<SimulationGraphicsValues> &GraphicsCache() const
	{
		return graphicsCache;
	}
	
	const uint32_t *MainBuffer() const
	{
		return mainBuffer.data();
	}

	void DecorationsEnabled(bool newDecorationsEnabled)
	{
		decorationsEnabled = newDecorationsEnabled;
	}
	bool DecorationsEnabled() const
	{
		return decorationsEnabled;
	}

	void BlackDecorations(bool newBlackDecorations)
	{
		blackDecorations = newBlackDecorations;
	}
	bool BlackDecorations() const
	{
		return blackDecorations;
	}

	void GravityFieldEnabled(bool newGravityFieldEnabled)
	{
		gravityFieldEnabled = newGravityFieldEnabled;
	}
	bool GravityFieldEnabled() const
	{
		return gravityFieldEnabled;
	}

	void GravityZonesEnabled(bool newGravityZonesEnabled)
	{
		gravityZonesEnabled = newGravityZonesEnabled;
	}
	bool GravityZonesEnabled() const
	{
		return gravityZonesEnabled;
	}

	void ColorMode(uint32_t newColorMode);
	uint32_t ColorMode() const
	{
		return colorMode;
	}

	void DisplayMode(uint32_t newDisplayMode);
	uint32_t DisplayMode() const
	{
		return displayMode;
	}

	void RenderMode(uint32_t newRenderMode);
	uint32_t RenderMode() const
	{
		return renderMode;
	}

	uint32_t HeatToColour(float temp) const;

	static const std::vector<uint32_t> &FlameTable();
	static const std::vector<uint32_t> &PlasmaTable();
	static const std::vector<uint32_t> &HeatTable();
	static const std::vector<uint32_t> &ClfmTable();
	static const std::vector<uint32_t> &FirwTable();
};
