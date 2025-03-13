#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>
#include <string>

// タスクの優先度
enum class TaskPriority {
    High,    // 高優先度
    Normal,  // 通常
    Low      // 低優先度
};

// タスク構造体
struct Task {
    std::string name; // タスク名
    std::function<void()> func; // タスクの実体
    TaskPriority priority = TaskPriority::Normal; // タスクの優先度
    std::vector<std::shared_future<void>> dependencies; // 依存関係タスク

    bool operator<(const Task& other) const {
        return priority < other.priority; // 優先度が高いものを先に処理
    }
};

class ThreadManager {
public:
    // Constructor
    ThreadManager();
    // Destructor
    ~ThreadManager();

    // タスクをキューに追加（依存関係あり）
    std::shared_future<void> EnqueueTask(
        const std::string& taskName,
        std::function<void()> task,
        TaskPriority priority = TaskPriority::Normal,
        std::vector<std::shared_future<void>> dependencies = {}
    );

    // バッチ処理を登録（複数のタスクをまとめて処理）
    std::shared_future<void> EnqueueBatchTask(
        const std::string& batchName,
        std::vector<std::function<void()>> tasks,
        TaskPriority priority = TaskPriority::Normal
    );

    // 全スレッドの停止
    void StopAllThreads();

    // 現在のタスク数を取得
    size_t GetTaskCount() {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        return m_TaskQueue.size();
    }

    // 現在のスレッドの数を取得
    size_t GetThreadCount() const {
        return m_WorkerCount;
    }

    // タスクキューをクリア
    void ClearTaskQueue() {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        while (!m_TaskQueue.empty()) {
            m_TaskQueue.pop();
        }
    }

    // 指定したタスクが完了するまで待機する
    void WaitForTask(std::shared_future<void> task) {
        if (task.valid()) {
            task.wait(); // タスク完了までブロック
        }
    }

    // 全てのタスクが完了するまで待機する
    void WaitForAllTasks() {
        while (true) {
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                if (m_TaskQueue.empty()) {
                    return;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // CPU負荷を抑えて待機
        }
    }

    // スレッドの数を変更
    void SetThreadCount(const uint32_t& newCount) {
        StopAllThreads(); // 既存のスレッドを停止
        m_WorkerCount = newCount;

        for (uint32_t i = 0; i < newCount; ++i) {
            m_Workers.emplace_back([this] { WorkerThread(); });
        }
    }   

private:
    // ワーカースレッドのメインループ
    void WorkerThread();
    // 依存関係が解決済みかチェック
    bool CanExecuteTask(const Task& task);
private:
    std::vector<std::thread> m_Workers;// ワーカースレッド
    std::priority_queue<Task> m_TaskQueue; // タスクキュー
    std::mutex m_QueueMutex;// キューの排他制御
    std::condition_variable m_Condition;// キューの待機
    std::atomic<bool> m_Stop{ false }; // スレッド停止フラグ
    uint32_t m_WorkerCount = 0; // ワーカースレッド数
};
