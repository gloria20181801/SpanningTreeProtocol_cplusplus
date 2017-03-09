#include <iostream>
#include "bridge.h"
#include <bits/stdc++.h>

using namespace std;


/////////////////////definition of network member functions///////////////////////
void network::fill_net_map(int bid, vector<char>lanSeg){
	net[bid] = lanSeg;
}
map<int, vector<char> > network::get_net(){
	return net;
}
void network::disp_net_map(){
	for(auto it = net.begin();it != net.end(); it++){
		cout<<name_gen(it->first)+": ";
		printvec(it->second);
	}
}
void network::printvec(vector<char> v){
	for(auto it=v.begin();it != v.end();it++){
		cout<<*it<<" ";
	}
	cout<<endl;
}
string network::name_gen(int id){
	return "B"+to_string(id);
}
//////////////////////////////////////////////////////////////////////////////////


/////////////////////definiton of bridge member functions/////////////////////////
//SPT related
bridge::bridge(int bid, vector<char>ports_name){
	state[-1]="AP";
	state[0]="RP";
	state[1]="DP";
	state[2]="NP";
	dist_from_root=0;
	selfID = bid;
	rootID = selfID;
	set_ports(ports_name);
	for(int i=0;i<ports_name.size();i++){
		port_state[i] = -1;
	}
	bpacket.empty = true;
}
string bridge::name_gen(int id){
	string name = "B"+to_string(id);
	return name;
}
void bridge::set_selfid(int bid){
	selfID = bid;
}
void bridge::set_ports(vector<char> port_names){
	for(int i=0; i<port_names.size(); i++){
		port_lan_map[i] = port_names[i];
	}
}
void bridge::set_port_state(int id, int st){
	port_state[id] = st;
}
int bridge::get_selfid(){
	return selfID;
}
int bridge::get_curr_rootid(){
	return rootID;
}
int bridge::get_curr_dist_root(){
	return dist_from_root;
}
map<int,char> bridge::get_port_lan_map(){
	return port_lan_map;
}
map<int,int> bridge::get_port_state(){
	return port_state;
}
int bridge::get_port(int pid){
	cout<<port_state[pid]<<" "<<state[port_state[pid]]<<endl;
}
void bridge::read_msg_at_port(int pid){
	for(auto it = global_segs.begin(); it != global_segs.end();it++){
		if(it->get_name() == port_lan_map[pid]){
			vector<vector<int> >msg = it->get_msg();
			for(auto k = msg.begin(); k != msg.end(); k++){
				vector<int> m = *k;
				if(m[0]<best_config_sofar[0]){
					rootID = m[0];
					dist_from_root = m[1]+1;
					best_config_sofar = m;
					set_port_state(pid,0);
				}
				if(m[0]==best_config_sofar[0] && m[1]<best_config_sofar[1]){
					dist_from_root = m[1]+1;
					best_config_sofar = m;
					set_port_state(pid,0);
				}
				if(m[0]==best_config_sofar[0] && m[1]==best_config_sofar[1] && m[2]<best_config_sofar[2]){
					dist_from_root = m[1]+1;
					best_config_sofar = m;
					set_port_state(pid,0);
				}
			}
			continue;
		}
	}
}
vector<int> bridge::message_gen(){
	if(rootID == selfID){
		vector<int> msg(3);
		msg[0] = rootID;
		msg[1] = dist_from_root;
		msg[2] = selfID;
		best_config_sofar = msg;
		return msg;
	}
	else{
		return best_config_sofar;
	}
}
void bridge::send(vector<int>msg){
	for(int i=0;i<port_lan_map.size();i++){
		if(port_state[i] != 2 && port_state[i] != 0){
			vector<lanSegment>::iterator it;
			for(it=global_segs.begin();it != global_segs.end();it++){
				if(it->get_name() == port_lan_map[i]){
					it->push_msg(msg);
					continue;
				}
			}
		}
	}
}
void bridge::disp_port_map(){
	cout<<name_gen(selfID)+": ";
	for(auto it=port_lan_map.begin();it != port_lan_map.end();it++){
		cout<<it->first<<"-"<<it->second<<" ";
	}
	cout<<endl;
}
void bridge::disp_port_state(){
	cout<<name_gen(selfID)+": ";
	for(auto it=port_state.begin();it != port_state.end(); it++){
		cout<<port_lan_map[it->first]<<"-"<<state[it->second]<<" ";
	}
	cout<<endl;
}

//forwarding related
void bridge::set_packet(int d, int s, int pid){
	bpacket.dst = d;
	bpacket.src = s;
	bpacket.bid = selfID;
	bpacket.pid = pid;
	bpacket.empty = false;
}
struct bridge::pckt bridge::get_packet(){
	return bpacket;
}
void bridge::flush_pct(){
	bpacket.empty = true;
}
void bridge::read_pct_at_port(int pid){
	for(auto it = global_segs.begin(); it != global_segs.end();it++){
		if(it->get_name() == port_lan_map[pid]){
			fill_ftable(it->get_packet().src, pid);
			set_packet(it->get_packet().dst, it->get_packet().src, pid);
		}
	}
}
void bridge::fill_ftable(int src, int pid){
	if(forwardtable.empty()){
		forwardtable[src] = port_lan_map[pid];
	}
	else{
		auto itf = forwardtable.begin();
		for(itf; itf != forwardtable.end(); itf++){
			if(itf->first == src){
				break;
			}
		}
		if(itf == forwardtable.end())
			forwardtable[src] = port_lan_map[pid];
	}
}
void bridge::send_to(pckt p, int pid){
	for(auto it=global_segs.begin(); it != global_segs.end(); it++){
		if(it->get_name() == port_lan_map[pid]){
			it->push_packet(p.dst,p.src,p.bid,p.pid);
		}
		continue;
	}
}
void bridge::forward(pckt p){
	if(forwardtable.empty()){
		for(int j=0; j<port_lan_map.size();j++){
			if(p.pid != j && port_state[j] != 2){
				send_to(p, j);
			}
		}
	}
	else{
			map<int,char>::iterator itt;
			for(itt=forwardtable.begin(); itt != forwardtable.end(); itt++){
			if(itt->first == p.dst){	
				char lsdst,lssrc;
				for(auto its = global_segs.begin(); its != global_segs.end(); its++){
					vector<int> hst = its->get_hosts();
					for(auto ith=hst.begin(); ith != hst.end(); ith++){
						if(*ith == p.dst)
							lsdst =its->get_name();
						if(*ith == p.src)
							lssrc =its->get_name();
					}
				}
				//cout<<lsdst<<" "<<lssrc<<endl;
				if(lsdst != lssrc){
					for(int j=0;j<port_lan_map.size();j++){
						if(port_lan_map[j] == itt->second){
							send_to(p, j);
						}
					}
				}
				break;
			}
			
		}
		if(itt == forwardtable.end()){
			for(int j=0; j<port_lan_map.size();j++){
				if(p.pid != j && port_state[j] != 2){
					send_to(p, j);
				}
			}
		}
	}
	flush_pct();
}
void bridge::disp_ftable(){
	cout<<name_gen(selfID)<<endl;
	cout<<"HOST ID|FORWARDING PORT"<<endl;
	if(forwardtable.empty())
		cout<<"EMPTY"<<endl;
	else{
		for(auto itt = forwardtable.begin(); itt != forwardtable.end(); itt++){
			cout<<"H"+to_string(itt->first)<<"|"<<itt->second<<endl;
		}
	}	
}
//////////////////////////////////////////////////////////////////////////////////


//////////////////////definition of lanSegment member function////////////////////
//SPT related
lanSegment::lanSegment(char c){
	seg_name = c;
	packet.empty = true;
}
void lanSegment::make_inv_net(int Bid){
	inv_net.push_back(Bid);
}
void lanSegment::disp_inv_net(){
	cout<<seg_name<<": ";
	printvec(inv_net);
}
char lanSegment::get_name(){
	return seg_name;
}
string lanSegment::host_name(int hid){
	return "H"+to_string(hid);
}
vector<int> lanSegment::get_hosts(){
	return hosts;
}
void lanSegment::set_hosts(vector<int> hst){
	hosts = hst;
}
vector<vector<int> > lanSegment::get_msg(){
	return messages;
}
vector<int> lanSegment::get_inv_net(){
	return inv_net;
}
void lanSegment::push_msg(vector<int> msg){
	messages.push_back(msg);
}
void lanSegment::printvec(vector<int> v){
	for(auto it=v.begin();it != v.end();it++){
		cout<<*it<<" ";
	}
	cout<<endl;
}
void lanSegment::process_seg_msg(int trace){
	auto DB = global_bridges.begin();
	for(auto ptr = global_bridges.begin(); ptr != global_bridges.end(); ptr++){
		if(ptr->get_selfid() == inv_net[0])
			DB = ptr;
	}//initialised a designated bridge DB
	for(auto it=inv_net.begin(); it != inv_net.end(); it++){
		for(auto itb=global_bridges.begin(); itb != global_bridges.end(); itb++){
			if(itb->get_selfid() == *it){
				auto port_lan_map = itb->get_port_lan_map();
				for(auto itp = port_lan_map.begin(); itp != port_lan_map.end(); itp++){
					if(itp->second == seg_name){
						itb->read_msg_at_port(itp->first);
						if(trace){
							cout<<itb->name_gen(itb->get_selfid())<<" "<<"r"<<" "<<"("<<itb->message_gen()[0]<<", "<<itb->message_gen()[1]<<", "<<itb->message_gen()[2]<<")"<<endl;
						}
						if(itb->get_curr_dist_root() < DB->get_curr_dist_root())
							DB = itb;
					}
					continue;
				}
				continue;
			}
		}
	}//updates have taken place in all the lan connected to this lan segement
	 //DB has also been found out, just need to find DP in that bridge
	auto port_lan_map = DB->get_port_lan_map();
	for(auto ptr = port_lan_map.begin(); ptr != port_lan_map.end(); ptr++){
		if(ptr->second == seg_name){
			DB->set_port_state(ptr->first, 1);
		}
		continue;
	}
	flush_seg();
}
void lanSegment::flush_seg(){
	messages.clear();
}

//forwarding related
struct lanSegment::pckt lanSegment::get_packet(){
	return packet;
}
void lanSegment::push_packet(int d, int s, int bid, int pid){
	packet.dst=d;
	packet.src=s;
	packet.bid=bid;
	packet.pid=pid;
	packet.empty = false;
}
void lanSegment::process_seg_pct(int trace){
	for(auto k=inv_net.begin(); k != inv_net.end(); k++){
		for(auto itb = global_bridges.begin(); itb != global_bridges.end(); itb++){
			if(itb->get_selfid() == *k){
				map<int,char>plmap = itb->get_port_lan_map();
				map<int,int>pstate = itb->get_port_state();
				for(auto itm = plmap.begin(); itm != plmap.end(); itm++){
					if(itm->second == seg_name && pstate[itm->first] != 2){
						if(trace){
							char X,Y;
							X = packet.src;
							Y = packet.dst;
							cout<<itb->name_gen(itb->get_selfid())<<" "<<"r"<<" "<<X<<"-->"<<Y<<endl;
						}
							
						itb->read_pct_at_port(itm->first);
					}
				}
			}
		}
	}
	flush_pct();
}
void lanSegment::flush_pct(){
	packet.empty = true;
}
//////////////////////////////////////////////////////////////////////////////////