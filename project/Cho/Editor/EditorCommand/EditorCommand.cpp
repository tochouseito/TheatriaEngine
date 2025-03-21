#include "pch.h"
#include "EditorCommand.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"
#include "Cho/Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Cho/GameCore/GameCore.h"

void AddGameObjectCommand::Execute(EditorCommand* edit)
{
	edit->GetGameCorePtr()->GetSceneManager()->AddGameObject();
}

void AddGameObjectCommand::Undo(EditorCommand* edit)
{
	edit;
}
