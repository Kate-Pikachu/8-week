#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <iterator>
#include <numeric>
#include <thread>
#include <vector>
#include <mutex>



class Threads_Guard
{
public:

	explicit Threads_Guard(std::vector < std::thread >& threads) :
		m_threads(threads)
	{}

	Threads_Guard(Threads_Guard const&) = delete;

	Threads_Guard& operator=(Threads_Guard const&) = delete;

	~Threads_Guard() noexcept
	{
		try
		{
			for (std::size_t i = 0; i < m_threads.size(); ++i)
			{
				if (m_threads[i].joinable())
				{
					m_threads[i].join();
				}
			}
		}
		catch (...)
		{
			// std::abort();
		}
	}

private:

	std::vector < std::thread > & m_threads;
};



std::mutex _mutex;

void Searcher(std::string main_s, std::string sub_s, std::vector<int>& vec, std::size_t block_start)
{
	_mutex.lock();
	const std::size_t length = main_s.length();

	int k = 0;

	for (std::size_t i = 0; i < length; ++i)
	{
		std::size_t sz = sub_s.find(main_s, i);
		vec.push_back(sz + block_start);

	}

	_mutex.unlock();
};




void parallel_find(std::string main_s, std::string sub_s, std::vector<int> & vec)
{
	const std::size_t length_m = main_s.length();
	const std::size_t length_s = sub_s.length();


	const std::size_t min_per_thread = 25;
	const std::size_t max_threads =
		(length_m + min_per_thread - 1) / min_per_thread;

	const std::size_t hardware_threads =
		std::thread::hardware_concurrency();

	const std::size_t num_threads =
		std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	const std::size_t block_size = length_m / num_threads;

	for (auto i = 1; i < num_threads; ++i)
	{
		std::string sub = sub_s.substr(i * block_size - length_m + 2, 2 * length_m - 2);
		std::size_t it = sub.find(sub_s);
		vec.push_back(i * block_size - length_m + 2 + it);
	}

	std::vector < std::thread > threads(num_threads - 1);

	Threads_Guard guard(threads);

	std::size_t block_start = 0;

	for (std::size_t i = 0; i < (num_threads - 1); ++i)
	{
		std::size_t block_end = block_start;
		block_end += block_size;

		threads[i] = std::thread(Searcher,
			std::ref(vec), sub_s.substr(block_start, block_size), main_s, i*block_size);
		
		block_start = block_end;
	}
}





int main(int argc, char** argv)
{
	std::string sub_s("CAGTCAGACGTCATCACTAGCTAGC");
	std::string main_s("TGC");
	std::size_t length = main_s.length();
	std::vector <std::size_t> vec;

	parallel_find(std::ref(vec), main_s, sub_s);

	for (auto i : vec)
	{
		std::cout << (i) << "\t" << sub_s.substr(i, length) << std::endl;
	}


	system("pause");
	return EXIT_SUCCESS;
}
