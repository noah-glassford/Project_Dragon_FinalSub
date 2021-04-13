#pragma once
#include "PostEffect.h"
class FilmGrain : public PostEffect
{
public:
	//Initializes framebuffer
	void Init(unsigned width, unsigned height) override;

	//Applies effect to this buffer
	void ApplyEffect(PostEffect* buffer) override;

	float m_Intensity = 1.f;

};