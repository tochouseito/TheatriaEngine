#pragma once

#include<cstdint>
#include<string>
#include<Windows.h>

class SystemState {
public:
    static SystemState& GetInstance() {
        static SystemState instance;
        return instance;
    }
private:
    SystemState(){}
    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

public:

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

   float GetDeltaTime() const { return deltaTime; }

   float GetFrameRate() const { return MaxFramePerSecond; }

   int32_t GetWindowWidth()const { return windowWidth; }

   void SetWindowWidth(int32_t value) { windowWidth = value; }

   int32_t GetWindowHeight()const { return windowHeight; }

   void SetWindowHeight(int32_t value) { windowHeight = value; }

   void SetNowFPS(float value) { nowFPS = value; }

   float GetNowFrameRate()const { return nowFPS; }

   void SetDeltaTime(float value) { deltaTime = value; }

   bool WindowResize();

   void SetProjectName(const std::string& name) { projectName = name; }

   std::string GetProjectName() {
       return projectName;
   }

   void SetProjectRoot(const std::string& name) { projectRoot = name; }

   std::string GetProjectRoot() { 
       return projectRoot;
   }

   // 警告ウィンドウを表示
   static void ShowWarning(const char* message) {
       MessageBoxA(
           nullptr,
           message,
           "Warning",
           MB_OK | MB_ICONWARNING
       );
   }

   bool IsEngineTest() { return isEngineTest; }

private:
    int32_t windowWidth = 1280;// ウィンドウ横幅
    int32_t windowHeight = 720;// ウィンドウ縦幅

    int32_t preWindowWidth = windowWidth;// ウィンドウ横幅
    int32_t preWindowHeight = windowHeight;// ウィンドウ縦幅

    float MaxFramePerSecond = 60.0f;// FPS

    float nowFPS = MaxFramePerSecond;

    float deltaTime = 1.0f / nowFPS;// デルタタイム

	bool isEngineTest = false;

    std::string projectName = "";
    std::string projectRoot = "";
};

inline float DeltaTime() {
    return SystemState::GetInstance().GetDeltaTime();
}

inline float MaxFrameRate() {
    return SystemState::GetInstance().GetFrameRate();
}

inline int32_t WindowWidth() {
    return SystemState::GetInstance().GetWindowWidth();
}

inline int32_t WindowHeight() {
    return SystemState::GetInstance().GetWindowHeight();
}

inline float NowFrameRate() {
    return SystemState::GetInstance().GetNowFrameRate();
}

inline std::string ProjectName() {
    return SystemState::GetInstance().GetProjectName();
}

inline std::string ProjectRoot() {
    return SystemState::GetInstance().GetProjectRoot();
}