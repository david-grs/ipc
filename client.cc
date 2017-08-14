#include "common.h"
#include "libs/shm_client.h"

#include <chrono>
#include <iostream>
#include <vector>

extern "C"
{
#include <signal.h>
}

// boo
volatile bool run = true;

int main(int argc, char *argv[])
{
	shm::client client("foob4r");
	client.start();

	std::vector<shm::data<shared_data>*> sets = client.find_array<shared_data>("shared_data");
	shm::data<std::size_t>& current_set = client.find<std::size_t>("current_set");

	auto start = std::chrono::steady_clock::now();
	int64_t ops = 0;

	::signal(15, [](int) { run = false; });
	::signal(2, [](int)  { run = false; });

	int last_set;
	std::vector<set> local_sets;

	while (run)
	{
		// wait for notification

		current_set.read([&](int s)
		{
			last_set = s;
		});

		sets[last_set]->read([&](const shared_data& data)
		{
			local_sets.resize(data._sets.size());
			for (std::size_t i = 0; i < data._sets.size(); ++i)
			{
				local_sets[i].points = data._sets[i].points;
			}
		});

		auto now = std::chrono::steady_clock::now();
		if (now - start > std::chrono::seconds(1))
		{
			std::cout << double(ops) / std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() << "ops/ms" << std::endl;
			start = now;
			ops = 0;
		}
	}
	return 0;
};
