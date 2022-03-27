#include "ThumbnailRenderer.h"

#include "common/tpt-compat.h"
#include "common/Worker.h"
#include "simulation/Simulation.h"
#include "simulation/ElementGraphics.h"
#include "graphics/SimulationRenderer.h"
#include "simulation/GameSave.h"

#include <stdexcept>

namespace graphics
{
	ThumbnailRenderer::ThumbnailRenderer()
	{
		simulation = std::make_unique<Simulation>();
		simulationRenderer = std::make_unique<SimulationRenderer>();
	}

	ThumbnailRenderer::~ThumbnailRenderer()
	{
	}

	void ThumbnailRenderer::Process(ThumbnailRendererTask &task)
	{
		auto &conf = task.conf;
		auto &output = task.output;
		if (conf.source->Collapsed())
		{
			throw std::runtime_error("save is collapsed");
		}
		if (conf.source->blockWidth > XRES / CELL || conf.source->blockHeight > YRES / CELL)
		{
			throw std::runtime_error("source rect is too big");
		}
		simulationRenderer->DecorationsEnabled(true);
		simulationRenderer->BlackDecorations(!conf.decorations);
		simulationRenderer->RenderMode(conf.renderMode); // * Default is (RENDER_BASC | RENDER_FIRE | RENDER_SPRK | RENDER_EFFE).
		simulationRenderer->DisplayMode(conf.displayMode); // * Default is 0.
		simulationRenderer->ColorMode(conf.colorMode); // * Default is 0.
		auto pasteRectB = conf.pasteRectB;
		auto seamB = 1; // * Increase this if the seams don't mesh properly.
		auto chunkBW = (XRES / CELL) - 2 * seamB;
		auto chunkBH = (YRES / CELL) - 2 * seamB;
		output = gui::Image::FromSize({ pasteRectB.size.x * CELL, pasteRectB.size.y * CELL });
		task.status = false;
		for (auto y = 0; y < pasteRectB.size.y; y += chunkBH)
		{
			for (auto x = 0; x < pasteRectB.size.x; x += chunkBW)
			{
				auto srcRectB  = pasteRectB.Intersect({ { x - seamB, y - seamB }, { chunkBW + 2 * seamB, chunkBH + 2 * seamB } });
				auto destRectB = pasteRectB.Intersect({ { x        , y         }, { chunkBW            , chunkBH             } });
				simulation->clear_sim();
				{
					auto save = std::make_unique<GameSave>(*conf.source);
					save->Expand();
					save->Extract(conf.pasteRotflip, conf.pasteNudge.x, conf.pasteNudge.y, srcRectB.pos.x, srcRectB.pos.y, srcRectB.size.x, srcRectB.size.y);
					auto failed = bool(simulation->Load(save.get(), true));
					if (failed)
					{
						return;
					}
				}
				simulationRenderer->ClearAccumulation();
				auto iterations = 1;
				if (conf.fire)
				{
					iterations += 15;
				}
				for (auto i = 0; i < iterations; ++i)
				{
					// * TODO-REDO_UI: Look into how SaveRenderer::Render differed from the usual rendering. Might have been more optimal.
					simulationRenderer->Render(*simulation);
				}
				auto srcDestOffB = destRectB.pos - srcRectB.pos;
				for (auto y = 0; y < destRectB.size.y * CELL; ++y)
				{
					auto srcPos = gui::Point{ 0, y } + srcDestOffB * CELL;
					auto destPos = gui::Point{ 0, y } + destRectB.pos * CELL;
					auto *destData = output.Pixels() + destPos.y * pasteRectB.size.x * CELL + destPos.x;
					auto *srcData = simulationRenderer->MainBuffer() + srcPos.y * XRES + srcPos.x;
					std::copy(srcData, srcData + destRectB.size.x * CELL, destData);
				}
			}
		}
		task.status = true;
	}

	std::shared_ptr<ThumbnailRendererTask> ThumbnailRendererTask::Create(Configuration conf)
	{
		auto task = std::make_shared<ThumbnailRendererTask>();
		task->conf = conf;
		return task;
	}

	void ThumbnailRendererTask::Process()
	{
		ThumbnailRenderer::Ref().Process(*this);
	}
}
