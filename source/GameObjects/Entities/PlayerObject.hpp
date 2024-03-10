#pragma once
#include <GameObjects/Entities/EntityObjectBase.hpp>

#include <GameObjects/CameraObject.hpp>

class PlayerObject : public EntityObjectBase {
	static EntityBody s_entityBody;

public:
	PlayerObject();

public:
	void onInitialization() override;
	void onProcess() override;
};