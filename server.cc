#include "common.h"
#include "libs/shm_server.h"

#include <thread>
#include <chrono>
#include <queue>

#include <boost/container/static_vector.hpp>
#include <boost/optional.hpp>

extern "C"
{
#include <signal.h>
}

// boo
volatile bool run = true;

namespace ipc = boost::interprocess;

int main(int argc, char *argv[])
{
	auto server = std::make_unique<shm::server>("foob4r");
	server->start();

	auto usets = server->construct<shared_sets>("shared_sets", 16);
	std::vector<shm::data<shared_sets>*> sets = *usets;

	auto current_set = server->construct<std::size_t>("current_set");

	auto start = std::chrono::steady_clock::now();
	int64_t ops = 0;

	::signal(15, [](int) { run = false; });
	::signal(2, [](int)  { run = false; });

	boost::optional<std::size_t> current;
	boost::container::static_vector<std::size_t, 32> q;

	std::vector<set> local_sets;

    auto update_set = [&](shared_sets& data)
    {
        data._sets.resize(local_sets.size());
        for (std::size_t i = 0; i < local_sets.size(); ++i)
        {
            set& s = data._sets[i];
            s.points = local_sets[i].points;
        }
    };

	while (run)
	{
		if (current)
		{
			q.push_back(*current);
			current = boost::none;
		}

		while (!current)
		{
			int n = q.front();
			q.erase(q.begin()); // pop_front()

            if (sets[n]->try_modify(update_set))
            {
                current = n;
            }
            else
            {
                q.push_back(n);
            }
		}

		current_set->modify([&](std::size_t& s)
		{
			s = *current;
		});

		auto now = std::chrono::steady_clock::now();
		if (now - start > std::chrono::seconds(1))
		{
			std::cout << double(ops) / std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() << "ops/ms" << std::endl;
			start = now;
			ops = 0;
		}
	}

	server.reset();

	return 0;
};
