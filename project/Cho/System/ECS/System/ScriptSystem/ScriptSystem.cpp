#include "PrecompiledHeader.h"
#include "ScriptSystem.h"
#include<assert.h>

//void ScriptSystem::Start(EntityManager& entityManager, ComponentManager& componentManager)
//{
//    for (Entity entity : entityManager.GetActiveEntities()) {
//        ScriptComponent* script = componentManager.GetScript(entity, ObjectType::Object);
//        if (script && script->startFunc) {
//            script->startFunc(script->id, script->type, script->ptr);
//        }
//    }
//}
//
//void ScriptSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
//{
//    for (Entity entity : entityManager.GetActiveEntities()) {
//        ScriptComponent* script = componentManager.GetScript(entity, ObjectType::Object);
//        if (script&&script->updateFunc) {
//            script->updateFunc(script->id, script->type, script->ptr);
//        }
//    }
//}
//
//void ScriptSystem::Cleanup(EntityManager& entityManager, ComponentManager& componentManager)
//{
//    for (Entity entity : entityManager.GetActiveEntities()) {
//        ScriptComponent* script = componentManager.GetScript(entity, ObjectType::Object);
//        if (script && script->cleanupFunc) {
//            assert(LoadFuncDLL(script));
//            script->cleanupFunc();
//        }
//    }
//}
//
//void ScriptSystem::LoadScripts(EntityManager& entityManager, ComponentManager& componentManager)
//{
//    for (Entity entity : entityManager.GetActiveEntities()) {
//        ScriptComponent* script = componentManager.GetScript(entity, ObjectType::Object);
//        if (script&&script->isScript) {
//            assert(LoadFuncDLL(script));
//        }
//    }
//}
//
//void ScriptSystem::UnLoadDLL(EntityManager& entityManager, ComponentManager& componentManager)
//{
//    for (Entity entity : entityManager.GetActiveEntities()) {
//        ScriptComponent* script = componentManager.GetScript(entity, ObjectType::Object);
//        if (script && script->isScript) {
//            FreeLibrary(script->dllHandle);
//        }
//    }
//    std::string name = "test.dll";
//    if (GetModuleHandleA(name.c_str()) != nullptr) {
//        // DLL まだメモリに残っている
//        for (Entity entity : entityManager.GetActiveEntities()) {
//            ScriptComponent* script = componentManager.GetScript(entity, ObjectType::Object);
//            if (script && script->isScript) {
//                FreeLibrary(script->dllHandle);
//            }
//        }
//    }
//}
//
//bool ScriptSystem::LoadFuncDLL(ScriptComponent* script)
//{
//    std::string projectName = ProjectName();
//    std::string projectRoot = ProjectRoot();
//    std::string dllPath = projectRoot + "\\" + projectName + "\\" + "bin\\Debug\\" + projectName + ".dll";
//
//    // DLLをロード
//    script->dllHandle = LoadLibraryA(dllPath.c_str());
//    if (!script->dllHandle) {
//        std::cerr << "Failed to load DLL: " << dllPath << "\n";
//        return false;
//    }
//    std::string funcName = "Create" + script->status.name + "Script";
//    // CreateScript関数を取得
//    typedef IScript* (*CreateScriptFunc)();
//    CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(script->dllHandle, funcName.c_str());
//    if (!createScript) {
//        std::cerr << "CreateScript function not found in DLL: " << dllPath << "\n";
//        FreeLibrary(script->dllHandle);
//        return false;
//    }
//
//    // スクリプトインスタンスを生成
//    IScript* scriptInstance = createScript();
//    if (!scriptInstance) {
//        std::cerr << "Failed to create script instance.\n";
//        FreeLibrary(script->dllHandle);
//        return false;
//    }
//
//    // スクリプトのStart関数とUpdate関数をラップ
//    script->startFunc = [scriptInstance](uint32_t id, uint32_t type, ComponentManager* ptr) {
//        std::cout << "Script Start: ID=" << id << ", Type=" << type << "\n";
//        ptr;
//        scriptInstance->SetEntityInfo(id, type, ptr);
//        scriptInstance->Start();
//        };
//    script->updateFunc = [scriptInstance](uint32_t id, uint32_t type, ComponentManager* ptr) {
//        std::cout << "Script Update: ID=" << id << ", Type=" << type << "\n";
//        ptr;
//        //scriptInstance->SetEntityInfo(id, type, ptr);
//        scriptInstance->Update();
//        };
//
//    // インスタンスの解放用のクロージャを設定
//    script->cleanupFunc = [scriptInstance, this]() {
//        delete scriptInstance;
//        };
//
//    return true;
//}
