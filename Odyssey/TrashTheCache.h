#pragma once
#include <vector>
#include <chrono>

inline void IntegersTiming(std::vector<float>& measurements)
{
    std::vector<int> ints{};
    ints.resize(10'000'000);

    std::fill(ints.begin(), ints.end(), 1);


    auto start = std::chrono::high_resolution_clock::now();
    for (int stepsize{ 1 }; stepsize <= 1024; stepsize *= 2)
    {
        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 10'000'000; i += stepsize)
            ints[i] *= 2;
        auto end = std::chrono::high_resolution_clock::now();
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        measurements.emplace_back(float(elapsedTime / 1000.f));
    }
}

inline void GameObjectTiming(bool useAlternative, std::vector<float>& measurements)
{
    struct Transform
    {
        float matrix[16] =
        {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };
    };

    class GameObject3D
    {
    public:
        Transform transform{};
        int ID{};
    };

    class GameObject3DAlt
    {
    public:
        Transform* transform;
        int ID;
    };


    if (!useAlternative)
    {
        std::vector<GameObject3D> objects{};
        objects.resize(10'000'000);


        auto start = std::chrono::high_resolution_clock::now();
        for (int stepsize{ 1 }; stepsize <= 1024; stepsize *= 2)
        {
            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < 10'000'000; i += stepsize)
                objects[i].ID *= 2;
            auto end = std::chrono::high_resolution_clock::now();
            const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            measurements.emplace_back(float(elapsedTime / 1000.f));
        }
    }

    else
    {
        std::vector<GameObject3DAlt> objects{};
        objects.resize(10'000'000);


        auto start = std::chrono::high_resolution_clock::now();
        for (int stepsize{ 1 }; stepsize <= 1024; stepsize *= 2)
        {
            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < 10'000'000; i += stepsize)
                objects[i].ID *= 2;
            auto end = std::chrono::high_resolution_clock::now();
            const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            measurements.emplace_back(float(elapsedTime / 1000.f));
        }
    }
}