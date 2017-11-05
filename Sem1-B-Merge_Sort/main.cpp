#include <cstdio>
#include <iostream>
using namespace std;

void Merge(int *start, int *finish, int *middle, int* buffer) {
	int *p1 = start;
	int *p2 = middle;
	int *p3 = buffer;

	while (p1 < middle || p2 < finish) {
		if (p1 < middle && (p2 >= finish || *p1 <= *p2))
			*p3++ = *p1++;
		if (p2 < finish && (p1 >= middle || *p2 < *p1))
			*p3++ = *p2++;
	}

	p3 = buffer;
  	for (int *i = start; i < finish; i++, p3++)
  		*i = *p3;
  	return;
}

void MergeSort(int *start, int *finish, int* buffer) {
 	if (finish - start <= 1)
 		return;
 	int *middle = start + (finish - start) / 2;
 	
 	MergeSort(start, middle, buffer);
 	MergeSort(middle, finish, buffer);
 	Merge(start, finish, middle, buffer);
 	
 	return;
 }

int main() {
	int n, m;
	scanf("%d %d", &n, &m);
	int *mas = new int[n * m];
	int *buffer = new int[n * m];
	
	for (int i = 0; i < n * m; i++) {
		scanf("%d", &mas[i]);
		buffer[i] = 0;
	}
	
	MergeSort(mas, mas + n * m, buffer);

	for (int i = 0; i < n * m; i++)
		printf("%d ", mas[i]);
	
	delete[] mas;
	delete[] buffer;
	return 0;
}