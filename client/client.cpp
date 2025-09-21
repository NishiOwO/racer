#include <racer.h>

#include <GetTime.h>
#include <RakSleep.h>

RakNet::RakPeerInterface* peer;
RakNet::SocketDescriptor sd;
RakNet::SystemAddress server;

double map_step;
double map_width;
double* map_height = NULL;
uint64_t player_id = 0;

void net_init(void){
	peer = RakNet::RakPeerInterface::GetInstance();
	peer->Startup(1, &sd, 1);
	peer->Connect(HOST, PORT, 0, 0);
}

void net_loop(void){
	RakNet::Packet* packet;
	RakNet::Time t1, t2, diff = 0;

	t1 = RakNet::GetTimeMS();
	while(1){
		for(packet = peer->Receive(); packet != NULL; peer->DeallocatePacket(packet), packet = peer->Receive()){
			if(packet->data[0] == ID_TERRAIN){
				RakNet::BitStream bs(packet->data, packet->length, false);
				int x, y;
				double w, d;

				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(map_width);
				bs.Read(map_step);

				w = map_width / map_step;

				printf("width=%f, step=%f\n", map_width, map_step);
				printf("expecting %d datas\n", (int)(w * w));
				for(y = 0; y < w; y++){
					for(x = 0; x < w; x++){
						bs.Read(d);
						arrput(map_height, d);
					}
				}
				printf("loaded map\n");

				draw_set_first();
			}else if(packet->data[0] == ID_ASSIGNED){
				RakNet::BitStream bs(packet->data, packet->length, false);

				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(player_id);

				printf("my id is %llx\n", player_id);
			}else if(packet->data[0] == ID_DESTROYED){
				RakNet::BitStream bs(packet->data, packet->length, false);
				uint64_t id;

				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(id);

				printf("%llx disconnected\n", id);

				draw_delete(id);
			}else if(packet->data[0] == ID_CAR){
				RakNet::BitStream bs(packet->data, packet->length, false);
				uint64_t sender;
				double x, y, z, rx, ry, rz;

				bs.IgnoreBytes(sizeof(RakNet::MessageID));
				bs.Read(sender);
				bs.Read(x);
				bs.Read(y);
				bs.Read(z);
				bs.Read(rx);
				bs.Read(ry);
				bs.Read(rz);

				draw_set(sender, x, y, z, rx, ry, rz);
			}
		}
		draw();

		t2 = RakNet::GetTimeMS();
		diff = t2 - t1;
		if(diff <= (1000 / FPS)){
			RakSleep((1000 / FPS) - diff);
		}
		t1 = RakNet::GetTimeMS();
	}
}
