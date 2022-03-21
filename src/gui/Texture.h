#pragma once
#include "Config.h"

#include "gui/Component.h"
#include "gui/Image.h"
#include "gui/Color.h"

struct SDL_Texture;

namespace gui
{
	class Texture : public Component
	{
	public:
		enum BlendMode
		{
			blendNone,
			blendNormal,
			blendAdd,
		};

	private:
		SDL_Texture *texture = NULL;
		Image image;
		Color modulation = { 255, 255, 255, 255 };
		BlendMode blend = blendNone;

		void UpdateTexture(bool rendererUp);
		void ConfigureTexture();

	public:
		Texture();

		void RendererUp() final override;
		void RendererDown() final override;

		void ImageData(Image newImage);
		const Image &ImageData() const
		{
			return image;
		}

		SDL_Texture *Handle() const
		{
			return texture;
		}

		void Modulation(Color newModulation);
		Color Modulation() const
		{
			return modulation;
		}

		void Blend(BlendMode newBlend);
		BlendMode Blend() const
		{
			return blend;
		}
	};
}
