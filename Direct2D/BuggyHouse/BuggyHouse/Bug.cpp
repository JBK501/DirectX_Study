#include "Bug.h"

Bug::Bug(D2DFramework* pFramework) : Actor(pFramework, L"Data/bug.png")
{
	RECT rct;

	// 기본 위치 설정
	// 1. 윈도우 크기를 받아온다.
	GetClientRect(pFramework->GetWindowHandle(), &rct); 
	
	// 2. 임의의 위치를 지정한다.
	mX = static_cast<float>(rct.left + rand() % (rct.right - rct.left));
	mY = static_cast<float>(rct.top + rand() % (rct.bottom - rct.top));

	mRotation = 0.0f;
	mSteps = 0.0f;
	mIsDeleted = false;
}

void Bug::Draw()
{
	// [이미지 회전 없이 이동]
	//if (mSteps++ > 20) // 1. 20걸음 이상이면
	//{
	//	mSteps = 0.0f; // 1.1. 걸음수를 초기화 한다.

	//	// 방향전환 
	//	int dir = static_cast<int>(mDirection); // 1.2 현재 방향을 가져온다.
	//	int count = static_cast<int>(Direction::COUNT); // 1.3. 방향의 개수를 가져온다.

	//	// 1.4. 새로운 방향을 구한다.
	//	dir += (1 - rand() % 3); //45도 단위로 이동한다
	//	dir = (dir + count) % count;

	//	// 1.5 방향을 재설정한다.
	//	mDirection = static_cast<Direction>(dir);
	//}

	/*switch (mDirection)
	{
	case Bug::Direction::UP:
		mY--;
		break;
	case Bug::Direction::UP_RIGHT:
		mY--;
		mX++;
		break;
	case Bug::Direction::RIGHT:
		mX++;
		break;
	case Bug::Direction::DOWN_RIGHT:
		mY++;
		mX++;
		break;
	case Bug::Direction::DOWN:
		mY++;
		break;
	case Bug::Direction::DOWN_LEFT:
		mY++;
		mX--;
		break;
	case Bug::Direction::LEFT:
		mX--;
		break;
	case Bug::Direction::UP_LEFT:
		mY--;
		mX--;
		break;
	default:
		break;
	}*/

	//Actor::Draw();
	
	
	// [이미지 회전포함 이동]
	if (mSteps++ > 20)
	{
		mSteps = 0.0f;
		mRotation += (1 - rand() % 3) * 45;
	}

	// 이동, 회전, 크기는 행렬로 표시가 가능하다.
	// 행렬 * 행렬 = 두 행렬의 성질을 모두 가진 행렬
	// ex> 이동 * 회전 = 이동하고 회전한 행렬
	// ex> 회전 * 이동 = 회전하고 이동한 행렬
	// 벡터도 행렬이다.

	// 원점을 구한다.
	auto size{ mpBitmap->GetPixelSize() };
	D2D1_RECT_F rect
	{
		0, 0,
		static_cast<float>(size.width),
		static_cast<float>(size.height),
	};
	
	// 전진한다.
	//		1. 단위 벡터를 이용하여 현재 진행할 위치를 구한다.
	auto target = UPVECTOR * D2D1::Matrix3x2F::Rotation(mRotation);	
	
	//		2. 위에서 구한 위치를 더한다.
	mX += target.x; 
	mY += target.y;
	

	auto matTranslate = D2D1::Matrix3x2F::Translation(mX, mY); // 이동 행렬을 만든다.
	auto matRotation = D2D1::Matrix3x2F::Rotation(
		mRotation, //각도
		D2D_POINT_2F{size.width * 0.5f, size.height * 0.5f }	// 중심축
	);

	// 원점을 기준으로 그린다.
	auto pRT = mpFramework->GetRenderTarget();
	
	pRT->SetTransform(matRotation * matTranslate);	// 변환된 행렬(위치)을 지정한다. -->회전하고 이동
	pRT->DrawBitmap(mpBitmap, rect, mOpacity);	// 비트맵에 그린다.
}

bool Bug::IsClicked(POINT& pt)
{
	auto size{ mpBitmap->GetPixelSize() };
	D2D1_RECT_F rect
	{
		mX, mY,
		static_cast<float>(mX + size.width),
		static_cast<float>(mY + size.height)
	};

	// 입력받은 좌표가 해당 범위에 있다면
	if (pt.x >= rect.left && pt.x <= rect.right &&
		pt.y >= rect.top && pt.y <= rect.bottom)
	{
		mIsDeleted = true;	// 벌레의 상태를 바꾼다.
		return true;
	}

	return false;
}
