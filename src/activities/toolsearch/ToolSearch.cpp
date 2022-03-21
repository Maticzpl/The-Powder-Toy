#include "ToolSearch.h"

#include "Config.h"
#include "activities/game/Game.h"
#include "activities/game/tool/Tool.h"
#include "activities/game/ToolButton.h"
#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/ScrollPanel.h"
#include "gui/Separator.h"
#include "gui/SDLWindow.h"
#include "gui/Static.h"
#include "gui/TextBox.h"
#include "language/Language.h"

#include <SDL.h>

namespace activities::toolsearch
{
	constexpr auto parentSize = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto simulationSize = gui::Point{ XRES, YRES };
	constexpr auto windowSize = gui::Point{ 236, 298 };

	class ToolSearchButton : public game::ToolButton
	{
		ToolSearch *elementSearch;

	public:
		ToolSearchButton(ToolSearch *elementSearch, String identifier, game::tool::Tool *tool) :
			ToolButton(identifier, tool),
			elementSearch(elementSearch)
		{
		}

		void Select(int toolIndex) final override
		{
			ToolButton::Select(toolIndex);
			elementSearch->Quit();
		}
	};

	ToolSearch::ToolSearch()
	{
		Size(windowSize);
		Position((parentSize - windowSize) / 2);

		{
			auto title = EmplaceChild<gui::Static>();
			title->Position({ 6, 5 });
			title->Size({ windowSize.x - 20, 14 });
			title->Text("DEFAULT_LS_TOOLSEARCH_TITLE"_Ls());
			title->TextColor(gui::Appearance::informationTitle);
			auto sep = EmplaceChild<gui::Separator>();
			sep->Position({ 1, 22 });
			sep->Size({ windowSize.x - 2, 1 });
			auto cancel = EmplaceChild<gui::Button>();
			cancel->Position({ 0, windowSize.y - 16 });
			cancel->Size({ windowSize.x / 2, 16 });
			cancel->Text("DEFAULT_LS_TOOLSEARCH_CANCEL"_Ls());
			cancel->Click([this]() {
				Quit();
			});
			Cancel([cancel]() {
				cancel->TriggerClick();
			});
			auto ok = EmplaceChild<gui::Button>();
			ok->Position({ windowSize.x / 2 - 1, windowSize.y - 16 });
			ok->Size({ windowSize.x - windowSize.x / 2 + 1, 16 });
			ok->Text(String::Build("\bo", "DEFAULT_LS_TOOLSEARCH_OK"_Ls()));
			ok->Click([this]() {
				if (buttons.size())
				{
					buttons[0]->HandleClick(SDL_BUTTON_LEFT);
				}
			});
			Okay([ok]() {
				ok->TriggerClick();
			});
		}

		sp = EmplaceChild<gui::ScrollPanel>().get();
		sp->Position({ 9, 53 });
		sp->Size(windowSize - gui::Point{ 18, 78 });
		sp->ScrollBarVisible(false);

		searchBox = EmplaceChild<gui::TextBox>().get();
		searchBox->Position({ 8, 31 });
		searchBox->Size({ windowSize.x - 16, 17 });
		searchBox->PlaceHolder(String::Build("[", "DEFAULT_LS_TOOLSEARCH_FIELD"_Ls(), "]"));
		searchBox->Change([this]() {
			Update();
		});

		searchBox->Focus();
		Update();
	}

	void ToolSearch::Update()
	{
		auto *spi = sp->Interior();
		for (auto *button : buttons)
		{
			spi->RemoveChild(button);
		}
		buttons.clear();

		auto queryLower = searchBox->Text().ToLower();
		struct Match
		{
			String identifier;
			game::tool::Tool *tool;
			int favouritePriority; // * Relevance by whether the tool is favourited.
			int haystackOrigin;    // * Relevance by origin of haystack.
			int needlePosition;    // * Relevance by position of needle in haystack.
			String toolName;       // * Fallback: order by tool name, whatever ordering that is.

			bool operator <(const Match &other) const
			{
				auto prec = tool->MenuPrecedence();
				auto other_prec = other.tool->MenuPrecedence();
				return std::tie(
					favouritePriority,
					haystackOrigin,
					needlePosition,
					prec,
					toolName
				) < std::tie(
					other.favouritePriority,
					other.haystackOrigin,
					other.needlePosition,
					other_prec,
					other.toolName
				);
			}
		};
		std::map<String, Match> nameToMatch;
		auto push = [&nameToMatch](Match match) {
			auto it = nameToMatch.find(match.toolName);
			if (it == nameToMatch.end())
			{
				nameToMatch.insert(std::make_pair(match.toolName, match));
			}
			else if (match < it->second)
			{
				it->second = match;
			}
		};
		auto pushIfMatches = [push, &queryLower](const String &identifier, game::tool::Tool *tool, const String &infoLower, int haystackRelevance) {
			bool favourite = false; // * TODO-REDO_UI: favourites
			if (infoLower == queryLower)
			{
				push({ identifier, tool, favourite ? 0 : 1, haystackRelevance, 0, tool->Name() });
			}
			if (infoLower.BeginsWith(queryLower))
			{
				push({ identifier, tool, favourite ? 0 : 1, haystackRelevance, 1, tool->Name() });
			}
			if (infoLower.Contains(queryLower))
			{
				push({ identifier, tool, favourite ? 0 : 1, haystackRelevance, 2, tool->Name() });
			}
		};
		for (const auto &identifierToTool : game::Game::Ref().Tools())
		{
			auto &identifier = identifierToTool.first;
			auto *tool = identifierToTool.second.get();
			pushIfMatches(identifier, tool, tool->Name().ToLower(), 0);
			pushIfMatches(identifier, tool, tool->Description().ToLower(), 1);
			if (tool->MenuVisible() && tool->MenuSection() >= 0 && tool->MenuSection() < SC_TOTAL)
			{
				pushIfMatches(identifier, tool, language::Template{ "DEFAULT_LS_GAME_SECTION_" + game::menuSections[tool->MenuSection()].name }().ToLower(), 2);
			}
		}

		std::vector<Match> sortedMatches;
		for (auto nameAndMatch : nameToMatch)
		{
			sortedMatches.push_back(nameAndMatch.second);
		}
		std::sort(sortedMatches.begin(), sortedMatches.end());

		auto posInit = gui::Point{ 1, 1 };
		auto pos = posInit;
		auto rows = 0;
		for (auto &match : sortedMatches)
		{
			auto *button = spi->EmplaceChild<ToolSearchButton>(this, match.identifier, match.tool).get();
			if (pos.x == posInit.x)
			{
				rows += 1;
			}
			button->Position(pos);
			button->ToolTipPosition({ simulationSize.x - gui::SDLWindow::Ref().TextSize(match.tool->Description()).x - 9, simulationSize.y - 12 });
			pos.x += 31;
			if (pos.x + 31 > sp->Size().x)
			{
				pos.x = posInit.x;
				pos.y += 19;
			}
			buttons.push_back(button);
		}
		spi->Size({ sp->Size().x, 1 + 19 * rows });
	}

	void ToolSearch::Draw() const
	{
		PopupWindow::Draw();
		gui::SDLWindow::Ref().DrawRectOutline({ sp->AbsolutePosition() - gui::Point{ 1, 1 }, sp->Size() + gui::Point{ 2, 2 } }, gui::Appearance::colors.inactive.idle.border);
	}
}
