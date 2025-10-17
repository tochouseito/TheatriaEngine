#include "pch.h"
#include "include/Main/Engine.h"

// === Engine Systemes ===
#include "include/Platform/FileSystem.h"
#include "include/Platform/Input.h"
#include "include/Platform/Network.h"
#include "include/Platform/Thread.h"
#include "include/Platform/Timer.h"
#include "include/Platform/WinApp.h"
#include "include/Core/Allocators.h"
#include "include/Core/CommandSystem.h"
#include "include/Core/EventSystem.h"
#include "include/Core/FileController.h"
#include "include/Core/JobSystem.h"
#include "include/Core/LogAssert.h"
#include "include/Core/MemoryManager.h"
#include "include/Core/UUID.h"
#include "include/Graphics/DescriptorAllocator.h"
#include "include/Graphics/FrameGraph.h"
#include "include/Graphics/GPUTimeline.h"
#include "include/Graphics/RenderDevice.h"
#include "include/Graphics/Renderer.h"
#include "include/Graphics/ResourceManager.h"
#include "include/Graphics/ShaderCompiler.h"
#include "include/Physics/PhysicsWorld.h"
#include "include/Audio/AudioEngine.h"
#include "include/GameCore/SceneManager.h"
#include "include/Assets/IDManager.h"
#include "include/Assets/Loader.h"
#include "include/Scripting/ScriptAPI.h"

// === Editor ===
#include "include/Editor/ImGuiManager.h"

using namespace Theatria;

/// @brief 実装隠蔽クラス
class Engine::Impl
{
public:
    Impl()
    {

    }
    ~Impl() = default;
private:
    std::unique_ptr<Platform::FileSystem>        m_pFileSystem;             ///< ファイルシステム
    std::unique_ptr<Platform::Input>             m_pInput;                  ///< 入力システム
    std::unique_ptr<Platform::Network>           m_pNetwork;                ///< ネットワークシステム
    std::unique_ptr<Platform::Thread>            m_pThread;                 ///< スレッドシステム
    std::unique_ptr<Platform::Timer>             m_pTimer;                  ///< タイマーシステム
    std::unique_ptr<Platform::WinApp>           m_pWinApp;                  ///< Windowsアプリケーション
    std::unique_ptr<Core::Allocators>         m_pAllocators;                ///< アロケータシステム
    std::unique_ptr<Core::CommandSystem>      m_pCommandSystem;             ///< コマンドシステム
    std::unique_ptr<Core::EventSystem>        m_pEventSystem;               ///< イベントシステム
    std::unique_ptr<Core::FileController>    m_pFileController;             ///< ファイルコントローラ
    std::unique_ptr<Core::JobSystem>         m_pJobSystem;                  ///< ジョブシステム
    std::unique_ptr<Core::LogAssert>         m_pLogAssert;                  ///< ログアサートシステム
    std::unique_ptr<Core::MemoryManager>     m_pMemoryManager;              ///< メモリマネージャ
    std::unique_ptr<Core::UUID>              m_pUUID;                       ///< UUID生成システム
    std::unique_ptr<Graphics::DescriptorAllocator> m_pDescriptorAllocator;  ///< ディスクリプタアロケータ
    std::unique_ptr<Graphics::FrameGraph>       m_pFrameGraph;              ///< フレームグラフ
    std::unique_ptr<Graphics::GPUTimeline>      m_pGPUTimeline;             ///< GPUタイムライン
    std::unique_ptr<Graphics::RenderDevice>     m_pRenderDevice;            ///< レンダーデバイス
    std::unique_ptr<Graphics::Renderer>         m_pRenderer;                ///< レンダラー
    std::unique_ptr<Graphics::ResourceManager>  m_pResourceManager;         ///< リソースマネージャ
    std::unique_ptr<Graphics::ShaderCompiler>   m_pShaderCompiler;          ///< シェーダーコンパイラ
    std::unique_ptr<Physics::PhysicsWorld>      m_pPhysicsWorld;            ///< 物理ワールド
    std::unique_ptr<Audio::AudioEngine>        m_pAudioEngine;              ///< オーディオエンジン
    std::unique_ptr<GameCore::SceneManager>     m_pSceneManager;            ///< シーンマネージャ
    std::unique_ptr<Assets::IDManager>         m_pIDManager;                ///< IDマネージャ
    std::unique_ptr<Assets::Loader>            m_pLoader;                   ///< アセットローダー
    std::unique_ptr<Scripting::ScriptAPI>      m_pScriptAPI;                ///< スクリプトAPI

    std::unique_ptr<Editor::ImGuiManager>    m_pImGuiManager;               ///< ImGuiマネージャ
};

/// @brief コンストラクタ
Theatria::Engine::Engine()
    : m_pImpl(std::make_unique<Impl>())
{
    Initialize();
}

/// @brief デストラクタ
Theatria::Engine::~Engine()
{
    Shutdown();
}
