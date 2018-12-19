#pragma once

#include <cmath>

class FastMath
{
public:

	static const unsigned char debruijn64Table[64];

	static const unsigned long long isolateMostSignificantBit(unsigned long long n)
	{
		n |= (n >> 1);
		n |= (n >> 2);
		n |= (n >> 4);
		n |= (n >> 8);
		n |= (n >> 16);
		n |= (n >> 32);
		return n & ~(n >> 1);
	}

	static const unsigned long long isolateLeastSignificantBit(const unsigned long long& n)
	{
		return n & (0 - n);
	}

	static const unsigned char singleBitToPosition(const unsigned long long& n)
	{
		return debruijn64Table[(n * 0x03f79d71b4cb0a89) >> 58] + (unsigned char)(n != 0);
	}

	static const unsigned char positionOfMostSignificantBit(unsigned long long n)
	{
		return singleBitToPosition(isolateMostSignificantBit(n));
	}

	static const unsigned char positionOfLeastSignificantBit(const unsigned long long& n)
	{
		return singleBitToPosition(isolateLeastSignificantBit(n));
	}

	static const unsigned long long positionToSingleBit(const unsigned char& n)
	{
		return (unsigned long long)1 << n - 1;
	}

	static const unsigned char bitBoardPopulationCount(unsigned long long n)
	{
		n = n - ((n >> 1) & 0x5555555555555555);
		n = ((n >> 2) & 0x3333333333333333) + (n & 0x3333333333333333);
		n = ((n >> 4) + n) & 0x0F0F0F0F0F0F0F0F;
		n = ((n >> 8) + n) & 0x00FF00FF00FF00FF;
		n = ((n >> 16) + n) & 0x0000FFFF0000FFFF;
		return ((n >> 32) + n) & 0x00000000000000FF;
	}

	static const bool isPrime(const unsigned long long n)
	{
		if (n % 2 == 0 && n > 2)
		{
			return false;
		}
		for (unsigned long long it = 3; it * it < n; it += 2)
		{
			if (n % it == 0)
			{
				return false;
			}
		}
		return true;
	}

	static const unsigned long long nextOrSamePrime(const unsigned long long n)
	{
		unsigned long long it = n;
		if (n < 4)
		{
			return n;
		}
		else if (n % 2 == 0)
		{
			++it;
		}
		while (!isPrime(it))
		{
			it+=2;
		}
		return it;
	}

	template <typename _FunctionType>
	static void foreachBit(unsigned long long bitBoard, const _FunctionType& foreachFunction)
	{
		while (bitBoard)
		{
			const unsigned long long singleBit = isolateLeastSignificantBit(bitBoard);
			bitBoard ^= singleBit;
			foreachFunction(singleBitToPosition(singleBit));
		}
	}

	template <typename _ContainerType>
	static void pushAllPointer(_ContainerType& a, const _ContainerType& b)
	{
		a->insert(a->end(), b->cbegin(), b->cend());
	}

	template <typename _Type>
	static const _Type fmax(const _Type& a, const _Type& b)
	{
		return b > a ? b : a;
	}

	template <typename _Type>
	static const _Type fmin(const _Type& a, const _Type& b)
	{
		return b < a ? b : a;
	}

	static const double sigmoid(const double& x) {
		return x / (1 + std::abs(x));
	}
};