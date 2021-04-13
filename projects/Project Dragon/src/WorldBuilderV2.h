#pragma once
#include <vector>
#include <Scene.h>
#include <RenderingManager.h>
#include <InstantiatingSystem.h>
#include <random>
static class WorldBuilderV2
{
public:
	//
	// -- Data --
	//

	//Stores the GOs in the world, used mainly in DestroyCurrentWorld()
	static std::vector<GameObject> currentWorldGOs;

	//TrueFalse data for the world || helps track if there is already a tile placed on a location
	static int WorldData[25][25];

	//	--	 Use this for changine the node size, will update the world get		-- //
	static const int nodeSize;

	//
	// Entry and Exit Points:
	//		0 = Top | 1 = Bottom | 2 = Right | 3 = Left | 4 = Null
	//
	static int WorldDataEAndEPoints[25][25][1];

	//
	// -- Functions --
	//
#pragma optimize ("", off)
	//Builds a new world for the game.
	static void BuildNewWorld();
#pragma optimize ("", off)
	//Resets the world data, called on by BuildNewWorld()
	static void ResetWorldData();
#pragma optimize ("", off)
	//Destroys all the GOs stored in currentWorldGOs, called on by BuildNewWorld()
	static void DestroyCurrentWorld();
	//Fills the WorldData array
#pragma optimize ("", off)
	static void FillWorldData();
	//Fills the world reletive to the data collected in FillWorldData()
#pragma optimize ("", off)
	static void GenerateTiles();
	//	WIP
	//		Populate the world with enemies (fills the currentEnemy variable)
	
#pragma optimize ("", off)//  WIP
	static void PopulateWorld(int dificulty);
	//Dificualy - 1 is normal, scale off of that
};
