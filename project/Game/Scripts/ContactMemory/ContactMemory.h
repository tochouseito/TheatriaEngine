#pragma once
#include <vector>
class ContactMemory
{
public:
	void AddMemory(uint32_t number);
	bool CheckHistory(uint32_t number);
	bool Empty() { return history.empty(); }
	/*履歴抹消*/
	void Clear();

private:
	/*履歴*/
	std::vector<uint32_t> history;
};

