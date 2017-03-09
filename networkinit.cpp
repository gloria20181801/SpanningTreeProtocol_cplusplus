#include <iostream>
#include "bridge.h"
#include <bits/stdc++.h>

using namespace std;

vector<bridge> global_bridges;
vector<lanSegment> global_segs;

int main(){
	string line;
	network n;
	int trace; cin>>trace;
	getline(cin,line);
	int p; cin>>p;
	getline(cin,line);//subtlity to prevent empty line reading messing around
	for(int i=0;i<p;i++){
		getline(cin,line);
		int bridge_id;
		vector<char> lanSeg;
		char temp;
		int pos;
		
		bridge_id = stoi(line.substr(1, line.find(':')));
		pos = line.find(':')+2;
		while(line.find(' ', pos) != string::npos){
			int tp = line.find(' ', pos)-1;
			temp = line[tp];
			lanSeg.push_back(temp);
			pos = tp+2;
			if(pos == line.length()-1){
				lanSeg.push_back(line[pos]);
			}
		}

		bridge b(bridge_id, lanSeg);
		global_bridges.push_back(b);
		n.fill_net_map(bridge_id, lanSeg);
		for(int k=0;k<lanSeg.size();k++){
			vector<lanSegment>::iterator it;
			for(it=global_segs.begin(); it!=global_segs.end();it++){
				if(it->get_name() == lanSeg[k]){
					it->make_inv_net(bridge_id);
					break;
				}					
			}
			if(it == global_segs.end()){
				lanSegment l(lanSeg[k]);
				l.make_inv_net(bridge_id);
				global_segs.push_back(l);
			}
		}
	}//At this point I have prepared the network and its ready for the simulation part
	
	/*********the code to print the network model***********
	cout<<endl<<"The built network model is: "<<endl;
	n.disp_net_map();
	cout<<endl;
	for(auto j = global_bridges.begin(); j != global_bridges.end(); j++){
		j->disp_port_map();
		j->disp_port_state();
		cout<<endl;
	}
	for(auto j = global_segs.begin(); j != global_segs.end(); j++){
		j->disp_inv_net();
	}
	cout<<endl<<"Simulating..."<<endl;
	********************************************************/

	//starting simulation from t=0
	//the simulation will converge in O(#bridges) time
	//for safe side simulation will be running for 2X(O(#bridges)+3)
	//the core idea in simulation is that if at t bridges generate/pass/send the messages to lansegment
	//then at t+1 these messages will be processed and the updates will take place in the bridges (hence the factor of 2)
	
	int finish_t = 2*(n.get_net().size()+3);
	for(int t=0; t<finish_t; t++){
		if(t%2 == 0){
			for(auto it=global_bridges.begin(); it != global_bridges.end(); it++){
				it->send(it->message_gen());
				if(trace){
					cout<<t<<" "<<it->name_gen(it->get_selfid())<<" "<<"s"<<" "<<"("<<it->message_gen()[0]<<", "<<it->message_gen()[1]<<", "<<it->message_gen()[2]<<")"<<endl;
				}
			}
		}
		else{
			for(auto it=global_segs.begin(); it != global_segs.end(); it++){
				if(trace)
					cout<<t<<" ";
				it->process_seg_msg(trace);
			}
		}
	}
	//at this point which evere point is neither marked RP or DP should be marked NP
	for(auto iu = global_bridges.begin(); iu != global_bridges.end(); iu++){
		map<int,int>ps = iu->get_port_state();
		for(auto p=ps.begin(); p != ps.end(); p++){
			if(p->second == -1)
				iu->set_port_state(p->first, 2);
			continue;
		}
	}
	//printing the status of the bridges in spanning tree
	for(auto iu = global_bridges.begin(); iu != global_bridges.end(); iu++){
		iu->disp_port_state();
	}

	//the second part of the simulation
	//for this part also any given transfer between two lan segments
	//will take place in O(#bridge)
	//simulation runtime is set to be 2X(O(#bridge)+ 3)
	for(int z=0; z<global_segs.size(); z++){
		getline(cin,line);
		char seg = line[0];
		vector<int>hosts;
		int temp;
		int pos=3;
		int prev_space = 2;
		while(line.find(' ', pos) != string::npos){
			int tp = line.find(' ',pos);
			temp = stoi(line.substr(pos+1, tp));
			hosts.push_back(temp);
			pos = tp+1;
		}
		if(line.find(' ',pos) == string::npos){
			hosts.push_back(stoi(line.substr(pos+1)));
		}
		for(auto its=global_segs.begin(); its != global_segs.end(); its++){
			if(its->get_name() == seg){
				its->set_hosts(hosts);
				//its->printvec(its->get_hosts());
			}
		}
	}

	int nforwd; cin>>nforwd;
	getline(cin, line);
	for(int i=0;i<nforwd;i++){
		getline(cin,line);
		int src=stoi(line.substr(1,line.find(' ')));
		int dst=stoi(line.substr(line.find(' ')+2));
		for(auto its = global_segs.begin(); its != global_segs.end(); its++){
			vector<int> hst = its->get_hosts();
			for(auto ith=hst.begin(); ith != hst.end(); ith++){
				if(*ith == src){	
					its->push_packet(dst,src,0,0);//intial packet pushed on the right segment
				}
			}
		}

		for(int t=0; t<finish_t; t++){
			if(t%2 == 0){ //time steps at which the packets will be read from all the lansegments
				for(auto its = global_segs.begin(); its != global_segs.end(); its++){
					if(!its->get_packet().empty){
						if(trace)
							cout<<t<<" ";
						its->process_seg_pct(trace);
					}
				}
			}
			else{//time steps at which the packets from bridges will be forwarded
				for(auto itb = global_bridges.begin(); itb != global_bridges.end(); itb++){
					if(!itb->get_packet().empty){
						itb->forward(itb->get_packet());
						if(trace){
							char X,Y;
							for(auto its = global_segs.begin(); its != global_segs.end(); its++){
								vector<int> hst = its->get_hosts();
								for(auto ith=hst.begin(); ith != hst.end(); ith++){
									if(*ith == src)
										X = its->get_name();
									if(*ith == dst)
										Y = its->get_name();
								}
							}
							cout<<t<<" "<<itb->name_gen(itb->get_selfid())<<" "<<"s"<<" "<<X<<"-->"<<Y<<endl;
						}
					}
				}
			}
		}
		//the forwarding tables after this line of input
		for(auto itb = global_bridges.begin(); itb != global_bridges.end(); itb++){
			itb->disp_ftable();
		}
		cout<<endl;
	}
}