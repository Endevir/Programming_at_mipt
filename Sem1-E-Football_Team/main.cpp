#include <cstdio>
typedef long long ll;

class Player 
{
private:
	int _efficiency;
	int _pos;
public:
	Player() {}
	Player(int efficiency, int pos)
		: _efficiency(efficiency)
		, _pos(pos)
	{}
	
	int GetVal() {
		return this -> _efficiency;
	}

	int GetPos() {
		return this -> _pos;
	}
	
	bool operator < (const Player &b) {
		return this -> _efficiency < b._efficiency;
	}

	bool operator <= (const Player &b) {
		return this -> _efficiency <= b._efficiency;
	}

	bool operator >= (const Player &b) {
		return this -> _efficiency >= b._efficiency;
	}
};

void Merge(Player *start, Player *finish, Player *middle, Player* buffer) {
	Player *p1 = start;
	Player *p2 = middle;
	Player *p3 = buffer;

	while (p1 < middle || p2 < finish) {
		if (p1 < middle && (p2 >= finish || *p1 <= *p2))
			*p3++ = *p1++;
		if (p2 < finish && (p1 >= middle || *p2 < *p1))
			*p3++ = *p2++;
	}

	p3 = buffer;
  	for (Player *i = start; i < finish; i++, p3++)
  		*i = *p3;
  	return;
}

void MergeSort(Player *start, Player *finish, Player* buffer) {
 	if (finish - start <= 1)
 		return;
 	Player *middle = start + (finish - start) / 2;
 	
 	MergeSort(start, middle, buffer);
 	MergeSort(middle, finish, buffer);
 	Merge(start, finish, middle, buffer);
 	return;
 }

int main() {
	int n;
	scanf("%d", &n);
	Player *mas = new Player[n];
	Player *buf = new Player[n];

	for (int i = 0; i < n; i++) {
		int x;
		scanf("%d", &x);
		mas[i] = Player(x, i);
	}

	MergeSort(mas, mas + n, buf);

	Player *right = mas + n;
	Player *left = mas + n; // полуинтервал - [левая, правая)
	
	ll kol = 0;
	ll maxkol = 0;
	Player *ansl = NULL;
	Player *ansr = NULL;
	
	while (left > mas) {
		if (kol >= maxkol) {
			maxkol = kol; 
			ansl = left;
			ansr = right;
		}

		if (right <= left + 1) {
			left--;
			kol += left->GetVal();
			continue;
		}

		if ((ll)(right - 1)->GetVal() <= (ll)(left - 1)->GetVal() + (ll)left->GetVal()) {
			left--;
			kol += (ll)left->GetVal();
		} else {
			right--;
			kol -= (ll)right->GetVal();
		}
	} 

	if (kol >= maxkol) {
		maxkol = kol; 
		ansl = left;
		ansr = right;
	}

	bool used[n];	
	for (Player *i = mas; i < mas + n; i++) {
		if (i >= ansl && i < ansr)
			used[i->GetPos()] = 1;
		else
			used[i->GetPos()] = 0;
	}

	printf("%lld\n", maxkol);
	for (int i = 0; i < n; i++)
		if (used[i])
			printf("%d ", i + 1);

	delete[] mas;
	delete[] buf;
	return 0;
}