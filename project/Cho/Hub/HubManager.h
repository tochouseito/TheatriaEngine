#pragma once
class PlatformLayer;
class CoreSystem;
class HubManager
{
public:
	// Constructor
	HubManager(PlatformLayer* platformLayer, CoreSystem* coreSystem) :
		m_pPlatformLayer(platformLayer), m_pCoreSystem(coreSystem)
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
	bool m_IsRun = true;
};

