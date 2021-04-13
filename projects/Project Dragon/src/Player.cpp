#include <Player.h>
#include <BtToGlm.h>
#include <PhysicsSystem.h>
#include <Enemy.h>
#include <RendererComponent.h>
#include <random>
#include <AssetLoader.h>

void Player::InitWeapons()
{
	m_RightHandWeapons.push_back(new FireWeapon());
	m_RightHandWeapons.push_back(new WaterWeapon());
	m_LeftHandWeapons.push_back(new AirWeapon());
	m_LeftHandWeapons.push_back(new EarthWeapon());

	m_RightHandWeapons[0]->m_FireDelay = 3;
	m_RightHandWeapons[1]->m_FireDelay = 3;
	m_LeftHandWeapons[0]->m_FireDelay = 2;
	m_LeftHandWeapons[1]->m_FireDelay = 1.f;

}


bool FireWeapon::Fire()
{
	
	if (m_CanShoot)
	{
		AudioEngine& engine = AudioEngine::Instance();
		AudioEvent& ShootSound = engine.GetEvent("Fire Shoot");
		ShootSound.Play();
	//not needed at the moment (its broken lol)
		m_CanShoot = false;
		m_Timer = 0.f;

		GameObject cameraObj = RenderingManager::activeScene->FindFirst("Camera");//will be used often
		
		//GameObject cameraObj = RenderingManager::activeScene->FindFirst("Camera");
		Transform t = cameraObj.get<Transform>();
		glm::mat4 tempView = glm::inverse(t.WorldTransform());

		double dArray[16] = { 0.0 }; 


		//This here gives us a lookAt vector
		const float* pSource = (const float*)glm::value_ptr(tempView);
		for (int i = 0; i < 16; ++i)
			dArray[i] = pSource[i];

		glm::vec3 lookDir;
		lookDir.x = -dArray[2];
		lookDir.y = -dArray[6];
		lookDir.z = -dArray[10];
		
		GameObject projectile = InstantiatingSystem::InstantiateEmpty("FW_Projectile");
		RendererComponent& rc = projectile.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Rock_Proj");
		PhysicsBody& p = projectile.emplace<PhysicsBody>();
		p.AddBody(1.f, cameraObj.get<PhysicsBody>().GetBody()->getCenterOfMassTransform().getOrigin() + BtToGlm::GLMTOBTV3(lookDir) * 5.f, btVector3(0.5,0.5,0.5), 0.f);
		p.GetBody()->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		p.GetBody()->setUserIndex(3);
		p.GetBody()->setUserIndex3((unsigned)projectile.entity());


		p.GetBody()->applyCentralImpulse(BtToGlm::GLMTOBTV3(lookDir) * 70.f);

		//the damage and collision check will happen in physicssystem.cpp in the callback func
		

	}
	
	return false;
}

bool WaterWeapon::Fire()
{
	AudioEngine& engine = AudioEngine::Instance();
	AudioEvent& ShootSound = engine.GetEvent("Ice Shoot");
	
	if (m_CanShoot)
	{
		ShootSound.Play();
		m_CanShoot = false;
		m_Timer = 0.f;


		GameObject cameraObj = RenderingManager::activeScene->FindFirst("Camera");
		Transform t = cameraObj.get<Transform>();
		glm::mat4 tempView = glm::inverse(t.WorldTransform());

		double dArray[16] = { 0.0 };


		//This here gives us a lookAt vector
		const float* pSource = (const float*)glm::value_ptr(tempView);
		for (int i = 0; i < 16; ++i)
			dArray[i] = pSource[i];

		glm::vec3 lookDir;
		lookDir.x = -dArray[2];
		lookDir.y = -dArray[6];
		lookDir.z = -dArray[10];

		//Grabs player position
		btVector3 playerPosition = RenderingManager::activeScene->FindFirst("Camera").get<PhysicsBody>().GetBody()->getCenterOfMassTransform().getOrigin();

		//construct our raycast vector for shooting

		//make laser to show where shot going
		//ITS SO TEMPORARY
		GameObject tracer = InstantiatingSystem::InstantiateEmpty("Shot_Tracer");
		tracer.emplace<Weap_Laser>();
		tracer.get<Weap_Laser>().m_Entity = tracer.entity();
		tracer.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Ice_Tracer");
		//creates a slight offset to make sure you can see where it goes
		glm::vec3 Offset = glm::vec3(1, 0, 0);
		tracer.get<Transform>().SetLocalPosition(t.GetLocalPosition() + glm::normalize(lookDir)* 25.f + Offset);
		tracer.get<Transform>().SetLocalScale(0.1f, 0.1f, 25);
		tracer.get<Transform>().SetLocalRotation(t.GetLocalRotation());
		cameraObj.get<Player>().m_Lasers.push_back(tracer.get<Weap_Laser>());
		cameraObj.get<Player>().m_Lasers[cameraObj.get<Player>().m_Lasers.size() -1 ].PlaceInVec = cameraObj.get<Player>().m_Lasers.size();


		lookDir *= 50;
		btVector3 to = BtToGlm::GLMTOBTV3(lookDir);
		to += playerPosition;

		btCollisionWorld::ClosestRayResultCallback Results(playerPosition, to);

		PhysicsSystem::GetWorld()->rayTest(playerPosition, to, Results);

		if (Results.hasHit() && Results.m_collisionObject->getUserIndex() == 2) //if this is run you hit an enemy
		{
			//Instantiate projectile/marker of where you shot because hitscan
			//InstantiatingSystem::LoadPrefabFromFile(glm::vec3(BtToGlm::BTTOGLMV3(Results.m_collisionObject->getWorldTransform().getOrigin())), "node/Water_Proj.node");
			AudioEngine& engine = AudioEngine::Instance();
			AudioEvent& HitMarker = engine.GetEvent("Hitmarker");
			HitMarker.Play();

			entt::registry& reg = RenderingManager::activeScene->Registry();

			if (Results.m_collisionObject->getUserIndex2() == 3)
			{
				if (reg.valid((entt::entity)Results.m_collisionObject->getUserIndex3()))
					if (reg.has<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()))
						reg.get<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()).m_hp -= 15;
			}
			else
			{

				if (reg.valid((entt::entity)Results.m_collisionObject->getUserIndex3()))
					if (reg.has<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()))
						reg.get<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()).m_hp -= 10;
			}

			return true;
		}
		else
		{
			//InstantiatingSystem::LoadPrefabFromFile(BtToGlm::BTTOGLMV3(to), "node/Water_Proj.node");
			//ECS::Get<Transform>(2).SetPosition(BtToGlm::BTTOGLMV3(to));
			return false;
		}
	}
	else
	{
		//	std::cout << "Not Shot\n";
		return false;

	}

	return false;
}

bool AirWeapon::Fire()
{
	AudioEngine& engine = AudioEngine::Instance();
	AudioEvent& ShootSound = engine.GetEvent("Air Shoot");

	if (m_CanShoot)
	{
		ShootSound.Play();
		m_CanShoot = false;
		m_Timer = 0.f;
	


		GameObject cameraObj = RenderingManager::activeScene->FindFirst("Camera");
		Transform t = cameraObj.get<Transform>();
		glm::mat4 tempView = glm::inverse(t.WorldTransform());

		double dArray[16] = { 0.0 };


		//This here gives us a lookAt vector
		const float* pSource = (const float*)glm::value_ptr(tempView);
		for (int i = 0; i < 16; ++i)
			dArray[i] = pSource[i];

		glm::vec3 lookDir;
		lookDir.x = -dArray[2];
		lookDir.y = -dArray[6];
		lookDir.z = -dArray[10];

		//Grabs player position
		btVector3 playerPosition = RenderingManager::activeScene->FindFirst("Camera").get<PhysicsBody>().GetBody()->getCenterOfMassTransform().getOrigin();

		//construct our raycast vector for shooting
		lookDir *= 10;
		btVector3 to = BtToGlm::GLMTOBTV3(lookDir);
		to += playerPosition;

		//since a shotgun shoots multiple pellets in a spread, this loop will run a set number of raytests and use rand to make the trajectory spread
		int i = 0;
		while (i < 10)
		{
			btVector3 tempVec = to;
			//std::cout << i;
			float PathDeviation = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); //this returns a random float between 0 and 1
			//lets pretend this is something fancy
			
			if (i % 1 == 0)
			tempVec.setX(tempVec.getX() + PathDeviation * 2);
			if (i % 2 == 0)
			tempVec.setY(tempVec.getY() + PathDeviation * 2);
			if (i % 3 == 0)
			tempVec.setZ(tempVec.getZ() + PathDeviation * 2);


			GameObject tracer = InstantiatingSystem::InstantiateEmpty("Shot_Tracer");
			tracer.emplace<Weap_Laser>();
			tracer.get<Weap_Laser>().m_Entity = tracer.entity();
			tracer.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Ice_Tracer");
			//creates a slight offset to make sure you can see where it goes
			glm::vec3 Offset = glm::vec3(1, 0, 0);
			tracer.get<Transform>().SetLocalPosition(t.GetLocalPosition() + glm::normalize(BtToGlm::BTTOGLMV3(tempVec)) * 5.f);
			tracer.get<Transform>().SetLocalScale(0.1f, 0.1f, 5);
			tracer.get<Transform>().SetLocalRotation(t.GetLocalRotation());
			cameraObj.get<Player>().m_Lasers.push_back(tracer.get<Weap_Laser>());
			cameraObj.get<Player>().m_Lasers[cameraObj.get<Player>().m_Lasers.size() - 1].PlaceInVec = cameraObj.get<Player>().m_Lasers.size();

			btCollisionWorld::ClosestRayResultCallback Results(playerPosition, tempVec);

			PhysicsSystem::GetWorld()->rayTest(playerPosition, tempVec, Results);

			if (Results.hasHit() && Results.m_collisionObject->getUserIndex() == 2) //if this is run you hit an enemy
			{
				//Instantiate projectile/marker of where you shot because hitscan
				//InstantiatingSystem::LoadPrefabFromFile(glm::vec3(BtToGlm::BTTOGLMV3(Results.m_collisionObject->getWorldTransform().getOrigin())), "node/Water_Proj.node");

				AudioEngine& engine = AudioEngine::Instance();
				AudioEvent& HitMarker = engine.GetEvent("Hitmarker");
				HitMarker.Play();

				entt::registry& reg = RenderingManager::activeScene->Registry();

				if (Results.m_collisionObject->getUserIndex2() == 3)
				{
					if (reg.valid((entt::entity)Results.m_collisionObject->getUserIndex3()))
						if (reg.has<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()))
							reg.get<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()).m_hp -= 5;
				}
				else
				{

					if (reg.valid((entt::entity)Results.m_collisionObject->getUserIndex3()))
						if (reg.has<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()))
							reg.get<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()).m_hp -= 2;
				}

			}
			else
			{
				//InstantiatingSystem::LoadPrefabFromFile(BtToGlm::BTTOGLMV3(tempVec), "node/Water_Proj.node");
				//ECS::Get<Transform>(2).SetPosition(BtToGlm::BTTOGLMV3(to));
				//return false;
			}
			i++;
		}
	}
	else
	{
		//	std::cout << "Not Shot\n";
		return false;

	}

	return false;
}

void EarthWeapon::Update()
{
	m_Timer += Timer::dt * 10.f; 
	if (m_Timer >= m_FireDelay) 
		m_CanShoot = true; 
	else m_CanShoot = false;
}

bool EarthWeapon::Fire()
{
	AudioEngine& engine = AudioEngine::Instance();
	AudioEvent& ShootSound = engine.GetEvent("Ground Shoot");

	if (m_CanShoot)
	{
		ShootSound.Play();
		m_CanShoot = false;
		m_Timer = 0.f;


		GameObject cameraObj = RenderingManager::activeScene->FindFirst("Camera");
		Transform t = cameraObj.get<Transform>();
		glm::mat4 tempView = glm::inverse(t.WorldTransform());

		double dArray[16] = { 0.0 };


		//This here gives us a lookAt vector
		const float* pSource = (const float*)glm::value_ptr(tempView);
		for (int i = 0; i < 16; ++i)
			dArray[i] = pSource[i];

		glm::vec3 lookDir;
		lookDir.x = -dArray[2];
		lookDir.y = -dArray[6];
		lookDir.z = -dArray[10];

		//Grabs player position
		btVector3 playerPosition = RenderingManager::activeScene->FindFirst("Camera").get<PhysicsBody>().GetBody()->getCenterOfMassTransform().getOrigin();

		GameObject tracer = InstantiatingSystem::InstantiateEmpty("Shot_Tracer");
		tracer.emplace<Weap_Laser>();
		tracer.get<Weap_Laser>().m_Entity = tracer.entity();
		tracer.emplace<RendererComponent>() = AssetLoader::GetRendererFromStr("Ice_Tracer");
		//creates a slight offset to make sure you can see where it goes
		glm::vec3 Offset = glm::vec3(1, 0, 0);
		tracer.get<Transform>().SetLocalPosition(t.GetLocalPosition() + glm::normalize(lookDir) * 15.f );
		tracer.get<Transform>().SetLocalScale(0.1f, 0.1f, 15);
		tracer.get<Transform>().SetLocalRotation(t.GetLocalRotation());
		cameraObj.get<Player>().m_Lasers.push_back(tracer.get<Weap_Laser>());
		cameraObj.get<Player>().m_Lasers[cameraObj.get<Player>().m_Lasers.size() - 1].PlaceInVec = cameraObj.get<Player>().m_Lasers.size();

		//construct our raycast vector for shooting
		lookDir *= 30;
		btVector3 to = BtToGlm::GLMTOBTV3(lookDir);
		to += playerPosition;

		btCollisionWorld::ClosestRayResultCallback Results(playerPosition, to);

		PhysicsSystem::GetWorld()->rayTest(playerPosition, to, Results);

		if (Results.hasHit() && Results.m_collisionObject->getUserIndex() == 2) //if this is run you hit an enemy
		{
			//Instantiate projectile/marker of where you shot because hitscan
			//InstantiatingSystem::LoadPrefabFromFile(glm::vec3(BtToGlm::BTTOGLMV3(Results.m_collisionObject->getWorldTransform().getOrigin())), "node/Water_Proj.node");
			AudioEngine& engine = AudioEngine::Instance();
			AudioEvent& HitMarker = engine.GetEvent("Hitmarker");
			HitMarker.Play();
			
			//does damage to enemy

			entt::registry& reg = RenderingManager::activeScene->Registry();

			if (Results.m_collisionObject->getUserIndex2() == 3)
			{
				if (reg.valid((entt::entity)Results.m_collisionObject->getUserIndex3())) 
					if (reg.has<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()))
				reg.get<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()).m_hp -= 5;
			}
			else
			{

				if (reg.valid((entt::entity)Results.m_collisionObject->getUserIndex3()))
					if (reg.has<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()))
				reg.get<Enemy>((entt::entity)Results.m_collisionObject->getUserIndex3()).m_hp -= 2;
			}

			return true;
		}
		else
		{
			//InstantiatingSystem::LoadPrefabFromFile(BtToGlm::BTTOGLMV3(to), "node/Water_Proj.node");
			//ECS::Get<Transform>(2).SetPosition(BtToGlm::BTTOGLMV3(to));
			return false;
		}
	}
	else
	{
		//	std::cout << "Not Shot\n";
		return false;

	}

	return false;

}


void Player::Update()
{


	btVector3 playerPosition = RenderingManager::activeScene->FindFirst("Camera").get<PhysicsBody>().GetBody()->getCenterOfMassTransform().getOrigin();
	btVector3 to = playerPosition;
	to.setZ(to.getZ() - 2.f);
	//to += playerPosition;
	btCollisionWorld::ClosestRayResultCallback Results(playerPosition, to);



	PhysicsSystem::GetWorld()->rayTest(playerPosition, to, Results);

	if (Results.hasHit())
	{
		 m_CanJump = true;
	}
	else
	{
		m_CanJump = false;
	}

	m_LeftHandWeapons[m_LeftEquiped]->Update();
	m_RightHandWeapons[m_RightEquiped]->Update();

	for (int i = 0; i < m_Lasers.size(); i++)
	{
		m_Lasers[i].Update();
		if (m_Lasers[i].m_Timer > 0.2f)
		{
			m_Lasers[i].Delete();
			m_Lasers.erase(m_Lasers.begin() + i);
		}
	}

}

void Player::SwitchLeftHand()
{
	m_LeftEquiped = !m_LeftEquiped;
}

void Player::SwitchRightHand()
{
	m_RightEquiped = !m_RightEquiped;
}

bool Player::LeftHandShoot()
{
	return m_LeftHandWeapons[m_LeftEquiped]->Fire();
}

bool Player::RightHandShoot()
{
	return m_RightHandWeapons[m_RightEquiped]->Fire();
}

bool Weapon::Fire()
{
	return false;
}

void Weap_Laser::Update()
{
	//get camera obj
	m_Timer += Timer::dt;
	
}

void Weap_Laser::Delete()
{

	if (RenderingManager::activeScene->Registry().valid(m_Entity))
	RenderingManager::activeScene->Registry().destroy(m_Entity);
	//
}
