#include <iostream>
#include <Windows.h>
#include <conio.h>
using namespace std;

#define MAX_BUF 76

class BaseObject {
public:
	BaseObject() {}
	BaseObject(int x) : _X(x) {}

protected:
	char scBuf[MAX_BUF] = {0,}; // index 75 에는 \0가 들어감
	int _X = 0;

protected:
	bool Pos_Check() {
		if (_X >= 75) {
			return false;
		}
		return true;
	}

	void Clear_scBuf() {
		memset(scBuf, ' ', MAX_BUF);
		scBuf[MAX_BUF - 1] = '\0';
	}

public:
	virtual bool Update() = 0;	// 리턴으로 파괴여부 결정
	virtual void Render() = 0;
	virtual ~BaseObject() {}
};

class OneStar : public BaseObject {
public:
	OneStar() : BaseObject(-1) {}

public:
	bool Update() { 
		_X += 1; 
		return Pos_Check();
	}

	void Render() {
		Clear_scBuf();
		memset(scBuf + _X, 'x', 1); //7071 7273 7475
		scBuf[MAX_BUF - 1] = '\0';
		printf("%s\n", scBuf);
	}
};

class TwoStar : public BaseObject {
public:
	TwoStar() : BaseObject(-2) {}

public:
	bool Update() {
		_X += 2; 
		return Pos_Check();
	}

	void Render() {
		Clear_scBuf();
		memset(scBuf + _X, 'x', 2); //7071 7273 7475
		scBuf[MAX_BUF - 1] = '\0';
		printf("%s\n", scBuf);
	}
};
class ThreeStar : public BaseObject {
public:
	ThreeStar() : BaseObject(-3) {}

public:
	bool Update() { 
		_X += 3;
		return Pos_Check();
	}

	void Render() {
		Clear_scBuf();
		memset(scBuf + _X, 'x', 3); // 727374
		scBuf[MAX_BUF - 1] = '\0';
		printf("%s\n", scBuf);
	}
};

int main() {
	BaseObject* p_base[20] = { 0, };

	while (1) {
		if (_kbhit()) {
			switch (_getch())
			{
			case 0x31:
				for (BaseObject*& p : p_base) {
					if (p == nullptr) {
						p = new OneStar;
						break;
					}
				}
				break;

			case 0x32:
				for (BaseObject*& p : p_base) {
					if (p == nullptr) {
						p = new TwoStar;
						break;
					}
				}
				break;

			case 0x33:
				for (BaseObject*& p : p_base) {
					if (p == nullptr) {
						p = new ThreeStar;
						break;
					}
				}
				break;
			}
		}

		//update : x == 75 일때 삭제하기
		for (BaseObject*& p : p_base) {
			if (p != nullptr) {
				//  x == 75, 삭제하기
				if (!(p->Update())) {
					delete p;
					p = nullptr;
				}
			}
		}

		system("cls");

		//render
		for (BaseObject*& p : p_base) {
			if (p != nullptr) {
				p->Render();
			}
			else {
				printf("\n");
			}
		}

		Sleep(30);
	}
}

/*




여기서 포인트는...

3종의 클래스가 있지만 main 함수쪽 에서는 어떤 클래스인지 알 필요 없이
오직 BaseObject 클래스만의 포인터를 다루며, 가상함수인 Update, Render 만을
호출하면 각 클래스가 알아서 자신이 해야 할 일을 하는 것.



- 각각의 객체들은 X 축 0 좌표부터 시작하여 우측으로 이동을 한다.

- 우측의 끝은 X 축 75 위치로 하며, 끝에 닿는 객체는 파괴된다.

- 출력은 cout,printf 를 사용하여 X 좌표에 맞도록 스페이스를 넣어서 출력한다

- OneStar 는 * / TwoStar 는 ** / ThreeStar 는 *** 를 출력한다

- OneStar 는 1칸 / TwoStar 는 2칸 / ThreeStar 는 3칸을 이동한다

- Y 축은 없으며, 각 배열 위치에 따라서 출력한다 (배열 마다 줄바꿈)



1. BaseObject 포인터 배열 20개.
2. 무한 루프로 돌아감
3. 입력 멈춤이 없도록 숫자 키보드 입력을 체크 (_kbhit(), _getch() 사용)
   + 1번:OneStar / 2번:TwoStar / 3번:ThreeStar 를 생성

4. 번호에 맞는 객체를 동적 생성하여 빈 배열에 등록
5. 메인 루프에서는 항시 배열의 모든 객체에 대하여
   Update(), Render() 를 호출해줌.

6. 각 클래스는 각자의 일을 한다.
   OneStar - 1칸씩 우측 이동 / *  출력
   TwoStar - 2칸씩 우측 이동 / **  출력
   ThreeStar - 3칸씩 우측 이동 / *** 출력


7. 우측 화면에 닿으면 해당 객체는 파괴 되어야 함
8. 메인 루프 하단에는 Sleep(10~50) 을 하여 속도를 늦춘다

main()
{
	while ( 1 )
	{
		KeyProcess();

		Update();

		system("cls");
		Render();

		Sleep(30);
	}
}

Update()
{
	BaseObject 객체 배열 전체를 돌면서
	for ( 0 ~ Max )
	{
		Array[N]->Update();
	}

	// 우측에 닿으면 삭제를 해야하는데 이건 알아서 시도 해봄.
}

render()
{
	BaseObject 객체 배열 전체를 돌면서
	Array[N]->Render();

	// 각 배열에 객체가 있던 없던 줄 바꿈을 해주자.
	// 새로운 객체를 생성시 빈 배열을 찾아서 넣고 있는데
	// 비어있는 배열에 대해서 줄 바꿈을 안해주면
	// 자꾸 줄이 바뀌어서 보기가 안좋음 !
}
*/