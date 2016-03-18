#pragma once
class GlutRenderable
{
public:
	GlutRenderable();
	~GlutRenderable();

	virtual void RenderShader() = 0;
	virtual void RenderFixedPipeline() = 0;
};

