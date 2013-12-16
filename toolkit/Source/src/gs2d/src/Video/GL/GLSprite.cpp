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

#include "GLSprite.h"

#include "../../Platform/Platform.h"

namespace gs2d {

bool GLSprite::LoadSprite(
	VideoWeakPtr video,
	GS_BYTE* pBuffer,
	const unsigned int bufferLength,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	m_video = boost::dynamic_pointer_cast<GLVideo>(video.lock());

	TexturePtr tex = m_video.lock()->CreateTextureFromFileInMemory(pBuffer, bufferLength, mask, width, height, 0);
	m_texture = boost::dynamic_pointer_cast<GLTexture>(tex);
	if (!m_texture)
		 return false;

	m_type = Sprite::T_BITMAP;
	Texture::PROFILE profile = m_texture->GetProfile();
	m_bitmapSize = math::Vector2(static_cast<float>(profile.width), static_cast<float>(profile.height));

	SetupSpriteRects(1, 1);
	return true;
}

bool GLSprite::LoadSprite(
	VideoWeakPtr video,
	const str_type::string& fileName,
	Color mask,
	const unsigned int width,
	const unsigned int height)
{
	m_video = boost::dynamic_pointer_cast<GLVideo>(video.lock());

	TexturePtr tex = m_video.lock()->LoadTextureFromFile(fileName, mask, width, height, 0);
	m_texture = boost::dynamic_pointer_cast<GLTexture>(tex);
	if (!m_texture)
		 return false;

	m_type = Sprite::T_BITMAP;
	Texture::PROFILE profile = m_texture->GetProfile();
	m_bitmapSize = math::Vector2(static_cast<float>(profile.width), static_cast<float>(profile.height));

	SetupSpriteRects(1, 1);
	return true;
}

bool GLSprite::CreateRenderTarget(
	VideoWeakPtr video,
	const unsigned int width,
	const unsigned int height,
	const Texture::TARGET_FORMAT format)
{
	m_video = boost::dynamic_pointer_cast<GLVideo>(video.lock());

	m_texture = boost::dynamic_pointer_cast<GLTexture>(m_video.lock()->CreateRenderTargetTexture(width, height, format));
	m_bitmapSize = math::Vector2(static_cast<float>(width), static_cast<float>(height));
	m_type = T_TARGET;
	SetupSpriteRects(1, 1);
	return true;
}

Texture::PROFILE GLSprite::GetProfile() const
{
	return m_texture->GetProfile();
}

TextureWeakPtr GLSprite::GetTexture()
{
	return m_texture;
}

Sprite::TYPE GLSprite::GetType() const
{
	return m_type;
}

math::Vector2i GLSprite::GetBitmapSize() const
{
	return GetBitmapSizeF().ToVector2i();
}

math::Vector2 GLSprite::GetBitmapSizeF() const
{
	return m_bitmapSize;
}

bool GLSprite::DrawShaped(
	const math::Vector2 &v2Pos,
	const math::Vector2 &v2Size,
	const math::Vector4& color0,
	const math::Vector4& color1,
	const math::Vector4& color2,
	const math::Vector4& color3,
	const float angle)
{
	if (v2Size == math::Vector2(0,0))
	{
		return true;
	}

	// compute flip parameters that will be sent to the VS
	math::Vector2 flipMul, flipAdd;
	GetFlipShaderParameters(flipAdd, flipMul);

	// centralizes the sprite according to the origin
	math::Vector2 v2Center = m_normalizedOrigin * v2Size;

	GLVideo* video = m_video.lock().get();
	ShaderPtr pCurrentVS = video->GetVertexShader();

	math::Matrix4x4 mRot;
	if (angle != 0.0f)
		mRot = math::RotateZ(math::DegreeToRadian(angle));
	pCurrentVS->SetMatrixConstant("rotationMatrix", mRot);

	// rounds up the final position to avoid alpha distortion
	math::Vector2 v2FinalPos;
	if (video->IsRoundingUpPosition())
	{
		v2FinalPos.x = floor(v2Pos.x);
		v2FinalPos.y = floor(v2Pos.y);
	}
	else
	{
		v2FinalPos = v2Pos;
	}

	pCurrentVS->SetConstant("size", v2Size);
	pCurrentVS->SetConstant("entityPos", v2FinalPos);
	pCurrentVS->SetConstant("center", v2Center);
	pCurrentVS->SetConstant("flipMul", flipMul);
	pCurrentVS->SetConstant("flipAdd", flipAdd);
	pCurrentVS->SetConstant("bitmapSize", GetBitmapSizeF());
	pCurrentVS->SetConstant("scroll", GetScroll());
	pCurrentVS->SetConstant("multiply", GetMultiply());

	const bool setCameraPos = pCurrentVS->ConstantExist("cameraPos");
	if (setCameraPos)
		pCurrentVS->SetConstant("cameraPos", video->GetCameraPos());

	if (m_rect.size.x == 0 || m_rect.size.y == 0)
	{
		pCurrentVS->SetConstant("rectSize", GetBitmapSizeF());
		pCurrentVS->SetConstant("rectPos", 0, 0);
	}
	else
	{
		pCurrentVS->SetConstant("rectSize", m_rect.size);
		pCurrentVS->SetConstant("rectPos", m_rect.pos);
	}

	pCurrentVS->SetConstant("color0", color0);
	pCurrentVS->SetConstant("color1", color1);
	pCurrentVS->SetConstant("color2", color2);
	pCurrentVS->SetConstant("color3", color3);

	if (pCurrentVS->ConstantExist("depth"))
		pCurrentVS->SetConstant("depth", video->GetSpriteDepth());

	// apply textures according to the rendering mode (pixel shaded or not)
	ShaderPtr pCurrentPS = video->GetPixelShader();
	pCurrentPS->SetTexture("diffuse", m_texture);

	pCurrentVS->SetShader();
	pCurrentPS->SetShader();

	// draw the one-pixel-quad applying the vertex shader
	video->GetRectRenderer().Draw(m_rectMode);

	return true;
}

bool GLSprite::Draw(
	const math::Vector2& v2Pos,
	const math::Vector4& color,
	const float angle,
	const math::Vector2& v2Scale)
{
	const math::Vector2 v2Size(GetFrameSize() * v2Scale);
	return DrawShaped(v2Pos, v2Size, color, color, color, color, angle);
}

bool GLSprite::DrawOptimal(
	const math::Vector2 &v2Pos,
	const math::Vector4& color,
	const float angle,
	const math::Vector2 &v2Size)
{
	return DrawShaped(v2Pos, v2Size, color, color, color, color, angle);
}

void GLSprite::SetSpriteDensityValue(const float value)
{
	if (m_type == T_TARGET)
		return;

	Texture::PROFILE profile = m_texture->GetProfile();
	m_bitmapSize = math::Vector2(static_cast<float>(profile.width) / value, static_cast<float>(profile.height) / value);
	m_densityValue = value;
	SetupSpriteRects(1, 1);
}

float GLSprite::GetSpriteDensityValue() const
{
	return m_densityValue;
}

boost::any GLSprite::GetTextureObject()
{
	return m_texture;
}

void GLSprite::BeginFastRendering()
{
	GLVideo* video = m_video.lock().get();
	video->SetVertexShader(video->GetFontShader());
	ShaderPtr pCurrentVS = video->GetVertexShader();
	pCurrentVS->SetConstant("bitmapSize", GetBitmapSizeF());

	// apply textures according to the rendering mode (pixel shaded or not)
	ShaderPtr pCurrentPS = video->GetPixelShader();
	pCurrentPS->SetTexture("diffuse", m_texture);
	pCurrentPS->SetShader();
}

bool GLSprite::DrawShapedFast(const math::Vector2 &v2Pos, const math::Vector2 &v2Size, const math::Vector4& color)
{
	if (v2Size == math::Vector2(0,0))
	{
		return true;
	}

	GLVideo* video = m_video.lock().get();
	ShaderPtr pCurrentVS = video->GetVertexShader();

	// rounds up the final position to avoid alpha distortion
	math::Vector2 v2FinalPos;
	if (video->IsRoundingUpPosition())
	{
		v2FinalPos.x = floor(v2Pos.x);
		v2FinalPos.y = floor(v2Pos.y);
	}
	else
	{
		v2FinalPos = v2Pos;
	}

	pCurrentVS->SetConstant("size", v2Size);
	pCurrentVS->SetConstant("entityPos", v2FinalPos);
	pCurrentVS->SetConstant("color0", color);

	if (m_rect.size.x == 0 || m_rect.size.y == 0)
	{
		pCurrentVS->SetConstant("rectSize", GetBitmapSizeF());
		pCurrentVS->SetConstant("rectPos", 0, 0);
	}
	else
	{
		pCurrentVS->SetConstant("rectSize", m_rect.size);
		pCurrentVS->SetConstant("rectPos", m_rect.pos);
	}

	pCurrentVS->SetShader();

	video->GetRectRenderer().Draw(m_rectMode);
	return true;
}

void GLSprite::EndFastRendering()
{
	m_video.lock()->SetVertexShader(ShaderPtr());
	m_video.lock()->SetPixelShader(ShaderPtr());
}

bool GLSprite::SaveBitmap(
	const str_type::char_t* name,
	const Texture::BITMAP_FORMAT fmt,
	math::Rect2D* pRect)
{
	return m_texture->SaveBitmap(name, fmt);
}

bool GLSprite::SetAsTexture(const unsigned int passIdx)
{
	GLVideoPtr video = m_video.lock();
	if (passIdx == 1 && video->GetPixelShader() == video->GetDefaultPS())
	{
		ShaderPtr defaultPS = (video->GetBlendMode(1) == Video::BM_MODULATE)
								? video->GetDefaultModulatePS()
								: video->GetDefaultAddPS();
		video->SetPixelShader(defaultPS);
		defaultPS->SetTexture("pass1", m_texture);
	}
	return true;
}

void GLSprite::GenerateBackup()
{
	m_texture->SaveTargetSurfaceBackup();
}

void GLSprite::RecoverFromBackup()
{
	m_texture->Recover();
}

void GLSprite::OnLostDevice()
{
	// TODO
}

} // namespace gs2d
