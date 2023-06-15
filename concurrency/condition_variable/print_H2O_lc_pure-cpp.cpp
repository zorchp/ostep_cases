#include <condition_variable>
#include <mutex>
#include <pthread.h>
#include <tuple>
#include <vector>
#include <functional>
using namespace std;


static enum state { A = 1, B, C, D, E } tmp;
vector<tuple<int, char, int>> rules = {
    {A, 'H', B}, //
    {B, 'O', C}, //
    {C, 'H', A}, //
    {B, 'H', D}, //
    {D, 'O', A}, //
    {A, 'O', E}, //
    {E, 'H', C}, //
};
class H2O {
    int next(char c) {
        enum Items { from = 0, ch, to };
        for (int i = 0; i < rules.size(); ++i) {
            auto rule = rules[i];
            if (get<from>(rule) == cur && get<ch>(rule) == c) {
                return get<to>(rule);
            }
        }
        return 0;
    }

    int can_print(char ch) { return next(ch) != 0 && quota > 0; }

    void H2O_before(char ch) {
        std::unique_lock<std::mutex> lk(mtx);
        while (!can_print(ch)) //
            cv.wait(lk);
        --quota;
    }

    void H2O_after(char ch) {
        lock_guard<std::mutex> lk(mtx);
        ++quota;
        cur = next(ch);
        cv.notify_all();
    }

    int cur, quota;
    std::mutex mtx;
    std::condition_variable cv;

public:
    H2O() : cur(A), quota(1) {}

    void hydrogen(function<void()> releaseHydrogen) {
        H2O_before('H');
        releaseHydrogen();
        H2O_after('H');
    }

    void oxygen(function<void()> releaseOxygen) {
        H2O_before('O');
        releaseOxygen();
        H2O_after('O');
    }
};
