#pragma once

#include "Config.h"
#include "common/ExplicitSingleton.h"
#include "common/Worker.h"
#include "gui/Point.h"
#include "gui/Rect.h"
#include "gui/Image.h"

#include <memory>
#include <cstdint>

class Simulation;
class SimulationRenderer;
class GameSave;

namespace graphics
{
	class ThumbnailRenderer;

	class ThumbnailRendererTask : public common::WorkerTask
	{
		struct Configuration
		{
			std::shared_ptr<const GameSave> source;
			bool decorations;
			bool fire;
			uint32_t renderMode;
			uint32_t displayMode;
			uint32_t colorMode;
			unsigned int pasteRotflip;
			gui::Point pasteNudge;
			gui::Rect pasteRectB;
		};
		Configuration conf;

		common::Task::Status Process() final override;

	public:
		gui::Image output;

		static std::shared_ptr<ThumbnailRendererTask> Create(Configuration conf);

		friend class ThumbnailRenderer;
	};

	class ThumbnailRenderer : public common::ExplicitSingleton<ThumbnailRenderer>
	{
		std::unique_ptr<Simulation> simulation;
		std::unique_ptr<SimulationRenderer> simulationRenderer;

	public:
		ThumbnailRenderer();
		~ThumbnailRenderer();

		common::Task::Status Process(ThumbnailRendererTask &task);
	};
}
