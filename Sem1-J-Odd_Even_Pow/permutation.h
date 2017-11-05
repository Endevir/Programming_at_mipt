#pragma once
#include <iostream>

class Permutation {
private:
	int *data;
	size_t data_size;
	long long Merge(int *start, int *finish, int *middle, int* buffer);
	long long MergeSort(int *start, int *finish, int* buffer);
public:
	Permutation();
	~Permutation();
	Permutation(const unsigned int x);
	Permutation(const unsigned int x, const int *n);
	Permutation(const Permutation &b);

	// ���������
	Permutation& operator =(const Permutation &a);
	Permutation& operator *=(const Permutation &a);
	friend const Permutation operator *(const Permutation &a, const Permutation &b);
	Permutation& operator ++();
	Permutation operator ++(int);
	Permutation& operator --();
	Permutation operator --(int);

	int operator[](const unsigned int &a) const;
	void operator() (int *a) const;

	// ��������� ��������� �������
	const Permutation next() const;
	const Permutation previous() const;
	const Permutation inverse() const;
	bool isOdd() const;
	bool isEven() const;
	Permutation pow(int degree) const;

	// ������� ��������� ������
	int* GetPermutation() const;
	size_t GetSize() const;
	long long GetInversesCount();
	int GetCycleSize(int i) const;

	bool equal(const Permutation &a) const;
	bool more(const Permutation &a) const;
};


long long Permutation::Merge(int *start, int *finish, int *middle, int* buffer) {
	int *p1 = start;
	int *p2 = middle;
	int *p3 = buffer;
	long long ans = 0;

	while (p1 < middle || p2 < finish) {
		if (p1 < middle && (p2 >= finish || *p1 <= *p2))
			*p3++ = *p1++;
		if (p2 < finish && (p1 >= middle || *p2 < *p1)) {
			ans += (long long)(middle - p1);
			*p3++ = *p2++;
		}
	}

	p3 = buffer;
	for (int *i = start; i < finish; i++, p3++)
		*i = *p3;
	return ans;
}

long long Permutation::MergeSort(int *start, int *finish, int* buffer) {
	if (finish - start <= 1)
		return 0;
	int *middle = start + (finish - start) / 2;

	long long ans1 = MergeSort(start, middle, buffer);
	long long ans2 = MergeSort(middle, finish, buffer);
	long long ans3 = Merge(start, finish, middle, buffer);
	return ans1 + ans2 + ans3;
}

long long Permutation::GetInversesCount() {
	int *buf = new int[data_size];
	return MergeSort(data, data + data_size, buf);
	delete[] buf;
}

//--------------------------------------------------------------------------//
//-------------------------���������� ��������------------------------------//
//--------------------------------------------------------------------------//

bool operator == (const Permutation &a, const Permutation &b)
{
	return a.equal(b);
}

bool operator > (const Permutation &a, const Permutation &b)
{
	return a.more(b);
}

bool operator < (const Permutation &a, const Permutation &b)
{
	return !(a > b || a == b);
}

bool operator != (const Permutation &a, const Permutation &b)
{
	return !(a == b);
}

bool operator >= (const Permutation &a, const Permutation &b)
{
	return (a > b) || (a == b);
}

bool operator <= (const Permutation &a, const Permutation &b)
{
	return !(a > b);
}

//--------------------------------------------------------------------------//
//------------------------��������� � ���������-----------------------------//
//--------------------------------------------------------------------------//

int Permutation::operator[](const unsigned int &a) const
{
	return data[a];
}

Permutation::~Permutation()
{
	delete[] data;
}

Permutation::Permutation()
{
	data = nullptr;
	data_size = 0;
	return;
}

Permutation::Permutation(const Permutation &b)
{
	data_size = b.GetSize();
	data = new int[data_size];
	for (size_t i = 0; i < data_size; i++)
		data[i] = b[i];
	return;
}

Permutation::Permutation(const unsigned int x)
{
	data = new int[x];
	data_size = x;
	for (unsigned int i = 0; i < x; i++)
		data[i] = i;
	return;
}

Permutation::Permutation(const unsigned int x, const int *n)
{
	data_size = x;
	data = new int[x];
	for (unsigned int i = 0; i < x; i++)
		data[i] = n[i];
	return;
}

// ���������
Permutation& Permutation::operator =(const Permutation &a)
{
	if (this == &a)
		return *this;
	if (data != nullptr)
		delete[] data;

	data_size = a.GetSize();
	data = new int[data_size];
	for (size_t i = 0; i < data_size; i++)
		data[i] = a[i];
	return *this;
}

Permutation& Permutation::operator *=(const Permutation &a)
{
	*this = *this * a;
	return *this;
}

const Permutation operator *(const Permutation &a, const Permutation &b) {
	Permutation res(a.GetSize());
	for (size_t i = 0; i < res.GetSize(); ++i)
		res.data[i] = a.data[b.data[i]];
	return res;
}

Permutation& Permutation::operator ++()
{
	if (data_size < 2)
		return *this;

	int first = data_size - 2;
	while (first > -1 && data[first] >= data[first + 1])
		first--;
	if (first == -1)
		return *this; // ������ ������������ ���

	int second = data_size - 1;
	while (second > first && data[first] >= data[second])
		second--;
	std::swap(data[first], data[second]);

	first = first + 1;
	second = data_size - 1; // ��������� ���������� ����� ������������������
	while (first < second)
	{
		std::swap(data[first], data[second]);
		first++;
		second--;
	}

	return *this;
}

Permutation Permutation::operator ++(int)
{
	Permutation res = (*this);
	++(*this);
	return Permutation();
}

Permutation& Permutation::operator --()
{
	if (data_size < 2)
		return *this;

	int first = data_size - 2;
	while (first > -1 && data[first] <= data[first + 1])
		first--;

	if (first == -1)
		return *this; // ������ ������������ ���

	int second = first;
	while (second + 1 < int(data_size) && data[first] >= data[second + 1])
		second++;
	std::swap(data[first], data[second]);

	first = first + 1;
	second = data_size - 1; // ����������� ���������� �����
	while (first < second)
	{
		std::swap(data[first], data[second]);
		first++;
		second--;
	}
	return *this;
}

Permutation Permutation::operator --(int)
{
	Permutation res = *this;
	--(*this);
	return res;
}

// �������-��������, ����������� ������������ � �������
void Permutation::operator() (int *a) const
{
	int *b = new int[GetSize()];
	for (size_t i = 0; i < GetSize(); i++)
		b[i] = a[i];

	for (size_t i = 0; i < GetSize(); i++)
		a[data[i]] = b[i];

	delete[] b;
	return;
}

// ��������� ��������� ������� 
const Permutation Permutation::next() const
{
	Permutation res = *this;
	++res;
	return res;
}

const Permutation Permutation::previous() const
{
	Permutation res = *this;
	--res;
	return res;
}

const Permutation Permutation::inverse() const
{
	Permutation res = *this;
	for (size_t i = 0; i < data_size; ++i) {
		res.data[data[i]] = i;
	}
	return res;
}

bool Permutation::isOdd() const
{
	int* used = new int[GetSize()];
	int even_cycles = 0;
	for (size_t i = 0; i < GetSize(); i++)
		used[i] = 0;

	// ������� ���������� ������ "������"
	for (size_t i = 0; i < GetSize(); i++)
	{
		if (used[i])
			continue;
		int cycle_size = 1;
		int v = data[i];
		while (int(i) != v)
		{
			used[v] = 1;
			v = data[v];
			cycle_size++;
		}
		if (cycle_size % 2 == 0)
			even_cycles++;
	}
	delete[] used;
	return bool(even_cycles % 2);
}

bool Permutation::isEven() const
{
	return !isOdd();
}

Permutation Permutation::pow(int degree) const 
{
	Permutation res = *this;

	int* used = new int[res.GetSize()];
	for (size_t i = 0; i < res.GetSize(); i++)
		used[i] = 0;

	for (size_t i = 0; i < res.GetSize(); i++)
	{
		if (used[i])
			continue;

		int cycle_size = res.GetCycleSize(i); 
		int *cycle = new int[cycle_size]; // ������ ����

		int v = i;
		for (int j = 0; j < cycle_size; j++) {
			cycle[j] = v;
			used[v] = 1;
			v = res.data[v];
		}

		for (int j = 0; j < cycle_size; j++) {
			int cycle_pow = abs(j + degree) / cycle_size + 1;
			res.data[cycle[j]] = cycle[(j + degree + cycle_pow * cycle_size) % cycle_size];
		}
	}
	delete[] used;
	return res;
}

// ������� ��������� ������

int* Permutation::GetPermutation() const
{
	return data;
}

size_t Permutation::GetSize() const
{
	return data_size;
}

bool Permutation::equal(const Permutation &a) const
{
	for (size_t i = 0; i < GetSize(); i++)
		if (data[i] != a[i])
			return 0;
	return 1;
}

bool Permutation::more(const Permutation &a) const
{
	for (size_t i = 0; i < GetSize(); i++)
		if (data[i] != a[i])
			return data[i] > a[i];
	return false;
}

int Permutation::GetCycleSize(int num) const {
	int* used = new int[GetSize()];
	for (size_t i = 0; i < GetSize(); i++)
		used[i] = 0;

	int cycle_size = 1;
	int v = data[num];
	while (num != v)
	{
		used[v] = 1;
		v = data[v];
		cycle_size++;
	}
	delete[] used;
	return cycle_size;
}