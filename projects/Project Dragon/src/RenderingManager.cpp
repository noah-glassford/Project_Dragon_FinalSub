#include "RenderingManager.h"
#include <RendererComponent.h>
#include <Transform.h>

#include "BackendHandler.h"

#include <Player.h>
#include <Enemy.h>
#include <AudioEngine.h>

#include <LightSource.h>

#include <UI.h>
#include <Interpolation.h>
#include <PhysicsSystem.h>
#include <CameraControlBehaviour.h>
#include <WorldBuilderV2.h>
#include <UniformBuffer.h>
#include <DirectionalLight.h>
#include <GLM/gtc/matrix_transform.hpp>
#include <SkinnedMesh.h>
#include <AssetLoader.h>


Shader::sptr RenderingManager::BaseShader = NULL;
Shader::sptr RenderingManager::NoOutline = NULL;
Shader::sptr RenderingManager::SkyBox = NULL;
Shader::sptr RenderingManager::Passthrough = NULL;
Shader::sptr RenderingManager::UIShader = NULL;
Shader::sptr RenderingManager::simpleDepthShader = NULL;
Shader::sptr RenderingManager::BoneAnimShader = NULL;

Framebuffer* RenderingManager::shadowBuf = nullptr;
PostEffect* RenderingManager::postEffect = nullptr;
BloomEffect* RenderingManager::bloomEffect = nullptr;
ColorCorrectionEffect* RenderingManager::colEffect = nullptr;
GBuffer* RenderingManager::gBuffer = nullptr;
IlluminationBuffer* RenderingManager::illuminationBuffer = nullptr;
Framebuffer* RenderingManager::shadowBuffer = nullptr;
FilmGrain* RenderingManager::filmgrain = nullptr;
GameScene::sptr RenderingManager::activeScene;

bool ShouldBloom;
bool ShouldColCorrect;
int LutSelect;
int TextureToggle = 1;
int NumPasses;
float Threshold;
bool shouldFilmGrain;
float grainIntensity;

void RenderingManager::Init()
{
	// GL states
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL); // New 


	BaseShader = Shader::Create();
	//First we initialize our shaders
	BaseShader->LoadShaderPartFromFile("shader/vertex_shader.glsl", GL_VERTEX_SHADER);
	BaseShader->LoadShaderPartFromFile("shader/gbuffer_pass_frag.glsl", GL_FRAGMENT_SHADER); 
	BaseShader->Link();

	NoOutline = Shader::Create();
	//First we initialize our shaders
	NoOutline->LoadShaderPartFromFile("shader/vertex_shader.glsl", GL_VERTEX_SHADER);
	NoOutline->LoadShaderPartFromFile("shader/gbuffer_pass_frag.glsl", GL_FRAGMENT_SHADER);
	NoOutline->Link();

	simpleDepthShader = Shader::Create();
	//First we initialize our shaders
	simpleDepthShader->LoadShaderPartFromFile("shader/simple_depth_vert.glsl", GL_VERTEX_SHADER);
	simpleDepthShader->LoadShaderPartFromFile("shader/simple_depth_frag.glsl", GL_FRAGMENT_SHADER);
	simpleDepthShader->Link();

	//For animated models only
	BoneAnimShader = Shader::Create();
	//First we initialize our shaders
	BoneAnimShader->LoadShaderPartFromFile("shader/skinned_vertex_shader.glsl", GL_VERTEX_SHADER);
	BoneAnimShader->LoadShaderPartFromFile("shader/gbuffer_pass_frag.glsl", GL_FRAGMENT_SHADER);
	BoneAnimShader->Link();

	

	UIShader = Shader::Create();
	UIShader->LoadShaderPartFromFile("shader/ui_vert.glsl", GL_VERTEX_SHADER);
	UIShader->LoadShaderPartFromFile("shader/ui_frag.glsl", GL_FRAGMENT_SHADER);
	UIShader->Link();
	UIShader->SetUniform("u_Scale", glm::vec2(1, 0.1));
	UIShader->SetUniform("u_Offset", glm::vec2(0, 0));

	BaseShader->SetUniform("u_LightAttenuationConstant", 1.f);
	BaseShader->SetUniform("u_LightAttenuationLinear", 0.08f);
	BaseShader->SetUniform("u_LightAttenuationQuadratic", 0.032f);

	BoneAnimShader->SetUniform("u_LightAttenuationConstant", 1.f);
	BoneAnimShader->SetUniform("u_LightAttenuationLinear", 0.08f);
	BoneAnimShader->SetUniform("u_LightAttenuationQuadratic", 0.032f);



	//init attenuation
	NoOutline->SetUniform("u_LightAttenuationConstant", 1.f);
	NoOutline->SetUniform("u_LightAttenuationLinear", 0.08f);
	NoOutline->SetUniform("u_LightAttenuationQuadratic", 0.032f);


	


	

	//creates some IMGUI sliders
	BackendHandler::imGuiCallbacks.push_back([&]() {
		if (ImGui::Button("Toggle Textures"))
		{
			if (TextureToggle == 1)
				TextureToggle = 0;
			else if (TextureToggle == 0)
				TextureToggle = 1;
		}
		if (ImGui::Button("Toggle Bloom"))
		{
			if (ShouldBloom == 1)
				ShouldBloom = 0;
			else if (ShouldBloom == 0)
				ShouldBloom = 1;
		}
		if (ImGui::Button("Toggle Color Correct"))
		{
			if (ShouldColCorrect == 1)
				ShouldColCorrect = 0;
			else if (ShouldColCorrect == 0)
				ShouldColCorrect = 1;
		}
		if (ImGui::Button("Toggle Film Grain"))
		{
			if (shouldFilmGrain == 1)
				shouldFilmGrain = 0;
			else if (shouldFilmGrain == 0)
				shouldFilmGrain = 1;
		}
		
		if (ImGui::SliderInt("For Color Correct: Lut To Choose", &LutSelect, 0, colEffect->_LUTS.size()))
		{
			colEffect->_LUT = colEffect->_LUTS[LutSelect];
		}

		if (ImGui::SliderFloat("For Bloom Effect: Threshold", &Threshold, 0.f, 1.f, "%.3f", 0.01f))
		{
			bloomEffect->SetThreshold(Threshold);
		}
		if (ImGui::SliderFloat("For FilmGrain Effect: Intensity", &grainIntensity, 0.f, 1.f, "%.3f", 0.1f))
		{
			filmgrain->m_Intensity=grainIntensity;
		}
		if (ImGui::SliderInt("For Bloom Effect: Passes", &NumPasses, 0, 100))
		{
			bloomEffect->SetPasses(NumPasses);
		}
		});
	SkyBox = std::make_shared<Shader>();
	//Want to add a test skybox
	// Load our shaders

	SkyBox->LoadShaderPartFromFile("shader/skybox-shader.vert.glsl", GL_VERTEX_SHADER);
	SkyBox->LoadShaderPartFromFile("shader/skybox-shader.frag.glsl", GL_FRAGMENT_SHADER);
	SkyBox->Link();

	Passthrough = std::make_shared<Shader>();

	Passthrough->LoadShaderPartFromFile("shader/passthrough_vert.glsl", GL_VERTEX_SHADER);
	Passthrough->LoadShaderPartFromFile("shader/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
	Passthrough->Link();

}
bool DeathSoundPlayed = false;
//int LightCount;
int enemyCount = 0;
void RenderingManager::Render()
{
	/*
	//Framebuffer* shadowBuf = &activeScene->FindFirst("Shadow Buffer").get<Framebuffer>();
	PostEffect* postEffect = &activeScene->FindFirst("Basic Effect").get<PostEffect>();
	BloomEffect* bloomEffect = &activeScene->FindFirst("Bloom Effect").get<BloomEffect>();
	ColorCorrectionEffect* colEffect = &activeScene->FindFirst("ColorGrading Effect").get<ColorCorrectionEffect>();
	GBuffer* gBuffer = &activeScene->FindFirst("G Buffer").get<GBuffer>();
	IlluminationBuffer* illuminationBuffer = &activeScene->FindFirst("Illumination Buffer").get<IlluminationBuffer>();
	Framebuffer* shadowBuffer = &activeScene->FindFirst("Shadow Buffer").get<Framebuffer>();
	*/

	postEffect->Clear();
	bloomEffect->Clear();
	colEffect->Clear();
	shadowBuffer->Clear();
	gBuffer->Clear();
	illuminationBuffer->Clear();
	filmgrain->Clear();

	glClearColor(0.08f, 0.17f, 0.31f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region Game_Updates

	// Update all world matrices for this frame
	activeScene->Registry().view<Transform>().each([](entt::entity entity, Transform& t) {
		t.UpdateWorldMatrix();
		});

	
	
	
	enemyCount = 0;
	// Update all world enemies for this frame
	activeScene->Registry().view<Enemy, PhysicsBody, Transform>().each([](entt::entity entity, Enemy& e, PhysicsBody& p, Transform& t) {
		enemyCount++;
		e.Update(p);
		if (e.m_hp <= 0.f)
		{

			activeScene->Registry().destroy(entity);
			
		}

		});
	/*
	LightCount = 0;
	//LightSource Lights[150];
	activeScene->Registry().view<Transform, LightSource>().each([&](entt::entity entity, Transform& t, LightSource& l) {
		
		if (LightCount < 150)
		{
			//create the string to send to the shader
			std::string uniformName;
			uniformName = "PointLights[";
			uniformName += std::to_string(LightCount);
			uniformName += "].";
			//this will be the begining, now we just need to add the part of the struct we want to set
			illuminationBuffer->GetShaders()[0]->SetUniform(uniformName + "position", t.GetLocalPosition());
			illuminationBuffer->GetShaders()[0]->SetUniform(uniformName + "ambient", l.m_Ambient);
			illuminationBuffer->GetShaders()[0]->SetUniform(uniformName + "diffuse", l.m_Diffuse);
			illuminationBuffer->GetShaders()[0]->SetUniform(uniformName + "specular", l.m_Specular);

		}

		LightCount++;
		});
*/
	//UniformBuffer lightUniBuf;
	//lightUniBuf.AllocateMemory(sizeof(LightSource) * 150);
	//lightUniBuf.SendData(reinterpret_cast<void*>(&Lights), sizeof(LightSource) * 150);
	//lightUniBuf.Bind(1);
	//illuminationBuffer->SetPointLights(lightUniBuf);

	//NoOutline->SetUniform("u_LightCount", LightCount);
	//BaseShader->SetUniform("u_LightCount", LightCount);
	

	//sets the scale for player HP Bar
	if (BackendHandler::m_ActiveScene == 1)
	{
		float t2 = 0.2 * activeScene->FindFirst("Camera").get<Player>().m_Hp;



		float scaleX = Interpolation::LERP(0, 1, t2);

		activeScene->FindFirst("PlayerHPBar").get<UI>().scale = glm::vec2(scaleX, 1);
	}

	activeScene->Registry().view<Transform, UI, RendererComponent>().each([](entt::entity entity, Transform& t, UI& ui, RendererComponent& rc)
		{
			UIShader->SetUniform("u_Scale", ui.scale);
			UIShader->SetUniform("u_Offset", ui.offset);
		});

	
	activeScene->FindFirst("Camera").get<Player>().Update();
	//temp
	//activeScene->FindFirst("NumberPlane").get<Transform>().LookAt(camTransform.GetLocalPosition());
	
#pragma endregion
	
#pragma region Rendering
	entt::basic_group<entt::entity, entt::exclude_t<>, entt::get_t<Transform>, RendererComponent> renderGroup =
		activeScene->Registry().group<RendererComponent>(entt::get_t<Transform>());
	
	

	DirectionalLight& sun = illuminationBuffer->GetSunRef();
	



	Transform& camTransform = activeScene->FindFirst("Camera").get<Transform>();
	//AudioEngine::Instance().GetListener().SetPosition(camTransform.GetLocalPosition());
	
	//std::cout << camTransform.GetLocalPosition().x << " , " << camTransform.GetLocalPosition().y << std::endl;

	glm::mat4 lightProjectionMatrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, -45.0f, 45.0f);
	glm::mat4 lightViewMatrix = glm::lookAt(camTransform.GetLocalPosition() + glm::vec3(-sun._lightDirection), camTransform.GetLocalPosition(), glm::vec3(0, 0, 1));
	glm::mat4 lightSpaceViewProj = lightProjectionMatrix * lightViewMatrix;

	//Transform& camTransform = activeScene->FindFirst("Camera").get<Transform>();
	glm::mat4 view = glm::inverse(camTransform.LocalTransform());
	glm::mat4 projection = activeScene->FindFirst("Camera").get<Camera>().GetProjection();
	glm::mat4 viewProjection = projection * view;
	//get the camera mat4s



	//Set shadow stuff
	illuminationBuffer->SetLightSpaceViewProj(lightSpaceViewProj);
	glm::vec3 camPos = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
	illuminationBuffer->SetCamPos(camPos);


	renderGroup.sort<RendererComponent>([](const RendererComponent& l, const RendererComponent& r) {
		// Sort by render layer first, higher numbers get drawn last
		if (l.Material->RenderLayer < r.Material->RenderLayer) return true;
		if (l.Material->RenderLayer > r.Material->RenderLayer) return false;

		// Sort by shader pointer next (so materials using the same shader run sequentially where possible)
		if (l.Material->Shader < r.Material->Shader) return true;
		if (l.Material->Shader > r.Material->Shader) return false;

		// Sort by material pointer last (so we can minimize switching between materials)
		if (l.Material < r.Material) return true;
		if (l.Material > r.Material) return false;

		return false;
		});

	// Start by assuming no shader or material is applied
	Shader::sptr current = nullptr;
	ShaderMaterial::sptr currentMat = nullptr;


	


	glViewport(0, 0, 4096, 4096);

	
	
	

	shadowBuffer->Bind();
	simpleDepthShader->Bind();
	// Iterate over the render group components and draw them
	
	renderGroup.each([&](entt::entity e, RendererComponent& renderer, Transform& transform) {
		
		
		//shadowBuf->BindDepthAsTexture(30);
		if(renderer.CastShadows)
		BackendHandler::RenderVAO(simpleDepthShader, renderer.Mesh,glm::mat4(1), transform, lightSpaceViewProj);
		});
		simpleDepthShader->UnBind();
		
	shadowBuffer->Unbind();
	
	int width, height;

	glfwGetWindowSize(BackendHandler::window, &width, &height);

	glViewport(0, 0, width, height);
	postEffect->BindBuffer(0);
	gBuffer->Bind();
	//firstly render gltf animations

	//AssetLoader::GetRendererFromStr("hands").Material->Shader->Bind();
	activeScene->Registry().view<GLTFSkinnedMesh, Transform>().each([&](entt::entity entity, GLTFSkinnedMesh& m, Transform& t) {
		m.UpdateAnimation(m.GetAnimation(0), Timer::dt);
		//Transform& camTransform = activeScene->FindFirst("Camera").get<Transform>();
		//BackendHandler::SetupShaderForFrame(BoneAnimShader, view, projection);
		//m.UpdateAnimation();
		
		BoneAnimShader->SetUniform("u_TextureToggle", TextureToggle);
		m.Draw(BoneAnimShader, viewProjection, (glm::mat4)t.WorldTransform());


		});
	//AssetLoader::GetRendererFromStr("hands").Material->Shader->UnBind();


	/*
	activeScene->Registry().view<GLTFSkinnedMesh, Transform>().each([&viewProjection](entt::entity entity, GLTFSkinnedMesh& m, Transform& t) {
		m.UpdateAnimation(m.GetAnimation(0), Timer::dt);
	

		m.Draw(BoneAnimShader, viewProjection, (glm::mat4)t.WorldTransform());


		});
		*/
	
	renderGroup.each([&](entt::entity e, RendererComponent& renderer, Transform& transform) {
	
		
	
		// If the shader has changed, set up it's uniforms
		if (current != renderer.Material->Shader) {
			current = renderer.Material->Shader;
			current->Bind();
			BackendHandler::SetupShaderForFrame(current, view, projection);
		}
		// If the material has changed, apply it
		if (currentMat != renderer.Material) {
			currentMat = renderer.Material;
			currentMat->Apply();
		}

		// Render the mesh
		renderer.Material->Shader->SetUniform("u_TextureToggle", TextureToggle);
		BackendHandler::RenderVAO(renderer.Material->Shader, renderer.Mesh, viewProjection, transform, lightSpaceViewProj);
		});
	gBuffer->Unbind();

	

	shadowBuffer->BindDepthAsTexture(30);

	illuminationBuffer->ApplyEffect(gBuffer);

	shadowBuffer->UnbindTexture(30);

	illuminationBuffer->DrawToScreen();

	//shadowBuf->UnbindTexture(30);

	postEffect->UnBindBuffer();
	if (ShouldBloom)
	{
		bloomEffect->ApplyEffect(illuminationBuffer);
		bloomEffect->DrawToScreen();
	}
	if (ShouldColCorrect)
	{
		colEffect->ApplyEffect(illuminationBuffer);
		colEffect->DrawToScreen();
	}
	if (shouldFilmGrain)
	{
		filmgrain->ApplyEffect(illuminationBuffer);
		filmgrain->DrawToScreen();
	}
     	BackendHandler::RenderImGui();

		activeScene->Poll();
		glfwSwapBuffers(BackendHandler::window);
#pragma endregion

//This needs to be at the end, risk crashes otherwise since it is deleting entities before the end of the render loop
		
		//std::cout << enemyCount << std::endl;
		if (enemyCount <= 1 && BackendHandler::m_ActiveScene == 1)
		{
			AudioEngine& engine = AudioEngine::Instance();

			AudioEvent& tempEnDeath = engine.GetEvent("Level Complete");
			if (!DeathSoundPlayed)
			{
				DeathSoundPlayed = true;
				tempEnDeath.Play();
			}		

			activeScene->DeleteAllEnts();
			PhysicsSystem::ClearWorld();
			BackendHandler::m_Scenes[1]->InitGameScene();
			InitPostEffects();
			
		}
		

	


}

void RenderingManager::InitPostEffects()
{
	
	postEffect = &activeScene->FindFirst("Basic Effect").get<PostEffect>();
	bloomEffect = &activeScene->FindFirst("Bloom Effect").get<BloomEffect>();
	colEffect = &activeScene->FindFirst("ColorGrading Effect").get<ColorCorrectionEffect>();
	gBuffer = &activeScene->FindFirst("G Buffer").get<GBuffer>();
	illuminationBuffer = &activeScene->FindFirst("Illumination Buffer").get<IlluminationBuffer>();
	shadowBuffer = &activeScene->FindFirst("Shadow Buffer").get<Framebuffer>();
	filmgrain = &activeScene->FindFirst("FilmGrain").get<FilmGrain>();
}
