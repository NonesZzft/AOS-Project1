#include <bits/stdc++.h>
using namespace std;

void fun() {
	while(1);
}

int main() {

	vector<thread> threads;
	for(int i = 0; i < 10; i++) threads.push_back(thread(fun));
	for(auto &t : threads) t.join();

	return 0;
	
}