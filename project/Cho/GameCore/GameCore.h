#pragma once

class ResourceManager;
class GraphicsEngine;

class GameCore
{
public:
	GameCore()
	{
	}
	~GameCore() 
	{
	}
	void Initialize();
	void Start(ResourceManager& resourceManager);
	void Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine);
	void Draw(GraphicsEngine& graphicsEngine);
private:
};

