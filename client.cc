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

namespace ipc = boost::interprocess;

int main(int argc, char *argv[])
{
	shm::client client("foob4r");
	client.start();

	auto sets = client.find_array2<shared_sets>("shared_sets");
	auto& curr_set = client.find2<current_set>("current_set");

	auto start = std::chrono::steady_clock::now();
	int64_t ops = 0;

	::signal(15, [](int) { run = false; });
	::signal(2, [](int)  { run = false; });

	std::size_t set_index;
	std::vector<set> local_sets;

	while (run)
	{
		// wait for notification

		curr_set.modify_lock([&](current_set& s, ipc::scoped_lock<ipc::interprocess_mutex>& lock)
		{
            if (!s.new_set)
                s.cond_new_set.wait(lock);

			set_index = s.set_index;
            s.new_set = false;
		});

		sets[set_index]->read([&](const shared_sets& data)
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
