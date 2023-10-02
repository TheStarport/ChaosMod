#pragma once
#include <Concepts.hpp>
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

        template <typename T>
            requires std::is_floating_point_v<T>
        T UniformFloat(T min, T max)
        {
            return std::uniform_real_distribution<T>(min, max)(engine);
        }

        template <typename T>
            requires StringRestriction<T>
        T UniformString(uint length)
        {
            std::uniform_int_distribution<short> aToZ('A', 'Z');
            T retStr;
            retStr.reserve(length);
            for (uint i = 0; i < length; i++)
            {
                retStr += static_cast<char>(aToZ(engine));
            }

            return retStr;
        }
};
