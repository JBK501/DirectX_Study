#pragma once
#include <list>
#include "D2DFramework.h"
#include "Actor.h"
#include "Bug.h"

class BuggyHouse : public D2DFramework
{
	std::unique_ptr<Actor> mspBackground;
	std::list<std::unique_ptr<Bug>> mBugs;

public:
	virtual void Initialize(HINSTANCE hInstance,
		LPCWSTR title = L"ActorExample",
		UINT width = 1024, UINT height = 768);
	virtual void Release() override;
	virtual void Render() override;

	void CheckBugs();	// 벌레 클릭시 삭제
};

