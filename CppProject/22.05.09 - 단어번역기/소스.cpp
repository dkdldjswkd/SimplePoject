#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

#define MAX_WORDS 100
#define MAX_LANGUAGE 2
#define MAX_WORD_LENGTH 20

#define KOR 0
#define ENG 1

#define DICTIONARY Dictionary::get_inst() 

class Dictionary {
public:
	const char dictionary[MAX_WORDS][MAX_LANGUAGE][MAX_WORD_LENGTH] =
	{
		{{"���"},{"apple"}},
		{{"�ٳ���"},{"banana"}},
		{{"����"},{"i"}},
		{{"�̴�"},{"am"}},
		{{"����"},{"was"}},
		{{"�ϳ���"},{"a"}},
		{{"�ҳ�"},{"boy"}},
		{{"��"},{"car"}},
		{{""},{""}},
		{{""},{""}},
		{{""},{""}},
	};

public:
	static Dictionary& get_inst() {
		static Dictionary* instance = new Dictionary();
		return *instance;
	}

	const char* find(const char const* p) {
		for (int i = 0; i < MAX_WORDS; i++) {
			if (strcmp(dictionary[i][ENG], p) == 0) {
				//����� �ڵ�
				return dictionary[i][KOR];
			}
		}

		return p;
	}
};

int main() {
	char eng_sentence[100];

	cout << "���� �Է� >> ";
	gets_s(eng_sentence);

	char* p = eng_sentence;

	for (int i = 0;; ++i) {
		switch (eng_sentence[i]) {
		case ' ':
			eng_sentence[i] = '\0';
			printf("%s ", DICTIONARY.find(p));
			p = &eng_sentence[i + 1];
			break;

		case '\0':
			printf("%s ", DICTIONARY.find(p));
			goto EXIT;
		}
	}

EXIT:
	return 0;
}