#pragma once
#include <Concepts.hpp>
#include <random>

class Random final : public Component
{
        std::mt19937 engine;
        inline static const std::vector<std::string> nouns = {
            #include "Nouns.txt"

        };

        inline static const std::vector<std::string> adjectives = {
            #include "Adjectives.txt"
        };

    public:
        Random() // NOLINT(cert-msc51-cpp)
        {
            std::random_device dev;
            engine = std::mt19937(dev());
        }

        void GetCurrentSeed(std::ostream& stream) const
        {
            stream << engine;
        }

        void Reseed(std::string_view seed)
        {
            std::seed_seq sseq{seed.begin(), seed.end()};
            engine.seed(sseq);
        }

        void Reseed(std::istream& seed)
        {
            seed >> engine;
        }

        std::string_view RandomNoun()
        {
            return nouns[std::uniform_int_distribution(0u, nouns.size() - 1)(engine)];
        }

        std::string_view RandomAdjective()
        {
            return adjectives[std::uniform_int_distribution(0u, adjectives.size() - 1)(engine)];
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

        template <typename Iterator>
            requires std::random_access_iterator<Iterator>
        uint Weighted(Iterator start, Iterator end)
        {
            return std::discrete_distribution<>(start, end)(engine);
        }
};
