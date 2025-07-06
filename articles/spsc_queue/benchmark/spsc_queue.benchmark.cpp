#include <benchmark/benchmark.h>

#include "queue_single_threaded.hpp"

#define REPEAT2(x) do{ x; x; } while(0)
#define REPEAT3(x) REPEAT2(x); x;
#define REPEAT4(x) REPEAT2(x); REPEAT2(x)
#define REPEAT8(x) REPEAT4(x); REPEAT4(x)
#define REPEAT9(x) REPEAT8(x); x;
#define REPEAT16(x) REPEAT8(x); REPEAT8(x)

static void BM_queue_single_threaded_enqueue_only(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1'000> queue;
    for (auto _ : state) {
        REPEAT16(queue.enqueue(1));
    }
    state.SetItemsProcessed(state.iterations() * 16);
}

static void BM_queue_single_threaded_enqueue_9_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1'000> queue;
    bool enqueue = true;
    int buffer;
    for (auto _ : state) {
        if (enqueue) {
            REPEAT9(queue.enqueue(0xDEAD));
            enqueue = false;
        } else {
            ::benchmark::DoNotOptimize(queue.try_dequeue(buffer));
            enqueue = true;
        }
    }
    state.SetItemsProcessed(state.iterations() * 5);
}

static void BM_queue_single_threaded_enqueue_3_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1'000> queue;
    bool enqueue = true;
    int buffer;
    for (auto _ : state) {
        if (enqueue) {
            REPEAT3(queue.enqueue(0xDEAD));
            enqueue = false;
        } else {
            ::benchmark::DoNotOptimize(queue.try_dequeue(buffer));
            enqueue = true;
        }
    }
    state.SetItemsProcessed(state.iterations() * 2);
}

static void BM_queue_single_threaded_enqueue_1_dequeue_1(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1'000> queue;
    int buffer;
    for (auto _ : state) {
        queue.enqueue(0xDEAD);
        ::benchmark::DoNotOptimize(queue.try_dequeue(buffer));
    }
    state.SetItemsProcessed(state.iterations() * 2);
}

static void BM_queue_single_threaded_enqueue_1_dequeue_3(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1'000> queue;
    bool enqueue = true;
    int buffer;
    for (auto _ : state) {
        if (enqueue) {
            queue.enqueue(0xDEAD);
            enqueue = false;
        } else {
            REPEAT3(::benchmark::DoNotOptimize(queue.try_dequeue(buffer)));
            enqueue = true;
        }
    }
    state.SetItemsProcessed(state.iterations() * 2);
}

static void BM_queue_single_threaded_enqueue_1_dequeue_9(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1'000> queue;
    bool enqueue = true;
    int buffer;
    for (auto _ : state) {
        if (enqueue) {
            queue.enqueue(0xDEAD);
            enqueue = false;
        } else {
            REPEAT9(::benchmark::DoNotOptimize(queue.try_dequeue(buffer)));
            enqueue = true;
        }
    }
    state.SetItemsProcessed(state.iterations() * 5);
}

static void BM_queue_single_threaded_dequeue_only(benchmark::State& state) {
    spsc_queue::queue_single_threaded_t<int, 1'000> queue;
    int buffer;
    for (auto _ : state) {
        REPEAT16(::benchmark::DoNotOptimize(queue.try_dequeue(buffer)));
    }
    state.SetItemsProcessed(state.iterations() * 16);
}

BENCHMARK(BM_queue_single_threaded_enqueue_only)->Name("queue_single_threaded, enqueue only ");
BENCHMARK(BM_queue_single_threaded_enqueue_9_dequeue_1)->Name("queue_single_threaded, enqueue 9 dequeue 1 ");
BENCHMARK(BM_queue_single_threaded_enqueue_3_dequeue_1)->Name("queue_single_threaded, enqueue 3 dequeue 1 ");
BENCHMARK(BM_queue_single_threaded_enqueue_1_dequeue_1)->Name("queue_single_threaded, enqueue 1 dequeue 1 ");
BENCHMARK(BM_queue_single_threaded_enqueue_1_dequeue_3)->Name("queue_single_threaded, enqueue 1 dequeue 3 ");
BENCHMARK(BM_queue_single_threaded_enqueue_1_dequeue_9)->Name("queue_single_threaded, enqueue 1 dequeue 9 ");
BENCHMARK(BM_queue_single_threaded_dequeue_only)->Name("queue_single_threaded, dequeue only ");

