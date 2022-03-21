#include "Texture.h"

#include "SDLWindow.h"
#include "SDLAssert.h"

namespace gui
{
	Texture::Texture()
	{
		Visible(false);
	}

	void Texture::ConfigureTexture()
	{
		if (texture)
		{
			switch (blend)
			{
			case blendNone:
				SDLASSERTZERO(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE));
				break;

			case blendNormal:
				SDLASSERTZERO(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));
				break;

			case blendAdd:
				SDLASSERTZERO(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD));
				break;

			default:
				break;
			}
			SDLASSERTZERO(SDL_SetTextureAlphaMod(texture, modulation.a));
			SDLASSERTZERO(SDL_SetTextureColorMod(texture, modulation.r, modulation.g, modulation.b));
		}
	}

	void Texture::UpdateTexture(bool rendererUp)
	{
		auto oldSize = Point{ 0, 0 };
		if (texture)
		{
			SDLASSERTZERO(SDL_QueryTexture(texture, NULL, NULL, &oldSize.x, &oldSize.y));
		}
		auto newSize = Point{ 0, 0 };
		if (rendererUp)
		{
			newSize = image.Size();
		}
		if (oldSize != newSize)
		{
			SDL_DestroyTexture(texture);
			texture = NULL;
			if (newSize.x && newSize.y)
			{
				texture = SDLASSERTPTR(SDL_CreateTexture(Renderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, newSize.x, newSize.y));
				ConfigureTexture();
			}
		}
		if (texture)
		{
			SDLASSERTZERO(SDL_UpdateTexture(texture, NULL, image.Pixels(), image.Size().x * sizeof(uint32_t)));
		}
	}

	void Texture::RendererUp()
	{
		UpdateTexture(true);
	}

	void Texture::RendererDown()
	{
		UpdateTexture(false);
	}

	void Texture::ImageData(Image newImage)
	{
		image = newImage;
		UpdateTexture(Renderer());
	}

	void Texture::Modulation(Color newModulation)
	{
		modulation = newModulation;
		ConfigureTexture();
	}

	void Texture::Blend(BlendMode newBlend)
	{
		blend = newBlend;
		ConfigureTexture();
	}
}
