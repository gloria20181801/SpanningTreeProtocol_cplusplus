#include <iostream>
#include <bits/stdc++.h>

using namespace std;


/////////////////////class definitions////////////////////////////////////////////
class network{
private:
	map<int, vector<char> >net;
	string inp_line;
public:
	void fill_net_map(int,vector<char>);
	map<int, vector<char> >get_net();
	void disp_net_map();
	void printvec(vector<char>);
	string name_gen(int);
};

class bridge{
private:
	//SPT related
	map<int,char>port_lan_map;
	map<int,int>port_state;
	map<int,string> state;
	int rootID;
	int selfID;
	int dist_from_root;
	vector<int>best_config_sofar;
	//forwarding related
	struct pckt{int dst; int src; int bid; int pid; bool empty;} bpacket;
	map<int,char> forwardtable;

public:
	//SPT related
	bridge(int, vector<char>);
	vector<int> message_gen();
	void send(vector<int>);
	string name_gen(int);
	void set_selfid(int);
	void set_ports(vector<char>);
	void set_port_state(int,int);
	int get_selfid();
	int get_curr_rootid();
	int get_curr_dist_root();
	map<int,char> get_port_lan_map();
	map<int,int> get_port_state();
	int get_port(int);
	void read_msg_at_port(int);
	void disp_port_map();
	void disp_port_state();
	//forwarding related
	void set_packet(int, int, int);
	pckt get_packet();
	void flush_pct();
	void read_pct_at_port(int);
	void send_to(pckt, int);
	void fill_ftable(int, int);
	void forward(pckt);
	void disp_ftable();
};
extern vector<bridge> global_bridges;

class lanSegment{
private:
	//SPT related
	char seg_name;
	vector<vector<int> > messages;//vector will store the messages recieved by segment in order
	vector<int> inv_net;//to know which bridges are connected to this lanseg
	int designated_Bid;
	//forwarding related
	struct pckt{int dst; int src; int bid; int pid; bool empty;} packet;
	vector<int>hosts;
	
public:
	//SPT related
	lanSegment(char);
	char get_name();
	vector<vector<int> >get_msg();
	vector<int> get_inv_net();
	void make_inv_net(int);
	void push_msg(vector<int>);
	void process_seg_msg(int);
	void flush_seg();
	void disp_inv_net();
	void printvec(vector<int>);
	//forwarding related
	pckt get_packet();
	string host_name(int);
	void set_hosts(vector<int>);
	vector<int> get_hosts();
	void process_seg_pct(int);
	void flush_pct();
	void push_packet(int,int,int,int);
};
extern vector<lanSegment> global_segs;
//////////////////////////////////////////////////////////////////////////////////