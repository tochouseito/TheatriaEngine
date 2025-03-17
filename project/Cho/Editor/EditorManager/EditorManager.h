#pragma once
#include <memory>
#include <imgui.h>

#include "Cho/Editor/MainMenu/MainMenu.h"

class EditorManager
{
public:
	EditorManager()
	{
		m_MainMenu = std::make_unique<MainMenu>(this);
	}
	~EditorManager()
	{

	}
	void Initialize();
	void Update();
private:
	std::unique_ptr<MainMenu> m_MainMenu = nullptr;
};

