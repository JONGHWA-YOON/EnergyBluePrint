#include "GraphicsPch.h"
#include "IFFGraphics.h"
#include "FFGraphicsEngine.h"

IFFGraphics* CreateFFRenderer()
{
	return new FFGraphicsEngine();
}

void DeleteFFRenderer(IFFGraphics** p_renderer)
{
	delete *p_renderer;
}
