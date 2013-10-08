/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#ifndef GS2D_D3D9_SPRITE_H_
#define GS2D_D3D9_SPRITE_H_

#include "D3D9Video.h"

namespace gs2d {

/**
 * \brief Implements a Direct3D 9 Sprite object
 */
class D3D9Sprite : public Sprite
{
	bool LoadSprite(
		VideoWeakPtr video,
		GS_BYTE *pBuffer,
		const unsigned int bufferLength,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool LoadSprite(
		VideoWeakPtr video,
		const std::wstring& fileName,
		Color mask = constant::ZERO,
		const unsigned int width = 0,
		const unsigned int height = 0);

	bool CreateRenderTarget(
		VideoWeakPtr video,
		const unsigned int width,
		const unsigned int height,
		const Texture::TARGET_FORMAT format = Texture::TF_DEFAULT);

	VideoWeakPtr m_video;
	TexturePtr m_texture;
	math::Vector2 m_size;
	Texture::TARGET_FORMAT m_targetFormat;
	Sprite::TYPE m_type;
	IDirect3DTexture9 *m_pBackupTexture;

	IDirect3DTexture9 *m_pTexture;
	IDirect3DDevice9 *m_pDevice;
	D3D9VideoInfoPtr m_pVideoInfo;

	void Init();
	bool GetInternalData();
	void OnLostDevice();
	void RecoverFromBackup();

public:
	D3D9Sprite();
	~D3D9Sprite();

	bool Draw(
		const math::Vector2& v2Pos,
		const math::Vector4& color,
		const float angle = 0.0f,
		const math::Vector2& v2Scale = math::Vector2(1.0f,1.0f));

	bool DrawShaped(
		const math::Vector2& v2Pos,
		const math::Vector2& v2Size,
		const math::Vector4& color0,
		const math::Vector4& color1,
		const math::Vector4& color2,
		const math::Vector4& color3,
		const float angle = 0.0f);

	bool SaveBitmap(const str_type::char_t* name, const Texture::BITMAP_FORMAT fmt, math::Rect2D *pRect = 0);

	bool DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const math::Vector4& color);

	bool DrawOptimal(
		const math::Vector2& v2Pos,
		const math::Vector4& color,
		const float angle = 0.0f,
		const math::Vector2& v2Size = math::Vector2(-1,-1));

	void BeginFastRendering();
	void EndFastRendering();

	TextureWeakPtr GetTexture();

	Texture::PROFILE GetProfile() const;
	math::Vector2i GetBitmapSize() const;
	math::Vector2 GetBitmapSizeF() const;

	TYPE GetType() const;
	boost::any GetTextureObject();

	void GenerateBackup();
	bool SetAsTexture(const unsigned int passIdx);

	void SetSpriteDensityValue(const float value);
	float GetSpriteDensityValue() const;

	math::Vector2 GetTextureSize() const;
};

typedef boost::shared_ptr<D3D9Sprite> D3D9SpritePtr;
typedef boost::weak_ptr<D3D9Sprite> D3D9SpriteWeakPtr;

} // namespace gs2d

#endif
