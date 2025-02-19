#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>
class ThreadManager {
public:
    // コンストラクタ
    ThreadManager(size_t numThreads);
    // デストラクタ
    ~ThreadManager();

    // タスクをキューに追加
    void EnqueueTask(std::function<void()> task);
    // 全スレッドの停止
    void StopAllThreads();

private:
    // ワーカースレッドの処理ループ
    void WorkerThread(); // ワーカースレッドのメイン処理

private:

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_{ false };
};

