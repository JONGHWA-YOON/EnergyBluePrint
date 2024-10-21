import graphics.GameViewCamera;

graphics::GameViewCamera* graphics::GameViewCamera::SingleInstance()
{
	if (instance == nullptr)
	{
		instance = new graphics::GameViewCamera();
	}

	return instance;
}

graphics::GameViewCamera* graphics::GameViewCamera::instance = nullptr;

graphics::GameViewCamera::~GameViewCamera()
{

}

graphics::GameViewCamera::GameViewCamera()
	: Camera()
{

}

void graphics::GameViewCamera::Init()
{

}

void graphics::GameViewCamera::Shutdown()
{

}

void graphics::GameViewCamera::OnUpdate(float ts)
{

}