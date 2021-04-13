#include <Enemy.h>
#include <RenderingManager.h>

#include <BtToGlm.h>
#include <Player.h>



void Enemy::Update(PhysicsBody m_This)
{
	

#include <iostream>

	if (m_hp <= 0)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(0, 0, -1000));
		m_This.GetBody()->setCenterOfMassTransform(t);
	}


	mTimer += Timer::dt;
	thisPosition = m_This.GetBody()->getCenterOfMassTransform().getOrigin();
	playerPosition = RenderingManager::activeScene->FindFirst("Camera").get<PhysicsBody>().GetBody()->getCenterOfMassTransform().getOrigin();
	distance.setX(powf(thisPosition.getX() - playerPosition.getX(), 2.0f));
	distance.setY(powf(thisPosition.getY() - playerPosition.getY(), 2.0f));
	distance.setZ(powf(thisPosition.getZ() - playerPosition.getZ(), 2.0f));

	//if (distance.length() < 150.f)
	//{
		if (mTimer >= 0.1f)
		{ //Update general enemy information every 2 seconds to reduce computing
			mTimer = 0.0f;
			movementDirection.x = 0;
			movementDirection.z = 0;



			//Chance behavior of enemy based on distance

			//Red Zone
			if (distanceNorm < lookRange * 3) {
				//	std::cout << "\nRetreating\n";
				if (thisPosition.getX() < playerPosition.getX()) {
					movementDirection.x = -1;
				}
				else movementDirection.x = 1;
				if (thisPosition.getZ() < playerPosition.getZ()) {
					movementDirection.z = -1;
				}
				else movementDirection.z = 1;
			}

			//Orange Zone
			else if (distanceNorm < lookRange * 4) {
				//std::cout << "\Attacking\n";
			}

			//Yellow Zone
			if (distanceNorm < lookRange * 8) {
				//std::cout << "\Hunting\n";
				if (thisPosition.getX() < playerPosition.getX()) {
					movementDirection.x = 1;
				}
				else movementDirection.x = -1;
				if (thisPosition.getY() < playerPosition.getY()) {
					movementDirection.y = 1;
				}
				else movementDirection.y = -1;
			}
			//std::cout << "\nX: " << distance.getX() << "\nY: " << distance.getY() << "\nZ: " << distance.getZ() << "\n\n";
			distanceNorm = sqrtf(distance.getX() + distance.getY() + distance.getZ());

			//	std::cout << "\nDistance: " << distanceNorm << "\n\n\n\n";
		}
		//uncomment this to make it move again
		m_This.GetBody()->setLinearVelocity(btVector3(movementDirection.x * m_MovementSpeed, movementDirection.y * m_MovementSpeed, -10));



		//check if player should take damage
		btVector3 Pla_Enemy_Diff;
		Pla_Enemy_Diff = playerPosition - thisPosition;
		glm::vec3 distance = BtToGlm::BTTOGLMV3(Pla_Enemy_Diff);
		float length = glm::length(distance);
		//	std::cout << length << std::endl;
		if (length < 6.7)
		{
			if (canBeHit)
			{

				Player& player = RenderingManager::activeScene->FindFirst("Camera").get<Player>();
				player.m_Hp--;
				canBeHit = false;
				HitTimer = 0.f;
				//player.PlayDamageSound();

			}
		}

		HitTimer += Timer::dt;
		if (HitTimer >= TimeBetweenHits)
			canBeHit = true;
		else
			canBeHit = false;

	//}


}






