#include <cstdio>
using namespace std;

unsigned int strlen(const char *s) {
	unsigned int len = 0;
	while (s[len] != '\0')
		len++;
	return len;
}

unsigned int findRepetitionsCount(const char* s, char c1, char c2) {
	unsigned int answer = 0;
	unsigned int len = strlen(s);
	for (unsigned int i = 0; i < len - 1; i++) {
		if (s[i] == c1 && s[i + 1] == c2)
			answer++;
	}
	return answer;
}

int main() {
	char string[1000004];
	char a, b;
	scanf("%s\n", &string);
	scanf("%c %c", &a, &b);
	printf("%u" ,findRepetitionsCount(string, a, b));
	return 0;	
}