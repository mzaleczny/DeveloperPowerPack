#include "benchmark/benchmark.h"
#include <queue>

#define DECLSPEC
#include "../Tilc/Utils/SmallQueue.h"

#define REPEAT2(x) x x
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT(x) REPEAT32(x)

template <typename T>
void BM_queue(benchmark::State& state) {
    T q;
    for (auto _ : state) {
        REPEAT({
            for (int i = 0; i < 14; ++i) q.push(i);
            while (!q.empty()) {
                benchmark::DoNotOptimize(q.front());
                q.pop();
            }
        });
    }
    state.SetItemsProcessed(32*state.iterations());
}

BENCHMARK_TEMPLATE1(BM_queue, std::queue<int>);
BENCHMARK_TEMPLATE1(BM_queue, TSmallQueue<int>);

BENCHMARK_TEMPLATE1(BM_queue, std::queue<char>);
BENCHMARK_TEMPLATE1(BM_queue, TSmallQueue<char>);

BENCHMARK_MAIN();
