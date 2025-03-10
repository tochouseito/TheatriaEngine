#pragma once
class DepthManager
{
public:
	// Constructor
	DepthManager()
	{
	}
	// Destructor
	~DepthManager() = default;
	
	// Getter
	uint32_t GetDepthBufferIndex() const { return m_DepthBufferIndex; }
	// Setter
	void SetDepthBufferIndex(const uint32_t& index) { m_DepthBufferIndex = index; }
private:
	uint32_t m_DepthBufferIndex = 0;
};

