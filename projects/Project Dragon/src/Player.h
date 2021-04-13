#pragma once
#include <RenderingManager.h>
#include <Timer.h>
#include <InstantiatingSystem.h>
#include <AudioEngine.h>
//base weapon class other weapons inherit from this
class Weap_Laser
{
public:
	float m_Timer;
	void Update();
	void Delete();
	entt::entity m_Entity;
	int PlaceInVec;
};

class Weapon
{
public:
	virtual void Update() { m_Timer += Timer::dt; if (m_Timer > m_FireDelay) m_CanShoot = true; else m_CanShoot = false; };
	virtual bool Fire();
	bool m_FireDelay;
	float m_Timer;
	float m_Damage;
	bool m_CanShoot;
};

class FireWeapon : public Weapon
{
public:
	bool Fire() override;
};

class WaterWeapon :public Weapon
{
public:
	bool Fire() override;
};

class AirWeapon : public Weapon
{
public:
	bool Fire() override;
};
class EarthWeapon : public Weapon
{
public:
	void Update() override;
	bool Fire() override;
};
class Player
{
public:
	void InitWeapons();
	void Update();
	void SwitchLeftHand();
	void SwitchRightHand();
	bool LeftHandShoot();
	bool RightHandShoot();
	int m_Hp = 5;
	std::vector<Weapon*> m_LeftHandWeapons;
	std::vector<Weapon*> m_RightHandWeapons;
	bool m_LeftEquiped;
	bool m_RightEquiped;
	bool m_CanJump;
	std::vector<Weap_Laser> m_Lasers;

};
