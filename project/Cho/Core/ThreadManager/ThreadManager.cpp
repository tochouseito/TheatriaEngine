#include "pch.h"
#include "ThreadManager.h"

// コンストラクタ: 指定した数のスレッドを作成
ThreadManager::ThreadManager() {
    for (uint32_t i = 0; i < static_cast<uint32_t>(ThreadName::kThreadCount); ++i) {
        m_Workers.emplace_back([this] { WorkerThread(); });
    }
}

// デストラクタ: スレッドを停止して解放
ThreadManager::~ThreadManager() {
    StopAllThreads();
}

// 単発タスクをキューに追加
void ThreadManager::EnqueueTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_Tasks.push(std::move(task));
    }
    m_Condition.notify_one(); // スレッドにタスクが追加されたことを通知
}

// ループタスクを追加
void ThreadManager::EnqueueLoopTask(std::function<void(std::atomic<bool>&)> loopTask) {
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_LoopTasks.push_back(std::move(loopTask));
    }
    m_Condition.notify_all(); // スレッドに通知
}

// 全スレッドの停止
void ThreadManager::StopAllThreads() {
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_Stop = true;
    }
    m_Condition.notify_all(); // すべてのスレッドを起こす

    for (std::thread& worker : m_Workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

// ワーカースレッドの処理ループ
void ThreadManager::WorkerThread() {
    while (true) {
        std::function<void()> task;
        bool hasTask = false;

        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Condition.wait(lock, [this] { return m_Stop || !m_Tasks.empty() || !m_LoopTasks.empty(); });

            if (m_Stop) return; // 停止フラグが立ったら終了

            if (!m_Tasks.empty()) {
                task = std::move(m_Tasks.front());
                m_Tasks.pop();
                hasTask = true;
            }
        }

        // 単発タスクがある場合は実行
        if (hasTask) {
            task();
        }

        // ループタスクがある場合は実行（並列で処理する）
        for (auto& loopTask : m_LoopTasks) {
            loopTask(m_Stop);
        }
    }
}
