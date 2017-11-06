#pragma once
#include <iostream>
#include <vector>

//--------------------------------------------------------------------------//
//----------------------------КЛАСС--BigInteger-----------------------------//
//--------------------------------------------------------------------------//

class BigInteger {
private:
	std::vector<int> data;
	int sign;
	// Обращение к элементу
	int operator[](const unsigned int &b);
	int operator[](const unsigned int &b) const;

	void add(const std::vector<int> &b);
	void subtract(const std::vector<int> &b);
	void multiply(const std::vector<int> &b);
	void Improve();

	void ReverseData();
	void MakeDataFromInt(int x);
	void MakeDataFromString(std::string s);

public:
	BigInteger();
	BigInteger(int x);
	
	// ---Присваивание---
	BigInteger& operator =(const int &b);

	// ---Преобразование---
	explicit operator bool() const;

	// ---Потоки ввода-вывода---
	friend std::istream& operator>>(std::istream& os, BigInteger& b);
	friend std::ostream& operator<<(std::ostream& os, const BigInteger& b);

	// ---Арифметика---
	BigInteger& operator +=(const BigInteger &b);
	BigInteger& operator -=(const BigInteger &b);
	BigInteger& operator *=(const BigInteger &b);
	BigInteger& operator /=(const BigInteger &b);
	BigInteger& operator %=(const BigInteger &b);

	BigInteger& operator ++();
	const BigInteger  operator ++(int);
	BigInteger& operator --();
	const BigInteger  operator --(int);

	const BigInteger operator -() const;
	const BigInteger operator +() const;

	// ---Вспомогательные функции---
	const std::vector<int>& GetData() const;
	int GetSign() const;
	unsigned int length() const;

	bool equal(const BigInteger &b) const;
	bool more(const BigInteger &b) const;

	const BigInteger GetAbs() const;


	const std::string toString() const;
	int ToInt() const;
    double ToDouble() const;
};

const BigInteger abs(const BigInteger &a)   
{
	return a.GetAbs();
}

//--------------------------------------------------------------------------//
//-------------------------Логические операции------------------------------//
//--------------------------------------------------------------------------//

bool operator == (const BigInteger &a, const BigInteger &b)
{
	return a.equal(b);
}

bool operator != (const BigInteger &a, const BigInteger &b)
{
	return !(a.equal(b));
}

bool operator > (const BigInteger &a, const BigInteger &b)
{
	return a.more(b);
}

bool operator >= (const BigInteger &a, const BigInteger &b)
{
	return a.more(b) || a.equal(b);
}

bool operator < (const BigInteger &a, const BigInteger &b)
{
	return !(a >= b);
}

bool operator <= (const BigInteger &a, const BigInteger &b)
{
	return !(a > b);
}

//--------------------------------------------------------------------------//
//------------------------Обращение к элементам-----------------------------//
//--------------------------------------------------------------------------//

int BigInteger::operator[] (const unsigned int &b)
{
	if (b >= this->length())
		return 0;
	return this->data[b];
}

int BigInteger::operator[] (const unsigned int &b) const
{
	if (b >= this->length())
		return 0;
	return this->data[b];
}

//--------------------------------------------------------------------------//
//----------------------------КОНСТРУКТОР-----------------------------------//
//--------------------------------------------------------------------------//

BigInteger::BigInteger()
{
	data = std::vector<int>(1, 0);
	sign = 1;
	return;
}

BigInteger::BigInteger(int n)
{
	MakeDataFromInt(n);
	return;
}

//--------------------------------------------------------------------------//
//----------Операторы равенства, преобразования, чтения-записи--------------//
//--------------------------------------------------------------------------//

BigInteger& BigInteger::operator =(const int &b)
{
	this->MakeDataFromInt(b);
	return *this;
}

BigInteger::operator bool() const {

	return (*this != BigInteger(0));
}

std::istream& operator>>(std::istream& os, BigInteger& b)
{
	std::string s;
	os >> s;
	b.MakeDataFromString(s);
	return os;
}

std::ostream& operator<<(std::ostream& os, const BigInteger& b)
{
	os << b.toString();
	return os;
}

//--------------------------------------------------------------------------//
//----------------------Операторы без присвоения----------------------------//
//--------------------------------------------------------------------------//

const BigInteger operator+(const BigInteger &a, const BigInteger &b)
{
	BigInteger res = a;
	res += b;
	return res;
}

const BigInteger operator-(const BigInteger &a, const BigInteger &b)
{
	BigInteger res = a;
	res -= b;
	return res;
}

const BigInteger operator *(const BigInteger &a, const BigInteger &b)
{
	BigInteger res = a;
	res *= b;
	return res;
}

const BigInteger operator /(const BigInteger &a, const BigInteger &b)
{
	BigInteger res = a;
	res /= b;
	return res;
}

const BigInteger operator %(const BigInteger &a, const BigInteger &b)
{
	BigInteger res = a;
	res %= b;
	return res;
}

//--------------------------------------------------------------------------//
//----------------------Операторы с присвоением-----------------------------//
//--------------------------------------------------------------------------//

BigInteger& BigInteger::operator +=(const BigInteger &b)
{
	if (GetSign() == b.GetSign())
	{ // Если this и b одного знака  
		add(b.data);
		return (*this);
	}

	if (abs(*this) >= abs(b))
	{ // Если this по модулю больше b.
		subtract(b.data);
	}
	else
	{ // Если this по модулю меньше b.
		std::vector<int> old = GetData();
		data = b.data;
		sign *= -1;
		subtract(old);
	}
	return (*this);
}

BigInteger& BigInteger::operator -=(const BigInteger &b)
{
	*this += -b;
	return (*this);
}

BigInteger& BigInteger::operator *=(const BigInteger &b)
{
	sign = sign * b.sign;
	multiply(b.data);
	return (*this);
}

BigInteger& BigInteger::operator /=(const BigInteger &b)
{
	if (abs(*this) < abs(b))
		return *this = BigInteger(0);

	BigInteger tmp = 0;
	BigInteger res = 0;

	for (int i = length() - 1; i >= 0; i--) 
	{
		tmp = tmp * 10 + (*this)[i];
		int l = 0, r = 10;
		while (r - l > 1) 
		{
			int mid = (l + r) / 2;
			abs(mid * b) <= abs(tmp) ? l = mid : r = mid;
		}	
		tmp -= abs(l * b);
		res.data.push_back(l);
	}
	res.ReverseData();
	res.sign = this->GetSign() * b.GetSign();
	res.Improve();
	*this = res;
	return (*this);
}

BigInteger& BigInteger::operator %=(const BigInteger &b)
{
	BigInteger division = *this / b;
	*this = *this - (division * b);
	if (this->length() == 1 && (*this)[0] == 0)
		this->sign = 1;

	return (*this);
}

//--------------------------------------------------------------------------//
//----------------Инкременты, декременты и унарные операции-----------------//
//--------------------------------------------------------------------------//

BigInteger& BigInteger::operator ++()
{
	(*this) += BigInteger(1);
	return (*this);
}

const BigInteger BigInteger::operator ++(int)
{
	BigInteger before = (*this);
	(*this) += BigInteger(1);
	return before;
}

BigInteger& BigInteger::operator --()
{
	(*this) -= BigInteger(1);
	return (*this);
}

const BigInteger BigInteger::operator --(int)
{

	// rifuh
	BigInteger before = *this;
	--(*this);
	return before;
}

const BigInteger BigInteger::operator -() const
{

	BigInteger res = *this;
	if (res.length() != 1 || res[0] != 0)
		res.sign *= -1;
	return res;
}

const BigInteger BigInteger::operator +() const
{
	return *this;
}

//--------------------------------------------------------------------------//
//---------------------Вспомогательные функции------------------------------//
//--------------------------------------------------------------------------//

void BigInteger::ReverseData()
{
	int p1 = 0;
	int p2 = data.size() - 1;
	while (p2 > p1) 
	{
		std::swap(data[p1], data[p2]);
		p1++;
		p2--;
	}
	return;
}

void BigInteger::MakeDataFromString(std::string s)
{
	if (s.length() == 0) 
	{
		*this = BigInteger(0);
		return;
	}

	if (s[0] == '-')
	{
		this->sign = -1;
		this->data.resize(s.length() - 1);
		for (unsigned int i = 0; i < s.length() - 1; i++)
			this->data[i] = s[i + 1] - '0';
		this->ReverseData();
		this->Improve();
		return;
	}
	if (s[0] == '+')
	{
		this->sign = 1;
		this->data.resize(s.length() - 1);
		for (unsigned int i = 0; i < s.length() - 1; i++)
			this->data[i] = s[i + 1] - '0';
		this->ReverseData();
		this->Improve();
		return;
	}

	this->sign = 1;
	this->data.resize(s.length());
	for (unsigned int i = 0; i < s.length(); i++)
		this->data[i] = s[i] - '0';
	this->ReverseData();
	this->Improve();
	return;
}

void BigInteger::MakeDataFromInt(int x)
{
	this->data = std::vector<int>(0);
	if (x == 0) 
	{
		*this = BigInteger();
		return;
	}
	this->sign = x / abs(x);
	x = abs(x);
	while (x > 0)
	{
		this->data.push_back(x % 10);
		x /= 10;
	}
	return;
}

const std::vector<int> & BigInteger::GetData() const
{
	return this->data;
}

int BigInteger::GetSign() const
{
	return this->sign;
}

unsigned int BigInteger::length() const
{
	return this->data.size();
}

bool BigInteger::equal(const BigInteger &b) const
{
	return this->GetData() == b.data && this->sign == b.sign;
}

bool BigInteger::more(const BigInteger &b) const
{
	if (GetSign() != b.GetSign())
		return GetSign() > b.GetSign();

	if (GetData().size() != b.GetData().size())
		return GetData().size() > b.GetData().size();
	
	for (int i = b.GetData().size() - 1; i >= 0; i--)
	{
		if (GetData()[i] != b.GetData()[i])
			return GetData()[i] * GetSign()  > b.GetData()[i] * b.GetSign();
	}
	return 0;
}

void BigInteger::add(const std::vector<int> &b)
{
	for (unsigned int i = 0; i < b.size(); i++) 
	{
		if (i == this->data.size())
			this->data.push_back(b[i]);
		else
			this->data[i] += b[i];
	}
	this->Improve();
	return;
}

void BigInteger::subtract(const std::vector<int> &b)
{
	for (unsigned int i = 0; i < b.size(); i++) 
	{
		if (this->data[i] < b[i]) 
		{
			this->data[i + 1]--;
			this->data[i] += 10;
		}

		this->data[i] -= b[i];
	}
	this->Improve();
	return;
}

void BigInteger::multiply(const std::vector<int> &b)
{
	BigInteger res = BigInteger(0);
	res.sign = sign;
	res.data.resize(b.size() + this->length() + 1, 0);
	
	for (unsigned int i = 0; i < b.size(); i++) 
	{
		for (unsigned int j = 0; j < this->length(); j++)
			res.data[i + j] += b[i] * (*this)[j];
	}
	res.Improve();
	*this = res;
	return;
}

void BigInteger::Improve()
{
	for (unsigned int i = 0; i < data.size(); i++) 
	{
		if (data[i] >= 10) 
		{
			if (i + 1 == data.size())
				data.push_back(data[i] / 10);
			else
				data[i + 1] += data[i] / 10;
			data[i] = data[i] % 10;
		}

		while (data[i] < 0) 
		{
			data[i] += 10;
			data[i + 1]--;
		}
	}
	while (data[data.size() - 1] == 0 && data.size() > 1)
		data.pop_back();

	if (this->length() == 1 && (*this)[0] == 0)
		this->sign = 1;
	return;
}

const std::string BigInteger::toString() const 
{
	std::string res = "";
	if (this->GetSign() == -1)
		res += std::string("-");
	
	int len = this->length();
	for (int i = len - 1; i >= 0; i--) 
		res += (*this)[i] + '0';
	return res;
}

int BigInteger::ToInt() const  
{

	int res = 0;
	for (unsigned int i = 0; i < length(); i++)
		res = res * 10 + (*this)[length() - i - 1];
	res *= GetSign();
	return res;
}

double BigInteger::ToDouble() const
{
    double res = 0.0;
    for (unsigned int i = 0; i < length(); i++)
        res = res * 10 + static_cast<double>((*this)[length() - i - 1]);
    res *= GetSign();
    return res;
}

const BigInteger BigInteger::GetAbs() const  {
	BigInteger res = *this;
	res.sign = 1;
	return res;
}

const BigInteger gcd(const BigInteger &a, const BigInteger &b) {
	return a ? gcd(b % a, a) : b;
}

//--------------------------------------------------------------------------//
//----------------------------КЛАСС--Rational-------------------------------//
//--------------------------------------------------------------------------//

class Rational {
private:
	BigInteger numerator, denominator;

public:
	Rational();
	Rational(const BigInteger &num);
	Rational(const int num); 

	Rational& operator *= (const Rational& num);
	Rational& operator += (const Rational& num);
	Rational& operator -= (const Rational& num);
	Rational& operator /= (const Rational& num);

    explicit operator double() const;
	const Rational operator -() const;

	const std::string toString() const;
	const std::string asDecimal(size_t precision) const;
	
	const BigInteger& GetNumerator() const;
	const BigInteger& GetDenomenator() const;

	void Improve();
};

//--------------------------------------------------------------------------//
//-------------------------Операторы сравнения------------------------------//
//--------------------------------------------------------------------------//

bool operator == (const Rational &a, const Rational &b)
{
	return a.GetNumerator() * b.GetDenomenator() == a.GetDenomenator() * b.GetNumerator();
}

bool operator < (const Rational &a, const Rational &b)
{
	return a.GetNumerator() * b.GetDenomenator() < a.GetDenomenator() * b.GetNumerator();
}

bool operator > (const Rational &a, const Rational &b) 
{
	return b < a;
}

bool operator <= (const Rational &a, const Rational &b) 
{
	return !(a > b);
}

bool operator >= (const Rational &a, const Rational &b) 
{
	return !(a < b);
}

bool operator != (const Rational &a, const Rational &b) 
{
	return !(a == b);
}

//--------------------------------------------------------------------------//
//-------------------------------КОНСТРУКТОРЫ-------------------------------//
//--------------------------------------------------------------------------//

Rational::Rational() {
	numerator = 0;
	denominator = 1;
}

Rational::Rational(const BigInteger &a)
{
	numerator = a;
	denominator = 1;
}

Rational::Rational(const int a) {
	numerator = BigInteger(a);
	denominator = 1;
}

//--------------------------------------------------------------------------//
//------------------------Операторы с присвоением---------------------------//
//--------------------------------------------------------------------------//

Rational& Rational::operator *= (const Rational& a) 
{
	numerator *= a.numerator;
	denominator *= a.denominator;
	Improve();
	return *this;
}

Rational& Rational::operator += (const Rational& a) 
{
	numerator = numerator * a.denominator + a.numerator * denominator;
	denominator *= a.denominator;
	Improve();
	return *this;
}

Rational& Rational::operator -= (const Rational& a) 
{
	*this += -a;
	Improve();
	return *this;
}

Rational& Rational::operator /= (const Rational& a) {
	numerator *= a.denominator;
	denominator *= a.numerator;
	Improve();
	return *this;
}

//--------------------------------------------------------------------------//
//-----------------------Операторы без присвоения---------------------------//
//--------------------------------------------------------------------------//

const Rational operator - (const Rational &a, const Rational &b)
{
	Rational res = a;
	res -= b;
	return res;
}

const Rational operator + (const Rational &a, const Rational &b) 
{
	Rational res = a;
	res += b;
	return res;
}

const Rational operator * (const Rational &a, const Rational &b) 
{
	Rational res = a;
	res *= b;
	return res;
}

const Rational operator / (const Rational &a, const Rational &b) 
{
	Rational res = a;
	res /= b;
	return res;
}

//--------------------------------------------------------------------------//
//-----------------------------Унарный минус--------------------------------//
//--------------------------------------------------------------------------//

const Rational Rational::operator -() const
{
	Rational res = *this;
	res.numerator *= -1;
	return res;
}

//--------------------------------------------------------------------------//
//-----------------------Вспомогательные функции----------------------------//
//--------------------------------------------------------------------------//

const std::string Rational::toString() const 
{
	std::string res = "";

	if (numerator * denominator < 0) 
		res += '-';
	res += abs(numerator).toString();

	if (denominator != 1)
		res += '/' + abs(denominator).toString();
	return res;
}

const std::string Rational::asDecimal(size_t precision = 0) const {
	BigInteger res = numerator;

	for (unsigned int i = 0; i < precision; i++)
		res *= 10;

	res /= denominator;
	res = abs(res);
	std::string s = res.toString();

	// Инвертирование строки.
	for (unsigned int i = 0; i < s.length(); i++)
		if (i < s.length() - i - 1)
			std::swap(s[i], s[s.length() - i - 1]);

	while (s.length() <= precision)
		s += '0';

	if (precision > 0)
		s = s.substr(0, precision) + '.' + s.substr(precision, s.length() - precision);

	if (numerator * denominator < 0)
		s += '-';

	// Обратное инвертирование строки в нормальный вид.
	for (unsigned int i = 0; i < s.length(); i++) 
		if (i < s.length() - i - 1)
			std::swap(s[i], s[s.length() - i - 1]);
	return s;
}

const BigInteger& Rational::GetNumerator() const
{
	return numerator;
}

const BigInteger& Rational::GetDenomenator() const
{
	return denominator;
}

void Rational::Improve() {
	BigInteger z = gcd(abs(numerator), abs(denominator));
	numerator /= z;
	denominator /= z;

	if (denominator < 0) {
		denominator *= -1;
		numerator *= -1;
	}
	return;
}

Rational::operator double() const {
    int precision = 20;
    BigInteger temp_numerator = numerator;

    for (int i = 0; i < precision; i++)
        temp_numerator *= 10;

    double res = (temp_numerator / denominator).ToDouble();

    for (int i = 0; i < precision; i++)
        res /= 10.0;

    return res;
}
