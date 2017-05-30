#include "stdafx.h"

//#include "gtest/gtest.h"

#include "benchmark/benchmark_api.h"

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

// http://qa.geeksforgeeks.org/3090/how-to-find-nth-prime-number

// The simplest case
bool isPrime(unsigned long num)
{
	if (num == 2)
		return true;

	if (num <= 1 || num % 2 == 0)	// 0, 1, and all even numbers
		return false;

	for (unsigned long x = 3; x*x <= num; x += 2) {
		if (num % x == 0)
			return false;
	}

	return true;
}

unsigned long get_nth_prime_without_sieve(unsigned long n) {
	
	if (n == 0) return 0;
	if (n == 1) return 2;

	unsigned long prime_num = 3;
	unsigned long local_n = 2;	// 2th prime number is 3

	while (local_n != n) {
		
		// check next prime number candidate
		prime_num += 2;	// 3,5,7,9... check only odd numbers

		if (isPrime(prime_num) == true) {
			local_n++;
		}
	}

	return prime_num;
}

/// snippet from
/// http://www.geeksforgeeks.org/sieve-of-eratosthenes/
///
std::vector<bool> SieveOfEratosthenes(unsigned long less)
{
	// Create a boolean array "prime[0..n]" and initialize
	// all entries it as true. A value in prime[i] will
	// finally be false if i is Not a prime, else true.
	vector<bool> prime(less + 1);
	std::fill(begin(prime), end(prime), true);

	for (unsigned long p = 2; p*p <= less; p++)
	{
		// If prime[p] is not changed, then it is a prime
		if (prime[p] == true)
		{
			// Update all multiples of p
			for (unsigned long i = p * 2; i <= less; i += p)
				prime[i] = false;
		}
	}

	return prime;

	// Print all prime numbers
	//for (unsigned long p = 2; p <= less; p++)
	//	if (prime[p] == true)
	//		cout << p << " ";
}

unsigned long get_nth_prime_with_basic_sieve(unsigned long n) {
	unsigned long precompute_max_count = 100000;

	do
	{
		if (precompute_max_count >= std::numeric_limits<unsigned long>::max())
			break;

		auto prime = SieveOfEratosthenes(precompute_max_count);

		// how manay 'true' exist?
		unsigned long primes_length = std::count(std::begin(prime) + 2, std::end(prime), true);

		if (primes_length < n) {
			precompute_max_count *= 2;	// double the limit & try again
			continue;
		}

		// get index nth true locates
		unsigned long nth = 0;

		// 2부터 시작해서(0과 1인 true이지만 prime number가 아니므로), 
		// 더해진 prime number의 합이 n과 같다면, 현재 그 위치를 반환한다.
		auto it = std::find_if(begin(prime) + 2, end(prime), [nth, n](bool e) mutable { 
			if (e) {
				nth++;
				if (nth == n)
					return true;
			} 
			return false;
		});

		// 그 위치의 수를 구한다.
		auto d = std::distance(begin(prime), it);

		return d;

	} while (true);

	return 0;
}

#include <memory>
// note1. 홀수 n이 주어졌을 때, n이 몇 번째 홀수인지 알아내려면 (n - 3) >> 1 한다.
unsigned long get_nth_prime_with_optimized_sieve(unsigned long n) {

	unsigned long precompute_max_count = 200000;

	// prime number를 체크하기 위해 우선 대상이 될 모든 odd number들을 담을 배열을 만든다.
	vector<bool> odd_numbers_divisible(precompute_max_count/2);	// odd numers

	// n개의 prime number들이 담길 array를 만든다.
	auto primes = std::make_unique<unsigned long[]>(n);

	// 3부터 모든 odd numbers에 대하여,
	for (unsigned int i = 3; i <= std::sqrt(precompute_max_count); i += 2)	// 3, 5, 7, 9, 11, 13....9257
	{
		auto i_th = (i - 3) >> 1;	// i_th는 i라는 odd#가 몇 번째 odd number인지 구한다.
		if (odd_numbers_divisible[i_th] == false)	// divide 할 수 없는 prime #찾았다면,
		{
			// prime number를 찾았다면, loop 돌며 이후 i의 위치마다 모두 prime number가 아니라고 체크해 둔다.
			for (unsigned int j = i*i; j < precompute_max_count; j += i)
			{
				// j는 이미 다른 수의 제곱이고 3부터 증가하는 홀수(3,5,6,7...) i 를 더한 값이므로, 
				// j는 prime#가 아니다.	

				if (j & 1)	// odd인 경우에만. even인 경우는 아예 prime #가 아니다.
				{
					// 3번째 odd#는 9, (9-3) >> 1 하면 3
					// 6번째 odd#는 15, (15-3) >> 1하면 6
					// y는 j라는 홀수가 몇 번째 홀수인지를 index를 구한다.
					auto y = (j - 3) >> 1;

					// y번째 odd#는 prime#가 아니다. divide 할 수 있다.
					odd_numbers_divisible[y] = true;
				}
			}
		}
	}

	unsigned long primes_length = std::count(std::begin(odd_numbers_divisible), std::end(odd_numbers_divisible), false) + 1; 
	// odd_numbers_divisible는 3부터 체크하므로 prime number인 2를 하나 더 함

	if (primes_length < n) {
		return 0;
	}

	// 첫번째 prime #는 2다.
	primes[0] = 2;
	unsigned int cnt = 1;
	for (unsigned int i = 0; 2 * i < precompute_max_count && cnt < n; i++)
	{
		if (odd_numbers_divisible[i] == false)
		{
			primes[cnt++] = 2 * i + 3;
			//cout << 2 * i + 3 << " ";
		}
	}
	//cout << endl;

	// [0]에 첫번째 prime이 있으므로 6번째 primn이라면 [6-1] 구해야 한다.
	return primes[n - 1];
}

/*
// google test code
TEST(PrimeCheck, basic_prime_check_tests)
{
	EXPECT_EQ(false, isPrime(1));
	EXPECT_EQ(true, isPrime(2));
	EXPECT_EQ(true, isPrime(3));
	EXPECT_EQ(false, isPrime(4));

	EXPECT_EQ(true, isPrime(17));
	EXPECT_EQ(true, isPrime(37));
	EXPECT_EQ(true, isPrime(41));

	EXPECT_EQ(false, isPrime(100));
}
*/

static void BM_get_nth_prime_without_sieve(benchmark::State& state) {
	while (state.KeepRunning())
		get_nth_prime_without_sieve(state.range_x());
}

static void BM_get_nth_prime_with_basic_sieve(benchmark::State& state) {
	while (state.KeepRunning())
		get_nth_prime_with_basic_sieve(state.range_x());
}


static void BM_get_nth_prime_with_optimized_sieve(benchmark::State& state) {
	while (state.KeepRunning())
		get_nth_prime_with_optimized_sieve(state.range_x());
}


BENCHMARK(BM_get_nth_prime_without_sieve)->Arg(10001);
BENCHMARK(BM_get_nth_prime_with_basic_sieve)->Arg(10001);
BENCHMARK(BM_get_nth_prime_with_optimized_sieve)->Arg(10001);

#pragma comment(lib, "benchmark.lib")
#pragma comment(lib, "shlwapi.lib")

#if _DEBUG
//#pragma comment(lib, "gtest\\Debug\\gtestd.lib")
#else
//#pragma comment(lib, "gtest\\Release\\gtest.lib")
#endif

//GTEST_API_ int main(int argc, char **argv) {
//	printf("Running main() from gtest_main.cc\n");
//	testing::InitGoogleTest(&argc, argv);
//	return RUN_ALL_TESTS();
//}

BENCHMARK_MAIN();
