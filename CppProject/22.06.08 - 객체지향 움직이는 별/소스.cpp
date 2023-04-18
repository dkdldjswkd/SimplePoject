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
	char scBuf[MAX_BUF] = {0,}; // index 75 ���� \0�� ��
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
	virtual bool Update() = 0;	// �������� �ı����� ����
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

		//update : x == 75 �϶� �����ϱ�
		for (BaseObject*& p : p_base) {
			if (p != nullptr) {
				//  x == 75, �����ϱ�
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




���⼭ ����Ʈ��...

3���� Ŭ������ ������ main �Լ��� ������ � Ŭ�������� �� �ʿ� ����
���� BaseObject Ŭ�������� �����͸� �ٷ��, �����Լ��� Update, Render ����
ȣ���ϸ� �� Ŭ������ �˾Ƽ� �ڽ��� �ؾ� �� ���� �ϴ� ��.



- ������ ��ü���� X �� 0 ��ǥ���� �����Ͽ� �������� �̵��� �Ѵ�.

- ������ ���� X �� 75 ��ġ�� �ϸ�, ���� ��� ��ü�� �ı��ȴ�.

- ����� cout,printf �� ����Ͽ� X ��ǥ�� �µ��� �����̽��� �־ ����Ѵ�

- OneStar �� * / TwoStar �� ** / ThreeStar �� *** �� ����Ѵ�

- OneStar �� 1ĭ / TwoStar �� 2ĭ / ThreeStar �� 3ĭ�� �̵��Ѵ�

- Y ���� ������, �� �迭 ��ġ�� ���� ����Ѵ� (�迭 ���� �ٹٲ�)



1. BaseObject ������ �迭 20��.
2. ���� ������ ���ư�
3. �Է� ������ ������ ���� Ű���� �Է��� üũ (_kbhit(), _getch() ���)
   + 1��:OneStar / 2��:TwoStar / 3��:ThreeStar �� ����

4. ��ȣ�� �´� ��ü�� ���� �����Ͽ� �� �迭�� ���
5. ���� ���������� �׽� �迭�� ��� ��ü�� ���Ͽ�
   Update(), Render() �� ȣ������.

6. �� Ŭ������ ������ ���� �Ѵ�.
   OneStar - 1ĭ�� ���� �̵� / *  ���
   TwoStar - 2ĭ�� ���� �̵� / **  ���
   ThreeStar - 3ĭ�� ���� �̵� / *** ���


7. ���� ȭ�鿡 ������ �ش� ��ü�� �ı� �Ǿ�� ��
8. ���� ���� �ϴܿ��� Sleep(10~50) �� �Ͽ� �ӵ��� �����

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
	BaseObject ��ü �迭 ��ü�� ���鼭
	for ( 0 ~ Max )
	{
		Array[N]->Update();
	}

	// ������ ������ ������ �ؾ��ϴµ� �̰� �˾Ƽ� �õ� �غ�.
}

render()
{
	BaseObject ��ü �迭 ��ü�� ���鼭
	Array[N]->Render();

	// �� �迭�� ��ü�� �ִ� ���� �� �ٲ��� ������.
	// ���ο� ��ü�� ������ �� �迭�� ã�Ƽ� �ְ� �ִµ�
	// ����ִ� �迭�� ���ؼ� �� �ٲ��� �����ָ�
	// �ڲ� ���� �ٲ� ���Ⱑ ������ !
}
*/