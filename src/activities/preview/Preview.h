#pragma once
#include "Config.h"

#include "gui/PopupWindow.h"
#include "backend/SaveIDV.h"
#include "backend/SaveInfo.h"
#include "backend/CommentInfo.h"

#include <memory>
#include <map>
#include <functional>

class GameSave;

namespace graphics
{
	class ThumbnailRendererTask;
}

namespace backend
{
	class GetComments;
	class GetSaveInfo;
	class GetSaveData;
	class GetPTI;
	class FavorSave;
	class DeleteSave;
}

namespace gui
{
	class Static;
	class ImageButton;
	class Spinner;
	class ScrollPanel;
	class TextBox;
	class Pagination;
	class Label;
}

namespace activities::preview
{
	class Preview : public gui::PopupWindow
	{
	public:
		using SuccessCallback = std::function<void ()>;
		using DeletedCallback = std::function<void ()>;

	private:
		backend::SaveIDV idv;
		std::shared_ptr<backend::GetPTI> getThumbnail;
		std::shared_ptr<backend::GetSaveData> getSaveData;
		std::shared_ptr<backend::FavorSave> favorSave;
		std::shared_ptr<backend::DeleteSave> deleteSave;
		std::shared_ptr<graphics::ThumbnailRendererTask> saveRender;
		backend::SaveInfo saveInfo{};
		std::shared_ptr<GameSave> saveData;
		gui::Button *open = nullptr;
		gui::Button *favor = nullptr;
		gui::ImageButton *preview = nullptr;
		gui::Spinner *previewSpinner = nullptr;
		gui::Spinner *commentSpinner = nullptr;
		gui::ScrollPanel *commentPanel = nullptr;
		gui::TextBox *commentBox = nullptr;
		bool canManage = false;
		bool canUnpublish = false;
		bool commentPrevPage = false;
		String saveAuthor;

		SuccessCallback success;
		DeletedCallback deleted;

		std::shared_ptr<backend::GetSaveInfo> getSaveInfo;
		bool saveInfoWantCommon = true;
		bool saveInfoWantFavorite = true;
		void SaveInfoStart();
		void SaveInfoFinish();

		void UpdateTasks();
		void UpdateOpen();

		gui::Pagination *commentPagination = nullptr;
		void RefreshComments();

		struct CommentBlock
		{
			gui::Component *panel = nullptr;
			gui::Label *name = nullptr;
			bool markIfFromAuthor = false;
			String username;
		};
		std::vector<CommentBlock> commentBlocks;
		std::shared_ptr<backend::GetComments> getComments;
		void CommentsStart(int newPage, bool resetPageBox);
		void CommentsFinish();
		void UpdateAuthorComments();

	public:
		Preview(backend::SaveIDV newIdv, std::shared_ptr<backend::GetPTI> inheritedGetThumbnail = nullptr);

		void Tick() final override;

		void Success(SuccessCallback newSuccess)
		{
			success = newSuccess;
		}
		SuccessCallback Success() const
		{
			return success;
		}

		void Deleted(DeletedCallback newDeleted)
		{
			deleted = newDeleted;
		}
		DeletedCallback Deleted() const
		{
			return deleted;
		}

		static String LowResImagePath(backend::SaveIDV idv);
	};
}
