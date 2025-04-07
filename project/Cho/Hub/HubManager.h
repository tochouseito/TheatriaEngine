#pragma once
class PlatformLayer;
class CoreSystem;
class ResourceManager;
class GraphicsEngine;
class GameCore;
class HubManager
{
public:
	// Constructor
	HubManager(
		PlatformLayer* platformLayer,
		CoreSystem* coreSystem,
		ResourceManager* resourceManager,
		GraphicsEngine* graphicsEngine,
		GameCore* gameCore
	) :
		m_pPlatformLayer(platformLayer),
		m_pCoreSystem(coreSystem),
		m_pResourceManager(resourceManager),
		m_pGraphicsEngine(graphicsEngine),
		m_pGameCore(gameCore)
	{
	}
	// Destructor
	~HubManager()
	{
	}
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// ウィンドウ
	void Window();
	
	bool IsRun() const { return m_IsRun; }

    void RenderFullScreenUI();
	void ShowSidebar();

	void ShowMainContent();

private:
	PlatformLayer* m_pPlatformLayer = nullptr;
	CoreSystem* m_pCoreSystem = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
	GameCore* m_pGameCore = nullptr;
	bool m_IsRun = true;
};

