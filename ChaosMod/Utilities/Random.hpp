#pragma once
#include <random>

class Random final : public Singleton<Random>
{
        std::mt19937 engine;

    public:
        Random() // NOLINT(cert-msc51-cpp)
        {
            std::random_device dev;
            engine = std::mt19937(dev());
        }

        template <typename T>
        T Uniform(T min, T max)
        {
            return std::uniform_int_distribution<T>(min, max)(engine);
        }
};
