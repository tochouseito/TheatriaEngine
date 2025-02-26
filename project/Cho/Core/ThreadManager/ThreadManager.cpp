#include "pch.h"
#include "ThreadManager.h"

// コンストラクタ: 指定した数のスレッドを作成
ThreadManager::ThreadManager(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        m_Workers.emplace_back([this] { WorkerThread(); });
    }
}

// デストラクタ: スレッドを停止して解放
ThreadManager::~ThreadManager() {
    StopAllThreads();
}

// タスクをキューに追加

void ThreadManager::EnqueueTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        m_Tasks.push(std::move(task));
    }
    m_Condition.notify_one(); // スレッドにタスクが追加されたことを通知
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
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Condition.wait(lock, [this] { return m_Stop || !m_Tasks.empty(); });

            if (m_Stop && m_Tasks.empty()) return; // 停止フラグが立ったら終了

            task = std::move(m_Tasks.front());
            m_Tasks.pop();
        }
        task(); // タスクを実行
    }
}