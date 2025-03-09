#pragma once

#include "Cho/SDK/DirectX/DirectX12/CommandManager/CommandManager.h"
#include "Cho/SDK/DirectX/DirectX12/CommandContext/CommandContext.h"

class GraphicsCore
{
public:
	// Constructor
	GraphicsCore(ID3D12Device8* device) :
		m_CommandManager(std::make_unique<CommandManager>(device))
	{
	}
	// Destructor
	~GraphicsCore() = default;

	// Getters
	CommandManager* GetCommandManager() const { return m_CommandManager.get(); }
	ID3D12CommandQueue* GetCommandQueue(const QueueType& type)const;
private:
	std::unique_ptr<CommandManager> m_CommandManager = nullptr;
};

