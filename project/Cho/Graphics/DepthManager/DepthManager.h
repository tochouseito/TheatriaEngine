#pragma once
class DepthManager
{
	friend class GraphicsEngine;
public:
	// Constructor
	DepthManager()
	{
	}
	// Destructor
	~DepthManager() = default;
	
	// Getter
	uint32_t GetDepthBufferIndex() const { return m_DepthBufferIndex; }
	uint32_t GetDebugDepthBufferIndex() const { return m_DebugDepthBufferIndex; }
	// Setter
	void SetDepthBufferIndex(const uint32_t& index) { m_DepthBufferIndex = index; }
	void SetDebugDepthBufferIndex(const uint32_t& index) { m_DebugDepthBufferIndex = index; }
private:
	uint32_t m_DepthBufferIndex = 0;
	uint32_t m_DebugDepthBufferIndex = 1;
};

