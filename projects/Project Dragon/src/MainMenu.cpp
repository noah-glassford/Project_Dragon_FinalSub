#include "MainMenu.h"
#include <Transform.h>
#include <Camera.h>
#include <RenderingManager.h>
#include <AssetLoader.h>
#include <Texture2D.h>
#include <ObjLoader.h>
#include <PhysicsBody.h>
#include <Player.h>
#include <PostEffect.h>
#include <ColorCorrection.h>
#include <Bloom.h>
#include <BackendHandler.h>
#include <IBehaviour.h>
#include <CameraControlBehaviour.h>
#include <UI.h>
#include <DirectionalLight.h>
void MainMenuScene::InitGameScene()
{

	GameScene::RegisterComponentType<Camera>();
	GameScene::RegisterComponentType<RendererComponent>();
	

	scene = GameScene::Create("GameScene");
	RenderingManager::activeScene = scene;

	

	GameObject cameraObject = scene->CreateEntity("Camera");
	{
		cameraObject.get<Transform>().SetLocalPosition(0, 0, 0).LookAt(glm::vec3(0, 1, 5));
		cameraObject.emplace<Player>();
		cameraObject.get<Player>().InitWeapons();
		PhysicsBody& p =cameraObject.emplace<PhysicsBody>();
		p.AddBody(0.f, btVector3(1, 1, 1), btVector3(1, 1, 1));
		//cameraObject.get<Transform>().setForward(glm::vec3(0, 0, -1));
		// We'll make our camera a component of the camera object
		Camera& camera = cameraObject.emplace<Camera>();// Camera::Create();
		
		camera.SetPosition(glm::vec3(0, 3, 3));
		camera.SetUp(glm::vec3(0, 0, 1));
		camera.LookAt(glm::vec3(0));
		camera.SetFovDegrees(90.0f); // Set an initial FOV
		camera.SetOrthoHeight(3.0f);

		BehaviourBinding::Bind<CameraControlBehaviour>(cameraObject);
	}
	GameObject UIObject = scene->CreateEntity("Menu");
	{ 
		RendererComponent& r = UIObject.emplace<RendererComponent>();
		r = AssetLoader::GetRendererFromStr("MainMenu");
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("model/plane.obj");
		UI& ui = UIObject.emplace<UI>();
		ui.offset = glm::vec2(0, 0);
		ui.scale = glm::vec2(1, 1);
		
	}
	
	//if these aren't here it crashes lmao
	BloomEffect* bloom;
	GameObject BloomEffectObject = scene->CreateEntity("Bloom Effect");
	{
		int width, height;
		glfwGetWindowSize(BackendHandler::window, &width, &height);
		bloom = &BloomEffectObject.emplace<BloomEffect>();
		bloom->Init(width, height);
		bloom->SetThreshold(0.1f);
	}
	PostEffect* basicEffect;
	GameObject framebufferObject = scene->CreateEntity("Basic Effect");
	{
		int width, height;
		glfwGetWindowSize(BackendHandler::window, &width, &height);

		basicEffect = &framebufferObject.emplace<PostEffect>();
		basicEffect->Init(width, height);
	}
	//color grading effect
	ColorCorrectionEffect* colorEffect;
	GameObject colorEffectObject = scene->CreateEntity("ColorGrading Effect");
	{
		int width, height;
		glfwGetWindowSize(BackendHandler::window, &width, &height);

		colorEffect = &colorEffectObject.emplace<ColorCorrectionEffect>();
		colorEffect->Init(width, height);

		//number here doesn't matter
		colorEffect->LoadLUT("cube/Neutral-512.cube", 0);
		colorEffect->LoadLUT("cube/BrightenedCorrectionwarm.cube", 0);
		colorEffect->LoadLUT("cube/colourcorrectcool.cube", 0);
		colorEffect->LoadLUT("cube/test.cube", 0);
		colorEffect->_LUT = colorEffect->_LUTS[0];
	}

	FilmGrain* filmGrain;
	GameObject filmGrainObj = scene->CreateEntity("FilmGrain");
	{
		int width, height;
		glfwGetWindowSize(BackendHandler::window, &width, &height);

		filmGrain = &filmGrainObj.emplace<FilmGrain>();
		filmGrain->Init(width, height);
	}


	GBuffer* gBuffer;
	IlluminationBuffer* illuminationBuffer;


	int width, height;
	glfwGetWindowSize(BackendHandler::window, &width, &height);

	GameObject gBufferObject = scene->CreateEntity("G Buffer");
	{
		gBuffer = &gBufferObject.emplace<GBuffer>();
		gBuffer->Init(width, height);
	}

	GameObject illuminationbufferObject = scene->CreateEntity("Illumination Buffer");
	{
		illuminationBuffer = &illuminationbufferObject.emplace<IlluminationBuffer>(150);
		illuminationBuffer->Init(width, height);
	}

	Framebuffer* shadowBuffer;
	int shadowWidth = 4096;
	int shadowHeight = 4096;

	GameObject shadowBufferObject = scene->CreateEntity("Shadow Buffer");
	{
		shadowBuffer = &shadowBufferObject.emplace<Framebuffer>();
		shadowBuffer->AddDepthTarget();
		shadowBuffer->Init(shadowWidth, shadowHeight);
	}

	//creates directional light gameobject
	GameObject sunObj = scene->CreateEntity("SUN");
	{
		sunObj.emplace<DirectionalLight>();
	}
	GameObject sunBufferObj = scene->CreateEntity("SunBuffer");
	{
		UniformBuffer& sunBuf = sunBufferObj.emplace<UniformBuffer>();
		sunBuf.AllocateMemory(sizeof(DirectionalLight));
		sunBuf.SendData(reinterpret_cast<void*>(&sunObj.get<DirectionalLight>()), sizeof(DirectionalLight));
		sunBuf.Bind(0);
	}

}
