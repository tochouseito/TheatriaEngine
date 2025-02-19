#include "PrecompiledHeader.h"
#include "ThreadManager.h"

// コンストラクタ: 指定した数のスレッドを作成

ThreadManager::ThreadManager(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this] { WorkerThread(); });
    }
}

// デストラクタ: スレッドを停止して解放

ThreadManager::~ThreadManager() {
    //StopAllThreads();
}

// タスクをキューに追加

void ThreadManager::EnqueueTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        tasks_.push(std::move(task));
    }
    condition_.notify_one(); // スレッドにタスクが追加されたことを通知
}

// 全スレッドの停止

void ThreadManager::StopAllThreads() {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stop_ = true;
    }
    condition_.notify_all(); // すべてのスレッドを起こす

    for (std::thread& worker : workers_) {
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
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty()) return; // 停止フラグが立ったら終了

            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task(); // タスクを実行
    }
}
