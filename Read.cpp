#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

vector<string> removeComments(vector<string> &source) {
	vector<string> res;
	string out;
	for(auto &s : source) {
		int n = s.size();
		for(int i = 0; i < n; i++) {
			if(s[i] == '#') break;
			out += s[i];
		}
		int start = 0;
		for(int i = 0; i <= out.size(); i++) {
			if(i == out.size() || out[i] != ' ') {
				start = i;
				break;
			}
		}
		out = out.substr(start);
		if(!out.empty() && isdigit(out[0])) res.push_back(out);
		out = "";
	}
	return res;
}

void read(const char *filename) {
	// notice: id should be from 0 to total_node
	vector<string> source;
	string s;
	freopen(filename, "r", stdin);
	while(getline(cin, s)) source.push_back(s);
	auto input = removeComments(source);
	if(input.empty()) return ;
	total_node = stoi(input[0]);
	char hostbuffer[256];
	gethostname(hostbuffer, sizeof(hostbuffer));
	string localhost(hostbuffer, 4);
	vector<Node> total(total_node);

	for(int i = 1; i <= total_node; i++) {
		// cout << input[i] << endl;
		int id, port;
		string host;
		istringstream in(input[i]);
		in >> id >> host >> port;
		total[id] = Node(id, host, port);
		if(localhost == host) {
			node.ID = id;
			node.hostName = host;
			node.listenPort = port;
		}
	}

	for(int i = total_node + 1; i < input.size(); i++) {
		// cout << input[i] << endl;
		istringstream in(input[i]);
		int id;
		while(in >> id) {
			if(i - total_node - 1 == node.ID) node_neighbors.push_back(total[id]);
		}
	}
}

int main() {
    return 0;
}