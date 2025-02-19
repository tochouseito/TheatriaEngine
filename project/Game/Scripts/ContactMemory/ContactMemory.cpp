#include "PrecompiledHeader.h"
#include "ContactMemory.h"

void ContactMemory::AddMemory(uint32_t number)
{
	/*履歴に追加*/
	history.push_back(number);
}

bool ContactMemory::CheckHistory(uint32_t number)
{
	for (uint32_t i = 0; i < history.size();i++) {
		if (history[i] == number) {
			return true;
		}
	}
	return false;
}

void ContactMemory::Clear()
{
	history.clear();
}
