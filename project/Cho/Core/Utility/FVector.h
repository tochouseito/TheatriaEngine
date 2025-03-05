#pragma once
#include <vector>
#include <cstdint>
#include "Cho/Core/Log/Log.h"
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
			ChoAssertLog("Invalid index out_of_range",false,__FILE__,__LINE__);
        }
        freeList.push_back(index);
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

    // 現在の有効な要素数
    size_t size() const {
        return nextIndex - freeList.size();
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

private:
    std::vector<T> data;          // 実際のデータ
    std::vector<size_t> freeList; // 再利用可能なインデックス
    size_t nextIndex = 0;         // 次に追加する要素の位置
};