#pragma once
#include <memory>

namespace ody
{
class GameObject;

class IPrefab
{
public:
	IPrefab() = default;
    virtual ~IPrefab() = default;
	IPrefab(const IPrefab& other) = delete;
	IPrefab(IPrefab&& other) = delete;
	IPrefab& operator=(const IPrefab& other) = delete;
	IPrefab& operator=(IPrefab&& other) = delete;

    virtual void Configure(GameObject* gameObject) const = 0;
};

}
