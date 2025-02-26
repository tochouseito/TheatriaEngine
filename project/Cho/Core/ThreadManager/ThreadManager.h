#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>

enum class ThreadName : uint32_t {
    Update0,
	Render0,
    Load0,
    kThreadCount,
};

class ThreadManager {
public:
    // コンストラクタ
    ThreadManager();
    // デストラクタ
    ~ThreadManager();

    // タスクをキューに追加
    void EnqueueTask(std::function<void()> task);
    // ループタスクを追加
    void EnqueueLoopTask(std::function<void(std::atomic<bool>&)> loopTask);
    // 全スレッドの停止
    void StopAllThreads();

private:
    // ワーカースレッドの処理ループ
    void WorkerThread();

private:
    std::vector<std::thread> m_Workers;
    std::queue<std::function<void()>> m_Tasks;
    std::vector<std::function<void(std::atomic<bool>&)>> m_LoopTasks;

    std::mutex m_QueueMutex;
    std::condition_variable m_Condition;
    std::atomic<bool> m_Stop{ false };
};
