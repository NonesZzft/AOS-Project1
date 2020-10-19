#include <bits/stdc++.h>
using namespace std;

mutex mtx;
int remaining = 0;
condition_variable data_cond;

void produce() {
    lock_guard<mutex> lck(mtx);
    remaining++;
    data_cond.notify_all();
}
void consume() {
    while(1) {
        unique_lock<mutex> lck(mtx);
        data_cond.wait(lck, []{ return remaining > 0; });
        remaining--;
        lck.unlock();
        break;
    }
}

int main() {
    produce();
    produce();
    consume();
    consume();
    consume();
    cout << remaining << endl;
    return 0;
}