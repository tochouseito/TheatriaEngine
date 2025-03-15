#pragma once
#include "Cho/GameCore/GameObject/GameObject.h"
#include "Cho/Core/Utility/FVector.h"
class ObjectContainer
{
public:
	ObjectContainer()
	{
	}
	~ObjectContainer()
	{
	}
private:
	FVector<GameObject> m_GameObjects;
};

