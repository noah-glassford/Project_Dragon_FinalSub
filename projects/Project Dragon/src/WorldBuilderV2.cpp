//
//
//  --  World Builder V2  --
//
//

#include "WorldBuilderV2.h"
#include "PhysicsBody.h"
#include <RendererComponent.h>
#include <AssetLoader.h>
#include <Enemy.h>
#include <LightSource.h>
#include <chrono>


std::vector<GameObject> WorldBuilderV2::currentWorldGOs;
int WorldBuilderV2::WorldData[100][100]{ 0 };
const int WorldBuilderV2::nodeSize = 40;
int WorldBuilderV2::WorldDataEAndEPoints[25][25][1]{ 0 };

void WorldBuilderV2::BuildNewWorld()
{
	auto start = std::chrono::high_resolution_clock::now();

	DestroyCurrentWorld();
	ResetWorldData();
	FillWorldData();

	int temp = 0;
	for each (int i in WorldData) {
		if (i > 0)
			temp += 1;
	}
	std::cout << "\n\n\nWorld Size: " << temp << "\n\n\n";
	GenerateTiles();
	PopulateWorld(1);

	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Time in ms to build world: " << duration.count();
}

void WorldBuilderV2::ResetWorldData()
{
	for each (bool i in WorldData) {
		i = 0;
		std::cout << i << '\n';
	}
}

void WorldBuilderV2::DestroyCurrentWorld()
{
	/*
	for each (GameObject node in currentWorldGOs) {
		std::cout << "Destroying OBJ In: CurrentWorldGos.";
		RenderingManager::activeScene->RemoveEntity(node);
	}
	*/
	currentWorldGOs.clear();
	
}

void WorldBuilderV2::FillWorldData()
{
	srand(time(NULL));

	bool isBuilding = true;
	bool canLeft, canRight, canUp, canDown;

	int currentX = 15, currentY = 15;
	int pastX, pastY;
	int MovementData = 4;

	while (isBuilding)
	{
		canLeft = false; canRight = false; canUp = false; canDown = false;
		//Sets tile at start
		WorldData[currentX][currentY] = 5;
		pastX = currentX; pastY = currentY;

		//Data check
		if (WorldData[currentX + 1][currentY] < 1 && currentX < 99)
			canRight = true;
		if (WorldData[currentX - 1][currentY] < 1 && currentX > 1)
			canLeft = true;
		if (WorldData[currentX][currentY + 1] < 1 && currentY < 99)
			canUp = true;
		if (WorldData[currentX][currentY - 1] < 1 && currentY > 1)
			canDown = true;

		//Exit Check
		if (!canLeft && !canRight && !canUp && !canDown)
			isBuilding = false;
		else {
			bool temp = false;
			while (!temp) {
				switch (rand() % 4) {
				case 0:
					if (canLeft) {
						currentX -= 1;
						MovementData = 3;
						temp = true;
					}
					break;
				case 1:
					if (canRight) {
						currentX += 1;
						MovementData = 2;
						temp = true;
					}
					break;
				case 2:
					if (canUp) {
						currentY += 1;
						MovementData = 0;
						temp = true;
					}
					break;
				case 3:
					if (canDown) {
						currentY -= 1;
						MovementData = 1;
						temp = true;
					}
					break;
				}
			}
		}
	}
}

void WorldBuilderV2::GenerateTiles()
{
	//
	//	TEMP -- Builds the world of empty tiles -- TEMP
	//
	for (int x = 0; x < 100; x++) {
		for (int y = 0; y < 100; y++) {
			if (WorldData[x][y] > 0) { //should place a tile
				//Floor

				//Randomly Selects a tile
				int RandTile = rand() % 4;

				if (RandTile == 0)
					InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, y * nodeSize, 0), "node/Blank_Floor_Tile.node"); //generates tile
				else if (RandTile == 1)
					InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, y * nodeSize, 0), "node/Node_1.node");//generates tile
				else if (RandTile == 2)
					InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, y * nodeSize, 0), "node/Node_2.node");//generates tile
				else if (RandTile == 3)
					InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, y * nodeSize, 0), "node/Node_3.node");//generates tile

				//places the camera properly
				btTransform t;
				t.setIdentity();
				t.setOrigin(btVector3(x * nodeSize, y * nodeSize, 2.f));
				RenderingManager::activeScene->FindFirst("Camera").get<PhysicsBody>().GetBody()->setWorldTransform(t);

				currentWorldGOs.push_back(InstantiatingSystem::m_Instantiated[InstantiatingSystem::m_Instantiated.size() - 1]);

				//Roof
				//InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, y * nodeSize, nodeSize)
				//	, "node/Blank_Floor_Tile.node");

				//
				// Exterior Walls
				//

				int wallRand = rand() % 2;


				if (WorldData[x][y + 1] < 1) {
					InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, (y * nodeSize) + (nodeSize / 2), nodeSize / 2)
						, "node/Blank_Wall_Y.node");
					currentWorldGOs.push_back(InstantiatingSystem::m_Instantiated[InstantiatingSystem::m_Instantiated.size() - 1]);
				}
				if (WorldData[x][y - 1] < 1) {
					if (wallRand == 0)
					{
						InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, (y * nodeSize) - (nodeSize / 2), nodeSize / 2)
							, "node/Blank_Wall_Y.node"); \
							currentWorldGOs.push_back(InstantiatingSystem::m_Instantiated[InstantiatingSystem::m_Instantiated.size() - 1]);
					}
					else
						InstantiatingSystem::LoadPrefabFromFile(glm::vec3(x * nodeSize, (y * nodeSize) - (nodeSize / 2), nodeSize / 2)
						, "node/Wall_House_Y.node"); \
						currentWorldGOs.push_back(InstantiatingSystem::m_Instantiated[InstantiatingSystem::m_Instantiated.size() - 1]);

				}

				if (WorldData[x + 1][y] < 1) {
					InstantiatingSystem::LoadPrefabFromFile(glm::vec3((x * nodeSize) + (nodeSize / 2), y * nodeSize, nodeSize / 2)
						, "node/Blank_Wall_X.node");
					currentWorldGOs.push_back(InstantiatingSystem::m_Instantiated[InstantiatingSystem::m_Instantiated.size() - 1]);

				}

				if (WorldData[x - 1][y] < 1) {
					InstantiatingSystem::LoadPrefabFromFile(glm::vec3((x * nodeSize) - (nodeSize / 2), y * nodeSize, nodeSize / 2)
						, "node/Blank_Wall_X.node");
					currentWorldGOs.push_back(InstantiatingSystem::m_Instantiated[InstantiatingSystem::m_Instantiated.size() - 1]);
				}

				std::cout << "\n[" << x << "][" << y << "]\n";
			}
		}
	}
}
int enCount;
void WorldBuilderV2::PopulateWorld(int dificulty)
{
	for (int x = 0; x < 100; x++) {
		for (int y = 0; y < 100; y++) {	//Read through the world data Array
			if (WorldData[x][y] > 0) {	//Find spawned floors and spawn enemies in them
				int RandEnemyType = rand() % 16;
				if (RandEnemyType == 0) {
					for (int i = 0; i < dificulty * 1; i++) { // Fire Enemy
						int randX = (rand() % 20) - 10;
						int randY = (rand() % 20) - 10;

						GameObject fe = InstantiatingSystem::InstantiateEmpty("FE");
						fe.get<Transform>().SetLocalRotation(90, 0, 0).SetLocalPosition(0, 0, -2).SetLocalScale(0.7,0.7,0.7);
						fe.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Fire_Enemy");
						PhysicsBody& p = fe.emplace<PhysicsBody>();
						Enemy& e = fe.emplace<Enemy>();
						e.m_hp = 25.f;
						p.AddBody(1.f, btVector3(x * nodeSize + randX, y * nodeSize + randY, 10), btVector3(2, 2, 2), 0.5f);
						p.GetBody()->setUserPointer(&fe);
						p.GetBody()->setUserIndex(2);
						p.GetBody()->setUserIndex2(0);
						p.GetBody()->setUserIndex3((unsigned)fe.entity());
					}
				}
				if (RandEnemyType == 1) {
					for (int i = 0; i < dificulty; i++) { // Rock Enemy
						int randX = (rand() % 20) - 10;
						int randY = (rand() % 20) - 10;
						enCount++;
						//Instantiate at glm::vec3((x * 20) + randX, (y * 20) + randY), 10);
						GameObject fe = InstantiatingSystem::InstantiateEmpty("FE");
						fe.get<Transform>().SetLocalRotation(90, 0, 0).SetLocalPosition(0, 0, -2).SetLocalScale(1.2f,1.2f,1.2f);
						fe.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Earth_Enemy");
						PhysicsBody& p = fe.emplace<PhysicsBody>();
						Enemy& e = fe.emplace<Enemy>();
						e.m_hp = 25.f;
						p.AddBody(1.f, btVector3(x * nodeSize + randX, y * nodeSize + randY, 10), btVector3(2, 2, 5), 0.5f);
						p.GetBody()->setUserPointer(&fe);
						p.GetBody()->setUserIndex(2);
						p.GetBody()->setUserIndex2(2);
						p.GetBody()->setUserIndex3((unsigned)fe.entity());
					}
				}
				if (RandEnemyType == 2) {
					for (int i = 0; i < dificulty * 1; i++) { // Water Enemy
						int randX = (rand() % 20) - 10;
						int randY = (rand() % 20) - 10;
						enCount++;
						//Instantiate at glm::vec3((x * 20) + randX, (y * 20) + randY), 10);
						GameObject fe = InstantiatingSystem::InstantiateEmpty("FE");
						fe.get<Transform>().SetLocalRotation(90, 0, 0).SetLocalPosition(0, 0, -2);
						fe.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Water_Enemy");
						PhysicsBody& p = fe.emplace<PhysicsBody>();
						Enemy& e = fe.emplace<Enemy>();
						e.m_hp = 25.f;
						p.AddBody(1.f, btVector3(x * nodeSize + randX, y * nodeSize + randY, 10), btVector3(2, 2, 2), 1.f);
						p.GetBody()->setUserPointer(&fe);
						p.GetBody()->setUserIndex(2);
						p.GetBody()->setUserIndex2(1);
						p.GetBody()->setUserIndex3((unsigned)fe.entity());
					}
				}
				if (RandEnemyType == 3) {
					for (int i = 0; i < dificulty * 1; i++) { // Air Enemy
						int randX = (rand() % 20) - 10;
						int randY = (rand() % 20) - 10;
						//Instantiate at glm::vec3((x * 20) + randX, (y * 20) + randY), 10);
						GameObject fe = InstantiatingSystem::InstantiateEmpty("FE");
						fe.get<Transform>().SetLocalRotation(90, 0, 0).SetLocalPosition(0, 0, -2).SetLocalScale(2.f,2.f,2.f);
						fe.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Air_Enemy");
						PhysicsBody& p = fe.emplace<PhysicsBody>();
						Enemy& e = fe.emplace<Enemy>();
						e.m_hp = 25.f;
						p.AddBody(1.f, btVector3(x * nodeSize + randX, y * nodeSize + randY, 10), btVector3(2, 2, 2), 1.f);
						p.GetBody()->setUserPointer(&fe);
						p.GetBody()->setUserIndex(2);
						p.GetBody()->setUserIndex2(3);
						p.GetBody()->setUserIndex3((unsigned)fe.entity());
					}
				}
			}
		}
	}
}