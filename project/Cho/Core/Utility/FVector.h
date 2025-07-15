#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

// フリーリスト付き可変長配列
template <typename T>
class FVector
{
    template <typename T>
    class FVectorIterator
    {
    public:
        FVectorIterator(FVector<T>* vector, size_t index)
            : vector(vector), index(index)
        {
            advanceToValid(); // 無効な要素をスキップ
        }

        T& operator*() { return (*vector)[index]; }

        FVectorIterator& operator++()
        {
            ++index;
            advanceToValid();
            return *this;
        }

        bool operator!=(const FVectorIterator& other) const
        {
            return index != other.index || vector != other.vector;
        }

    private:
        FVector<T>* vector;
        size_t index;

        void advanceToValid()
        {
            while (index < vector->nextIndex && !vector->isValid(index))
            {
                ++index;
            }
        }
    };
    template <typename T>
    class FVectorConstIterator
    {
    public:
        FVectorConstIterator(const FVector<T>* vector, size_t index)
            : vector(vector), index(index)
        {
            advanceToValid();
        }

        const T& operator*() const { return (*vector)[index]; }

        FVectorConstIterator& operator++()
        {
            ++index;
            advanceToValid();
            return *this;
        }

        bool operator!=(const FVectorConstIterator& other) const
        {
            return index != other.index || vector != other.vector;
        }

    private:
        const FVector<T>* vector;
        size_t index;

        void advanceToValid()
        {
            while (index < vector->nextIndex && !vector->isValid(index))
            {
                ++index;
            }
        }
    };
    using iterator = FVectorIterator<T>;
    friend class FVectorIterator<T>;
    using iterator = FVectorIterator<T>;
    using const_iterator = FVectorConstIterator<T>;
public:
    FVector() = default;

    // 新しい要素を追加し、インデックスを返す
    size_t push_back(T&& value)
    {
        if (!freeList.empty())
        {
            size_t index = freeList.back();
            freeList.pop_back();
            data[index] = std::move(value);
            return index;
        }
        if (nextIndex >= data.size())
        {
            data.push_back(std::move(value));
        } else
        {
            data[nextIndex] = std::move(value);
        }
        return nextIndex++;
    }

    template <typename... Args>
    size_t emplace_back(Args&&... args)
    {
        if (!freeList.empty())
        {
            size_t index = freeList.back();
            freeList.pop_back();

            // 配置newで再構築
            new (&data[index]) T(std::forward<Args>(args)...);
            return index;
        }

        if (nextIndex >= data.size())
        {
            data.emplace_back(std::forward<Args>(args)...);
        }
        else
        {
            new (&data[nextIndex]) T(std::forward<Args>(args)...);
        }

        return nextIndex++;
    }

    // 指定インデックスの要素を削除（フリーリストに追加）
    void erase(size_t index) {
        if (index >= nextIndex)
        {
            Log::Write(LogLevel::Assert, "Invalid index out_of_range");
            return;
        }

        if (!isValid(index)) return;

        // 明示的にデストラクタ呼び出し
        data[index].~T();

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
        for (size_t i = 0; i < nextIndex; ++i)
        {
            if (isValid(i))
            {
                data[i].~T();
            }
        }
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

    T& back()
    {
        for (size_t i = nextIndex; i-- > 0; )
        {
            if (isValid(i))
            {
                return data[i];
            }
        }
        Log::Write(LogLevel::Assert, "FVector::back() called on empty or invalid container");
        // 万が一に備えて return（未定義動作防止のために例外投げるほうが適切かも）
        static T dummy{};
        return dummy;
    }

    const T& back() const
    {
        for (size_t i = nextIndex; i-- > 0; )
        {
            if (isValid(i))
            {
                return data[i];
            }
        }
        Log::Write(LogLevel::Assert, "FVector::back() const called on empty or invalid container");
        static const T dummy{};
        return dummy;
    }

	// 非const iterator
    iterator begin()
    {
        return iterator(this, 0);
    }

    iterator end()
    {
        return iterator(this, nextIndex);
    }

    const_iterator begin() const
    {
        return const_iterator(this, 0);
    }

    const_iterator end() const
    {
        return const_iterator(this, nextIndex);
    }


private:
    std::vector<T> data;          // 実際のデータ
    std::vector<size_t> freeList; // 再利用可能なインデックス
    size_t nextIndex = 0;         // 次に追加する要素の位置
};