#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include "Cho/Core/Log/Log.h"

// フリーリスト付き固定配列
template <typename T, size_t N>
class FArray {
public:
    FArray() : nextIndex(0), activeCount(0) {}

    // 新しい要素を追加し、インデックスを返す
    size_t push_back(T&& value) {
        if (activeCount >= N) {
			ChoAssertLog("FArray is full", false, __FILE__, __LINE__);
        }

        size_t index;
        if (!freeList.empty()) {
            index = freeList.back();
            freeList.pop_back();
        } else {
            index = nextIndex++;
        }

        data[index] = std::move(value);
        activeCount++;
        return index;
    }

    // 指定インデックスの要素を削除（フリーリストに追加）
    void erase(size_t index) {
        if (index >= nextIndex) {
			ChoAssertLog("Invalid index out_of_range", false, __FILE__, __LINE__);
        }
        if (activeCount == 0) {
            return;
        }
        freeList.push_back(index);
        activeCount--;
    }

    // インデックスアクセス
    T& operator[](size_t index) {
        if (index >= nextIndex) {
			ChoAssertLog("Index out of range", false, __FILE__, __LINE__);
        }
        return data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= nextIndex) {
			ChoAssertLog("Index out of range", false, __FILE__, __LINE__);
        }
        return data[index];
    }

    // 現在の要素数
    size_t size() const {
        return activeCount;
    }

    // 最大容量
    constexpr size_t capacity() const {
        return N;
    }

    // クリア
    void clear() {
        freeList.clear();
        nextIndex = 0;
        activeCount = 0;
    }

private:
    std::array<T, N> data;      // 固定長のデータ
    std::vector<size_t> freeList; // 再利用可能なインデックス
    size_t nextIndex;            // 次に使用するインデックス
    size_t activeCount;          // 有効な要素数
};
