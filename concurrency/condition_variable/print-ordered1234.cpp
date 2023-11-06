#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>

void f1() { putchar('1'); }
void f2() { putchar('2'); }
void f3() { putchar('3'); }
void f4() { putchar('4'); }

std::mutex mtx;
std::condition_variable cv;
short k = 0;

void t1() {
    while (1) {
        std::unique_lock l(mtx);
        cv.wait(l, [] { return k == 0; });
        f1();
        k = 1;
        cv.notify_all();
    }
}

void t2() {
    while (1) {
        std::unique_lock l(mtx);
        cv.wait(l, [] { return k == 1; });
        f2();
        k = 2;
        cv.notify_all();
    }
}

void t3() {
    while (1) {
        std::unique_lock l(mtx);
        cv.wait(l, [] { return k == 2; });
        f3();
        k = 3;
        cv.notify_all();
    }
}
void t4() {
    while (1) {
        std::unique_lock l(mtx);
        cv.wait(l, [] { return k == 3; });
        f4();
        k = 0;
        cv.notify_all();
    }
}

int main(int argc, char *argv[]) {
    std::vector<std::thread> vth;

    for (int i{}; i < 10; ++i) {
        vth.push_back(std::thread(t4));
        vth.push_back(std::thread(t3));
        vth.push_back(std::thread(t2));
        vth.push_back(std::thread(t1));
    }

    for (auto &t : vth) {
        t.join();
    }
    return 0;
}
