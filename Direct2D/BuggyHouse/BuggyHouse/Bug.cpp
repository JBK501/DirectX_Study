#include "Bug.h"

Bug::Bug(D2DFramework* pFramework) : Actor(pFramework, L"Data/bug.png")
{
	RECT rct;

	// �⺻ ��ġ ����
	// 1. ������ ũ�⸦ �޾ƿ´�.
	GetClientRect(pFramework->GetWindowHandle(), &rct); 
	
	// 2. ������ ��ġ�� �����Ѵ�.
	mX = static_cast<float>(rct.left + rand() % (rct.right - rct.left));
	mY = static_cast<float>(rct.top + rand() % (rct.bottom - rct.top));

	mRotation = 0.0f;
	mSteps = 0.0f;
	mIsDeleted = false;
}

void Bug::Draw()
{
	// [�̹��� ȸ�� ���� �̵�]
	//if (mSteps++ > 20) // 1. 20���� �̻��̸�
	//{
	//	mSteps = 0.0f; // 1.1. �������� �ʱ�ȭ �Ѵ�.

	//	// ������ȯ 
	//	int dir = static_cast<int>(mDirection); // 1.2 ���� ������ �����´�.
	//	int count = static_cast<int>(Direction::COUNT); // 1.3. ������ ������ �����´�.

	//	// 1.4. ���ο� ������ ���Ѵ�.
	//	dir += (1 - rand() % 3); //45�� ������ �̵��Ѵ�
	//	dir = (dir + count) % count;

	//	// 1.5 ������ �缳���Ѵ�.
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
	
	
	// [�̹��� ȸ������ �̵�]
	if (mSteps++ > 20)
	{
		mSteps = 0.0f;
		mRotation += (1 - rand() % 3) * 45;
	}

	// �̵�, ȸ��, ũ��� ��ķ� ǥ�ð� �����ϴ�.
	// ��� * ��� = �� ����� ������ ��� ���� ���
	// ex> �̵� * ȸ�� = �̵��ϰ� ȸ���� ���
	// ex> ȸ�� * �̵� = ȸ���ϰ� �̵��� ���
	// ���͵� ����̴�.

	// ������ ���Ѵ�.
	auto size{ mpBitmap->GetPixelSize() };
	D2D1_RECT_F rect
	{
		0, 0,
		static_cast<float>(size.width),
		static_cast<float>(size.height),
	};
	
	// �����Ѵ�.
	//		1. ���� ���͸� �̿��Ͽ� ���� ������ ��ġ�� ���Ѵ�.
	auto target = UPVECTOR * D2D1::Matrix3x2F::Rotation(mRotation);	
	
	//		2. ������ ���� ��ġ�� ���Ѵ�.
	mX += target.x; 
	mY += target.y;
	

	auto matTranslate = D2D1::Matrix3x2F::Translation(mX, mY); // �̵� ����� �����.
	auto matRotation = D2D1::Matrix3x2F::Rotation(
		mRotation, //����
		D2D_POINT_2F{size.width * 0.5f, size.height * 0.5f }	// �߽���
	);

	// ������ �������� �׸���.
	auto pRT = mpFramework->GetRenderTarget();
	
	pRT->SetTransform(matRotation * matTranslate);	// ��ȯ�� ���(��ġ)�� �����Ѵ�. -->ȸ���ϰ� �̵�
	pRT->DrawBitmap(mpBitmap, rect, mOpacity);	// ��Ʈ�ʿ� �׸���.
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

	// �Է¹��� ��ǥ�� �ش� ������ �ִٸ�
	if (pt.x >= rect.left && pt.x <= rect.right &&
		pt.y >= rect.top && pt.y <= rect.bottom)
	{
		mIsDeleted = true;	// ������ ���¸� �ٲ۴�.
		return true;
	}

	return false;
}
