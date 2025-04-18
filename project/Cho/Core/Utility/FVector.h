#pragma once
#include <vector>
#include <cstdint>
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;
// フリーリスト付き可変長配列
template <typename T>
class FVector {
public:
    FVector() = default;

    // 新しい要素を追加し、インデックスを返す
    size_t push_back(T&& value) {
        if (!freeList.empty()) {
            size_t index = freeList.back();
            freeList.pop_back();
            data[index] = std::move(value);
            return index;
        }
        if (nextIndex >= data.size()) {
            data.push_back(std::move(value));
        } else {
            data[nextIndex] = std::move(value);
        }
        return nextIndex++;
    }

    // 指定インデックスの要素を削除（フリーリストに追加）
    void erase(size_t index) {
        if (index >= nextIndex) {
			Log::Write(LogLevel::Assert, "Invalid index out_of_range");
        }
        freeList.push_back(index);
    }

    // インデックスアクセス
    T& operator[](size_t index) {
        if (index >= nextIndex) {
			Log::Write(LogLevel::Assert, "Index out of range");
        }
        return data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= nextIndex) {
			Log::Write(LogLevel::Assert, "Index out of range");
        }
        return data[index];
    }

    // 現在の有効な要素数
    /*size_t size() const {
        return nextIndex - freeList.size();
    }*/

	// 要素が有効かどうか
	bool isValid(size_t index) const
	{
        // フリーリストにあったら無効な要素
		for (const auto& freeIndex : freeList)
		{
			if (index == freeIndex)
			{
				return false;
			}
		}
		// 有効な要素
		return index < nextIndex;
	}

    // 予約容量
    size_t capacity() const {
        return data.capacity();
    }

    // クリア
    void clear() {
        data.clear();
        freeList.clear();
        nextIndex = 0;
    }

    // 予約
    void reserve(size_t newCapacity) {
        data.reserve(newCapacity);
    }

	// vectorの取得
	std::vector<T>& GetVector() { return data; }

private:
    std::vector<T> data;          // 実際のデータ
    std::vector<size_t> freeList; // 再利用可能なインデックス
    size_t nextIndex = 0;         // 次に追加する要素の位置
};