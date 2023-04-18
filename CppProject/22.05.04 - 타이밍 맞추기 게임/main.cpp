#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>
using namespace std;

int g_Timing[9] = { 5, 10, 14, 17, 20, 25, 29, 31, 33 };
string g_Score[9] = { " ", " ", " ", " ", " ", " ", " ", " ", " " };
float timing[9] = { 0, };
int life = 0;

int main() {

	for (;;) {
		clock_t t = clock();
		float s = (float)t / 1000;
		printf("%.3f Sec\n", s);

		if (s > g_Timing[life] + 1) {
			g_Score[life] = "FAIL";
			++life;
		}

		if (_kbhit()) {
			if (life > 9)
				return 0;

			auto ch = _getch(); // ¹öÆÛ ºñ¿ì´Â ¿ëµµ

			timing[life] = abs(s - g_Timing[life]);

			if (timing[life] <= 0.25f) {
				g_Score[life] = "S";
			}
			else if (timing[life] <= 0.5f) {
				g_Score[life] = "A";
			}
			else if (timing[life] <= 0.75f) {
				g_Score[life] = "B";
			}
			else if (timing[life] <= 0.99f) {
				g_Score[life] = "C";
			}
			else {
				g_Score[life] = "FAIL";
			}

			++life;
		}

		printf("%d Sec : %s\n", g_Timing[0], g_Score[0].c_str());
		printf("%d Sec : %s\n", g_Timing[1], g_Score[1].c_str());
		printf("%d Sec : %s\n", g_Timing[2], g_Score[2].c_str());
		printf("%d Sec : %s\n", g_Timing[3], g_Score[3].c_str());
		printf("%d Sec : %s\n", g_Timing[4], g_Score[4].c_str());
		printf("%d Sec : %s\n", g_Timing[5], g_Score[5].c_str());
		printf("%d Sec : %s\n", g_Timing[6], g_Score[6].c_str());
		printf("%d Sec : %s\n", g_Timing[7], g_Score[7].c_str());
		printf("%d Sec : %s\n", g_Timing[8], g_Score[8].c_str());

		Sleep(50);
		system("cls");
	}
}