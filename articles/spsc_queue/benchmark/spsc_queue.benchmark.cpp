#include "queue_single_threaded.hpp"
#include "queue_locking.hpp"

#include <benchmark/benchmark.h>

#include "queue_lock_free.hpp"

#define REPEAT2(x) do{ x; x; } while(0)
#define REPEAT3(x) REPEAT2(x); x;
#define REPEAT4(x) REPEAT2(x); REPEAT2(x)
#define REPEAT8(x) REPEAT4(x); REPEAT4(x)
#define REPEAT9(x) REPEAT8(x); x;
#define REPEAT16(x) REPEAT8(x); REPEAT8(x)

static constexpr int min_repeats = 16;

static void BM_queue_single_threaded_enqueue_only(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1024> queue;
    double counter{};
    for (auto _ : state) {
        for (int i = 0; i < min_repeats; ++i) {
            queue.enqueue(i);
        }
        counter += min_repeats;
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(0, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * min_repeats);
}
BENCHMARK(BM_queue_single_threaded_enqueue_only)->Name("queue_single_threaded | enqueue only ");

static void BM_queue_single_threaded_enqueue_9_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1024> queue;
    bool enqueue = true;
    int buffer{};
    double counter_enqueue{};
    double counter_dequeue{};
    for (auto _ : state) {
        if (enqueue) {
            constexpr int repeats_amount = 9 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter_enqueue += repeats_amount;
            enqueue = false;
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            enqueue = true;
            counter_dequeue += min_repeats;
        }
        ::benchmark::ClobberMemory();
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter_enqueue, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter_dequeue, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * 5 * min_repeats);
}
BENCHMARK(BM_queue_single_threaded_enqueue_9_dequeue_1)->Name("queue_single_threaded | enqueue 9 dequeue 1 ");

static void BM_queue_single_threaded_enqueue_3_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1024> queue;
    bool enqueue = true;
    int buffer{};
    double counter_enqueue{};
    double counter_dequeue{};

    for (auto _ : state) {
        if (enqueue) {
            constexpr int repeats_amount = 3 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter_enqueue += repeats_amount;
            enqueue = false;
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter_dequeue += min_repeats;
            enqueue = true;
        }
        ::benchmark::ClobberMemory();
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter_enqueue, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter_dequeue, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * 2 * min_repeats);
}
BENCHMARK(BM_queue_single_threaded_enqueue_3_dequeue_1)->Name("queue_single_threaded | enqueue 3 dequeue 1 ");

static void BM_queue_single_threaded_enqueue_1_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1024> queue;
    int buffer{};
    double counter{};

    for (auto _ : state) {
        for (int i = 0; i < min_repeats; ++i) {
            queue.enqueue(0xDEAD);
            ::benchmark::DoNotOptimize(queue.try_dequeue(buffer));
            ::benchmark::DoNotOptimize(buffer);
            ::benchmark::ClobberMemory();
        }
        counter += min_repeats;
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * 2 * min_repeats);
}
BENCHMARK(BM_queue_single_threaded_enqueue_1_dequeue_1)->Name("queue_single_threaded | enqueue 1 dequeue 1 ");

static void BM_queue_single_threaded_enqueue_1_dequeue_3(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1024> queue;
    bool enqueue = true;
    int buffer{};
    double counter_enqueue{};
    double counter_dequeue{};

    for (auto _ : state) {
        if (enqueue) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter_enqueue += min_repeats;
            enqueue = false;
        } else {
            constexpr int repeats_amount = 3 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter_dequeue += 3 * min_repeats;
            enqueue = true;
        }
        ::benchmark::ClobberMemory();
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter_enqueue, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter_dequeue, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * 2 * min_repeats);
}
BENCHMARK(BM_queue_single_threaded_enqueue_1_dequeue_3)->Name("queue_single_threaded | enqueue 1 dequeue 3 ");

static void BM_queue_single_threaded_enqueue_1_dequeue_9(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1024> queue;
    bool enqueue = true;
    int buffer{};
    double counter_enqueue{};
    double counter_dequeue{};

    for (auto _ : state) {
        if (enqueue) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter_enqueue += min_repeats;
            enqueue = false;
        } else {
            constexpr int repeats_amount = 9 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter_dequeue += repeats_amount;
            enqueue = true;
        }
        ::benchmark::ClobberMemory();
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter_enqueue, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter_dequeue, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * 5 * min_repeats);
}
BENCHMARK(BM_queue_single_threaded_enqueue_1_dequeue_9)->Name("queue_single_threaded | enqueue 1 dequeue 9 ");

static void BM_queue_single_threaded_dequeue_only(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1024> queue;
    int buffer{};
    double counter{};

    for (auto _ : state) {
        for (int i = 0; i < min_repeats; ++i) {
            queue.try_dequeue(buffer);
            ::benchmark::DoNotOptimize(buffer);
        }
        counter += min_repeats;
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(0, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * min_repeats);
}
BENCHMARK(BM_queue_single_threaded_dequeue_only)->Name("queue_single_threaded | dequeue only ");

static void BM_queue_locking_enqueue_only(benchmark::State& state) {
    spsc_queue::queue_locking_t<int, 1024> queue;
    double counter{};

    for (auto _ : state) {
        for (int i = 0; i < min_repeats; ++i) {
            queue.enqueue(1);
        }
        counter += min_repeats;
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(0, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * min_repeats);
}
BENCHMARK(BM_queue_locking_enqueue_only)->Name("queue_locking | enqueue only ");

static void BM_queue_locking_enqueue_9_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_locking_t<int, 1024> queue;
    int buffer{};
    double counter{};

    const auto is_producer = state.thread_index() == 0;
    for (auto _ : state) {
        if (is_producer) {
            constexpr int repeats_amount = 9 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += repeats_amount;
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += min_repeats;
        }
        ::benchmark::ClobberMemory();
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 9 * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    }
}
BENCHMARK(BM_queue_locking_enqueue_9_dequeue_1)->Name("queue_locking | enqueue 9 dequeue 1 ")->Threads(2);

static void BM_queue_locking_enqueue_3_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_locking_t<int, 1024> queue;
    int buffer{};
    const auto is_producer = state.thread_index() == 0;
    double counter{};

    for (auto _ : state) {
        if (is_producer) {
            constexpr int repeats_amount = 3 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += repeats_amount;
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += min_repeats;
        }
        ::benchmark::ClobberMemory();
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 3 * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    }
}
BENCHMARK(BM_queue_locking_enqueue_3_dequeue_1)->Name("queue_locking | enqueue 3 dequeue 1 ")->Threads(2);

static void BM_queue_locking_enqueue_1_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_locking_t<int, 1024> queue;
    int buffer{};
    double counter{};
    const auto is_producer = state.thread_index() == 0;

    for (auto _ : state) {
        if (is_producer) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
        }
        counter += min_repeats;
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    }
}
BENCHMARK(BM_queue_locking_enqueue_1_dequeue_1)->Name("queue_locking | enqueue 1 dequeue 1 ")->Threads(2);

static void BM_queue_locking_enqueue_1_dequeue_3(benchmark::State& state) {
    spsc_queue::queue_locking_t<int, 1024> queue;
    int buffer{};
    double counter{};
    const auto is_producer = state.thread_index() == 0;

    for (auto _ : state) {
        if (is_producer) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += min_repeats;
        } else {
            constexpr int repeats_amount = 3 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += repeats_amount;
        }
        ::benchmark::ClobberMemory();
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 3 * min_repeats);
    }
}
BENCHMARK(BM_queue_locking_enqueue_1_dequeue_3)->Name("queue_locking | enqueue 1 dequeue 3 ")->Threads(2);

static void BM_queue_locking_enqueue_1_dequeue_9(benchmark::State& state) {
    spsc_queue::queue_locking_t<int, 1024> queue;
    int buffer{};
    double counter{};
    const auto is_producer = state.thread_index() == 0;

    for (auto _ : state) {
        if (is_producer) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += min_repeats;
        } else {
            constexpr int repeats_amount = 9 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += repeats_amount;
        }
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 9 * min_repeats);
    }
}
BENCHMARK(BM_queue_locking_enqueue_1_dequeue_9)->Name("queue_locking | enqueue 1 dequeue 9 ")->Threads(2);

static void BM_queue_locking_dequeue_only(benchmark::State& state) {
    spsc_queue::queue_locking_t<int, 1024> queue;
    int buffer{};
    double counter{};

    for (auto _ : state) {
        for (int i = 0; i < min_repeats; ++i) {
            queue.try_dequeue(buffer);
            ::benchmark::DoNotOptimize(buffer);
        }
        counter += min_repeats;
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(0, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * min_repeats);
}
BENCHMARK(BM_queue_locking_dequeue_only)->Name("queue_locking | dequeue only ");

static void BM_queue_lock_free_enqueue_only(benchmark::State& state) {
    spsc_queue::queue_lock_free_t<int, 1024> queue;
    double counter{};

    for (auto _ : state) {
        for (int i = 0; i < min_repeats; ++i) {
            queue.enqueue(1);
        }
        counter += min_repeats;
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(0, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * min_repeats);
}
BENCHMARK(BM_queue_lock_free_enqueue_only)->Name("queue_lock_free | enqueue only ");

static void BM_queue_lock_free_enqueue_9_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_lock_free_t<int, 1024> queue;
    int buffer{};
    double counter{};

    const auto is_producer = state.thread_index() == 0;
    for (auto _ : state) {
        if (is_producer) {
            constexpr int repeats_amount = 9 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += repeats_amount;
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += min_repeats;
        }
        ::benchmark::ClobberMemory();
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 9 * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    }
}
BENCHMARK(BM_queue_lock_free_enqueue_9_dequeue_1)->Name("queue_lock_free | enqueue 9 dequeue 1 ")->Threads(2);

static void BM_queue_lock_free_enqueue_3_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_lock_free_t<int, 1024> queue;
    int buffer{};
    const auto is_producer = state.thread_index() == 0;
    double counter{};

    for (auto _ : state) {
        if (is_producer) {
            constexpr int repeats_amount = 3 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += repeats_amount;
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += min_repeats;
        }
        ::benchmark::ClobberMemory();
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 3 * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    }
}
BENCHMARK(BM_queue_lock_free_enqueue_3_dequeue_1)->Name("queue_lock_free | enqueue 3 dequeue 1 ")->Threads(2);

static void BM_queue_lock_free_enqueue_1_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_lock_free_t<int, 1024> queue;
    int buffer{};
    double counter{};
    const auto is_producer = state.thread_index() == 0;

    for (auto _ : state) {
        if (is_producer) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
        } else {
            for (int i = 0; i < min_repeats; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
        }
        counter += min_repeats;
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    }
}
BENCHMARK(BM_queue_lock_free_enqueue_1_dequeue_1)->Name("queue_lock_free | enqueue 1 dequeue 1 ")->Threads(2);

static void BM_queue_lock_free_enqueue_1_dequeue_3(benchmark::State& state) {
    spsc_queue::queue_lock_free_t<int, 1024> queue;
    int buffer{};
    double counter{};
    const auto is_producer = state.thread_index() == 0;

    for (auto _ : state) {
        if (is_producer) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += min_repeats;
        } else {
            constexpr int repeats_amount = 3 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += repeats_amount;
        }
        ::benchmark::ClobberMemory();
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 3 * min_repeats);
    }
}
BENCHMARK(BM_queue_lock_free_enqueue_1_dequeue_3)->Name("queue_lock_free | enqueue 1 dequeue 3 ")->Threads(2);

static void BM_queue_lock_free_enqueue_1_dequeue_9(benchmark::State& state) {
    spsc_queue::queue_lock_free_t<int, 1024> queue;
    int buffer{};
    double counter{};
    const auto is_producer = state.thread_index() == 0;

    for (auto _ : state) {
        if (is_producer) {
            for (int i = 0; i < min_repeats; ++i) {
                queue.enqueue(0xDEAD);
            }
            counter += min_repeats;
        } else {
            constexpr int repeats_amount = 9 * min_repeats;
            for (int i = 0; i < repeats_amount; ++i) {
                queue.try_dequeue(buffer);
                ::benchmark::DoNotOptimize(buffer);
            }
            counter += repeats_amount;
        }
    }

    if (is_producer) {
        state.counters["1) enqueue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * min_repeats);
    } else {
        state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
        state.SetItemsProcessed(state.iterations() * 9 * min_repeats);
    }
}
BENCHMARK(BM_queue_lock_free_enqueue_1_dequeue_9)->Name("queue_lock_free | enqueue 1 dequeue 9 ")->Threads(2);

static void BM_queue_lock_free_dequeue_only(benchmark::State& state) {
    spsc_queue::queue_lock_free_t<int, 1024> queue;
    int buffer{};
    double counter{};

    for (auto _ : state) {
        for (int i = 0; i < min_repeats; ++i) {
            queue.try_dequeue(buffer);
            ::benchmark::DoNotOptimize(buffer);
        }
        counter += min_repeats;
    }

    state.counters["1) enqueue"] = ::benchmark::Counter(0, ::benchmark::Counter::kIsRate);
    state.counters["2) try_dequeue"] = ::benchmark::Counter(counter, ::benchmark::Counter::kIsRate);
    state.SetItemsProcessed(state.iterations() * min_repeats);
}
BENCHMARK(BM_queue_lock_free_dequeue_only)->Name("queue_lock_free | dequeue only ");
