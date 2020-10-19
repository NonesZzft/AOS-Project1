#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
using namespace std;

struct Node {
	int ID;
	string hostName;
	int listenPort;
	Node() {}
	Node(int id, string hn, int lp) : ID(id), hostName(hn), listenPort(lp) {}
};

struct Eccentricity {
	int ID;
	int round;
	Eccentricity() {}
	Eccentricity(int id, int r) : ID(id), round(r) {}
};

vector<string> removeComments(vector<string> &source);

void read(const char *filename);

void client(Node neighbor);

void server();

void communicate(int sock);



mutex mtx;
condition_variable cv;
int Round = 0;
Node node;
int total_node;
vector<Node> node_neighbors;
vector<Eccentricity> found_node;

int main() {
	// read file
	read("config.txt");

	// output information
	cout << "Host name: " << node.hostName << endl;
	cout << "Listen port: " << node.listenPort << endl;
	cout << "NodeID: " << node.ID << " with total node " << total_node << endl;
	cout << "Node neighbors are: " << endl;
	for(auto nn : node_neighbors) cout << nn.ID << " " << nn.hostName << " " << nn.listenPort << endl;;
	cout << endl;

	// initial find itself
	found_node.push_back(Eccentricity(node.ID, Round));
	Round++;
	// begin server thread
	thread th_receive(server);
	// send message to neighbor unitl found every node
	while(found_node.size() < total_node) {
		cout << "Current round: " << Round << endl;
		vector<thread> threads;
		// start send message thread
		for(auto &nn : node_neighbors) threads.push_back(thread(client, nn));
		for(auto &t : threads) t.join();
		unique_lock<mutex> lck(mtx);
		Round++;
		cv.notify_all();
		lck.unlock();
		sleep(0.01);
	}
	cout << "\n\n\ncomplete after round: " << Round << ", which is the eccentricity + 1.\n\n\n" << endl;
	
	ofstream outfile;
	outfile.open("config-" + to_string(node.ID) + ".dat", ios::out | ios::trunc);
	outfile << node.ID << endl;
	for(int i = 1; i < total_node; i++) {
		for(auto &fn : found_node) {
			if(fn.round == i) {
				outfile << fn.ID << " ";
			}
		}
		outfile << endl;
	}
	outfile << Round - 1 << endl;

	th_receive.join();

	return 0;
}




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




void server() {
	// initialize
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	// int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	//                                             &opt, sizeof(opt)))
	// {
	//     perror("setsockopt");
	//     exit(EXIT_FAILURE);
	// }

	// listen port is this node's port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(node.listenPort);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 50) < 0) {
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	
	// listen and communicate with client
	vector<thread> threads;
	while(1) {
		if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
			perror("accept failed");
			exit(EXIT_FAILURE);
		}
		threads.push_back(thread(communicate, new_socket));
		if(threads.size() > 3 * total_node) {
			for(auto &t: threads) t.join();
			threads.clear();
		}
	}
	// will not reach here cause server need listen all the time
	for(auto &t : threads) t.join();
	return ;
}

void communicate(int sock) {
	// receive client message
	char buffer[1024] = {0};
	read(sock, buffer, 1024);
	cout << "Server receive buffer: " << string(buffer) << endl;
	int receive_round = stoi(string(buffer));
	// wait until the receive round is the current round
	unique_lock<mutex> lck(mtx);
	while(receive_round > Round && found_node.size() < total_node) cv.wait(lck);
	string send_message;
	for(auto &fn : found_node) {
		if(fn.round < receive_round) send_message += " " + to_string(fn.ID);
	}
	lck.unlock();
	// send the found node in the previous round
	const char * buf = send_message.c_str();
	cout << "Server respond found node on receive_round - 1: ";
	for(auto &fn : found_node) if(fn.round < receive_round) cout << fn.ID << " ";
	cout << endl;
	send(sock, buf, strlen(buf), 0);
}


void client(Node neighbor) {
	// connect
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};
	struct hostent *host_entry = nullptr;
	// try get host ip
	while(1) {
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("\n Socket creation error \n");
			close(sock);
			return ;
		}

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(neighbor.listenPort);


		char hostbuffer[18] = "dc01.utdallas.edu";
		for(int i = 0; i < 4; i++) hostbuffer[i] = neighbor.hostName[i];
		host_entry = gethostbyname(hostbuffer);
		if(host_entry && host_entry->h_addr_list[0]) break;
	}

	char *IPbuffer = nullptr;
	while(IPbuffer == nullptr) {
		IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
	}

	// Convert IPv4 and IPv6 addresses from text to binary form
	// if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	if(inet_pton(AF_INET, IPbuffer, &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		close(sock);
		return ;
	}

	// try connect
	while(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		// printf("\nConnection Failed \n");
	}

	// send current round message
	const char * buf = to_string(Round).c_str();
	send(sock, buf, strlen(buf), 0);
	cout << "Client send message round: " << Round << endl;

	// receive found node message from server
	valread = read(sock, buffer, 1024);
	cout << "Client receive found node: " << string(buffer) << endl;
	close(sock);

	// update found node and the round the node found
	unique_lock<mutex> lck(mtx);
	string rec(buffer);
	istringstream in(rec);
	int id;
	while(in >> id) {
		bool found = false;
		for(auto fn : found_node) {
			if(fn.ID == id) {
				found = true;
				break;
			}
		}
		if(id != node.ID && !found) {
			found_node.push_back(Eccentricity(id, Round));
		}
	}
	cv.notify_all();
	lck.unlock();

	return ;
}