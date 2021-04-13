#include <AssetLoader.h>
#include <ObjLoader.h>
#include <RenderingManager.h>
#include <Texture2D.h>

std::vector<std::string> AssetLoader::RendererNames;
std::vector<RendererComponent> AssetLoader::Renderers;
std::vector<std::string> AssetLoader::GameObjectNames;
std::vector<GameObject> AssetLoader::GameObjects;


void AssetLoader::Init() //doing it manually because, actually I don't know why
{
	//basic floor tile

	VertexArrayObject::sptr plane = ObjLoader::LoadFromFile("model/plane.obj", glm::vec4(1, 1, 1, 1));
	VertexArrayObject::sptr wall = ObjLoader::LoadFromFile("model/wall.obj", glm::vec4(1, 1, 1, 1));
	Texture2D::sptr Ice_Tex = Texture2D::LoadFromFile("image/icetexture.png");
	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");

	{
	Texture2D::sptr floor = Texture2D::LoadFromFile("image/floor.png");
	//Texture2D::sptr Wall_Normal = Texture2D::LoadFromFile("image/Wall_Normal_Map.png");
	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
	ShaderMaterial::sptr Floor_Mat = ShaderMaterial::Create();
	Floor_Mat->Shader = RenderingManager::NoOutline;
	Floor_Mat->Set("s_Diffuse", floor);
	Floor_Mat->Set("s_Specular", noSpec);
	Floor_Mat->Set("u_Shininess", 2.0f);
	Floor_Mat->Set("u_TextureMix", 0.0f);
	RendererComponent FloorTile;
	FloorTile.SetMaterial(Floor_Mat).SetMesh(plane);
	FloorTile.CastShadows = false;
	Renderers.push_back(FloorTile);
	RendererNames.push_back("Floor");
}
	{
	ShaderMaterial::sptr Wall_Mat = ShaderMaterial::Create();
	Texture2D::sptr test = Texture2D::LoadFromFile("image/New_Wall_Tex.png");
	Wall_Mat->Shader = RenderingManager::NoOutline;
	Wall_Mat->Set("s_Diffuse", test);
	Wall_Mat->Set("s_Specular", test);
	Wall_Mat->Set("u_Shininess", 2.0f);
	Wall_Mat->Set("u_TextureMix", 0.0f);
	RendererComponent Wall_Tile;
	Wall_Tile.SetMaterial(Wall_Mat).SetMesh(wall);
	Renderers.push_back(Wall_Tile);
	RendererNames.push_back("Wall");
}
	
	{
	//Water Proj
	VertexArrayObject::sptr Ice_Mesh = ObjLoader::LoadFromFile("model/Ice projectile.obj");

	//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
	ShaderMaterial::sptr Ice_Mat = ShaderMaterial::Create();
	Ice_Mat->Shader = RenderingManager::BaseShader;
	Ice_Mat->Set("s_Diffuse", Ice_Tex);
	Ice_Mat->Set("s_Specular", noSpec);
	Ice_Mat->Set("u_Shininess", 2.0f);
	Ice_Mat->Set("u_TextureMix", 0.0f);
	RendererComponent Ice_Proj;
	Ice_Proj.SetMaterial(Ice_Mat).SetMesh(Ice_Mesh);
	Renderers.push_back(Ice_Proj);
	RendererNames.push_back("Water_Proj");

}

	{
		VertexArrayObject::sptr Barrel_Mesh = ObjLoader::LoadFromFile("model/barrel.obj");
		Texture2D::sptr Barrel_Tex = Texture2D::LoadFromFile("image/BARREL.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr Barrel_Mat = ShaderMaterial::Create();
		Barrel_Mat->Shader = RenderingManager::BaseShader;
		Barrel_Mat->Set("s_Diffuse", Barrel_Tex);
		Barrel_Mat->Set("s_Specular", noSpec);
		Barrel_Mat->Set("u_Shininess", 2.0f);
		Barrel_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Barrel;
		Barrel.SetMaterial(Barrel_Mat).SetMesh(Barrel_Mesh);
		Renderers.push_back(Barrel);
		RendererNames.push_back("Barrel");
	}
	//box
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/box.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/box.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::BaseShader;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Box");
	}

	//box
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/Offsetcube.obj");
		//Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/box.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::BaseShader;
		box_Mat->Set("s_Diffuse", Ice_Tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Ice_Tracer");
	}
	//Torch
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/torch.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/TORCH.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::BaseShader;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Torch");
	}
	
	//side buildings
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/side_house.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/side_building.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::BaseShader;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("SideBuildings");
	}

	//side buildings
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/Sphere.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/red.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::BaseShader;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Sphere");
	}
	
	//Pillar
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/Floor_Lamp.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/Floor_Lamp_Tex.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::BaseShader;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Pillar");
	}
	//pillar broken
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/pillar broken.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/pillar.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::BaseShader;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Broken_Pillar");
	}
	//Fire Enemy
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/FE_I_1.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/FE_TEXTURE.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::NoOutline;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Fire_Enemy");
	}

	//Fire Enemy
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/rock projectile.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/FE_TEXTURE.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::NoOutline;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Rock_Projectile");
	}
	//Water Enemy
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/waterenemy.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/WE_TEXTURE.png");
		Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::NoOutline;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Water_Enemy");
	}
	//Earth Enemy
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/Earth_En.obj");
		Texture2D::sptr box_tex = Texture2D::LoadFromFile("image/earthenemytexture.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::NoOutline;
		box_Mat->Set("s_Diffuse", box_tex);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Earth_Enemy");
	}
	//Air Enemy
	{
		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/air_enemy.obj");
		Texture2D::sptr air_text = Texture2D::LoadFromFile("image/air_enemy_texture.png");
		//	Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		ShaderMaterial::sptr box_Mat = ShaderMaterial::Create();
		box_Mat->Shader = RenderingManager::NoOutline;
		box_Mat->Set("s_Diffuse", air_text);
		box_Mat->Set("s_Specular", noSpec);
		box_Mat->Set("u_Shininess", 2.0f);
		box_Mat->Set("u_TextureMix", 0.0f);
		RendererComponent Box;
		Box.SetMaterial(box_Mat).SetMesh(box_Mesh);
		Renderers.push_back(Box);
		RendererNames.push_back("Air_Enemy");
	}


	//model for player hands
	{

		VertexArrayObject::sptr box_Mesh = ObjLoader::LoadFromFile("model/hand_rest.obj");
		static Texture2D::sptr hand = Texture2D::LoadFromFile("image/handtexture.png");
		ShaderMaterial::sptr handMat = ShaderMaterial::Create();
		ShaderMaterial::sptr firehandMat = ShaderMaterial::Create();
		ShaderMaterial::sptr waterhandMat = ShaderMaterial::Create();
		ShaderMaterial::sptr airhandMat = ShaderMaterial::Create();
		handMat->Shader = RenderingManager::BoneAnimShader;
		handMat->Set("s_Diffuse", hand);
		handMat->Set("s_Specular", noSpec);
		handMat->Set("u_Shininess", 1.0f);
		handMat->Set("u_TextureMix", 0.0f);

		RendererComponent rc;
		rc.CastShadows = false;
		rc.SetMaterial(handMat).SetMesh(box_Mesh);
		Renderers.push_back(rc);
		RendererNames.push_back("hands");


	}

	{
		Texture2D::sptr hpBar = Texture2D::LoadFromFile("image/HP_FULL.png");
		//Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		//Material for menu
		ShaderMaterial::sptr HP = ShaderMaterial::Create();
		HP->Shader = RenderingManager::UIShader;
		HP->Set("s_Diffuse", hpBar);
		HP->Set("s_Specular", hpBar);
		HP->Set("u_Shininess", 2.0f);
		HP->Set("u_TextureMix", 0.0f);
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("model/plane.obj");
		RendererComponent hp;
		hp.SetMaterial(HP).SetMesh(vao);
		Renderers.push_back(hp);
		RendererNames.push_back("hpBar");
	}
	

	{
		Texture2D::sptr menu = Texture2D::LoadFromFile("image/deathscreen.png");

		//Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		//Material for menu
		ShaderMaterial::sptr HP = ShaderMaterial::Create();
		HP->Shader = RenderingManager::UIShader;
		HP->Set("s_Diffuse", menu);
		HP->Set("s_Specular", menu);
		HP->Set("u_Shininess", 2.0f);
		HP->Set("u_TextureMix", 0.0f);
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("model/plane.obj");
		RendererComponent hp;
		hp.SetMaterial(HP).SetMesh(vao);
		Renderers.push_back(hp);
		RendererNames.push_back("DeathScreen");
	}

	{
		Texture2D::sptr menu = Texture2D::LoadFromFile("image/MainMenu.png");
		Texture2D::sptr noSpec = Texture2D::LoadFromFile("image/grassSpec.png");
		//Material for menu
		ShaderMaterial::sptr Menu = ShaderMaterial::Create();
		Menu->Shader = RenderingManager::UIShader;
		Menu->Set("s_Diffuse", menu);
		Menu->Set("s_Specular", menu);
		Menu->Set("u_Shininess", 2.0f);
		Menu->Set("u_TextureMix", 0.0f);
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("model/plane.obj");
		RendererComponent hp;
		hp.SetMaterial(Menu).SetMesh(vao);
		Renderers.push_back(hp);
		RendererNames.push_back("MainMenu");
	}

}
RendererComponent& AssetLoader::GetRendererFromStr(std::string name)
{
	for (int i = 0; i < Renderers.size(); i++)
	{
		if (name == RendererNames[i])
		{
			return Renderers[i];
		}
	}
	return Renderers[0]; //backup
}
GameObject& AssetLoader::GetGameObjectFromStr(std::string name)
{
	for (int i = 0; i < GameObjectNames.size(); i++)
	{
		if (name == GameObjectNames[i])
		{
			return GameObjects[i];
		}
	}
	return GameObjects[0]; //backup
}
/*
Mesh& AssetLoader::GetMeshFromStr(std::string name)
{
	// TODO: insert return statement here
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		if (name == m_MeshNames[i])
		{
			return m_Meshes[i];
		}
	}
	return m_Meshes[0];
}

Material& AssetLoader::GetMatFromStr(std::string name)
{
	// TODO: insert return statement here

	for (int i = 0; i < m_Materials.size(); i++)
	{
		if (name == m_MaterialNames[i])
		{
			return m_Materials[i];
		}
	}
	return m_Materials[0];
}
*/