#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

static int x = 2;

int main() {

    // Define a Lambda Expression
    auto f = [](int x) {
        cout << x << endl;
    };

    // This thread is launched by using
    // lamda expression as callable

    thread th1, th2, th3;

    th1 = thread(f, x);
    // thread th1(f, x);
    x--;
    // x--;
    th2 = thread(f, 2);
    x--;
    // thread th3(f, 3);
    th3 = thread(f, 3);


    // Wait for thread t3 to finish
    th1.join();
    th2.join();
    th3.join();
    


    return 0;
}