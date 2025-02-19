#pragma once
#include <random>

class Random {
public:
    // 汎用的な乱数生成関数
    template <typename T>
    static T get(T min, T max) {
        if constexpr (std::is_integral_v<T>) { // 整数型の場合
            std::uniform_int_distribution<T> dist(min, max);
            return dist(getEngine());
        }
        else if constexpr (std::is_floating_point_v<T>) { // 浮動小数点型の場合
            std::uniform_real_distribution<T> dist(min, max);
            return dist(getEngine());
        }
        else {
            static_assert(always_false<T>::value, "Unsupported type for Random::get");
        }
    }

    // シードを設定
    static void setSeed(unsigned int seed) {
        getEngine().seed(seed);
    }

private:
    // 乱数エンジンを取得（静的に管理）
    static std::mt19937& getEngine() {
        static std::mt19937 engine(std::random_device{}());
        return engine;
    }

    // 常に false を返すテンプレート型
    template <typename>
    struct always_false : std::false_type {};
};