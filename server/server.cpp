#include <racer.h>

#include <GetTime.h>
#include <RakSleep.h>

RakNet::RakPeerInterface* peer;
RakNet::SocketDescriptor sd;

typedef struct player {
	RakNet::SystemAddress key;
	uint64_t value;
} player_t;

player_t* players = NULL;

void net_init(void){
	sd = RakNet::SocketDescriptor(PORT, 0);
	peer = RakNet::RakPeerInterface::GetInstance();
	peer->Startup(MAXCLIENTS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAXCLIENTS);
}

void net_loop(void){
	RakNet::Packet* packet;
	RakNet::Time t1, t2, diff = 0;

	printf("server ready\n");
	t1 = RakNet::GetTimeMS();
	while(1){
		for(packet = peer->Receive(); packet != NULL; peer->DeallocatePacket(packet), packet = peer->Receive()){
			if(packet->data[0] == ID_NEW_INCOMING_CONNECTION){
				RakNet::BitStream bs, bs2;
				int i;
				uint64_t id;
				double w = map_width / map_step;

				bs.Write((RakNet::MessageID)ID_TERRAIN);
				bs.Write(map_width);
				bs.Write(map_step);
				for(i = 0; i < w * w; i++){
					bs.Write(map_height[i]);
				}
				peer->Send(&bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, packet->systemAddress, false);

	
				id = RakNet::RakPeerInterface::Get64BitUniqueRandomNumber();
				bs2.Write((RakNet::MessageID)ID_ASSIGNED);
				bs2.Write(id);
				peer->Send(&bs2, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);

				hmput(players, packet->systemAddress, id);
				physics_car(id, 0, 3, 0);

				printf("connection accepted, %llx\n", id);
			}else if(packet->data[0] == ID_CONNECTION_LOST){
				uint64_t id = hmget(players, packet->systemAddress);
				RakNet::BitStream bs;

				physics_kill(id);

				bs.Write((RakNet::MessageID)ID_DESTROYED);
				bs.Write(id);
				peer->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

				printf("connection lost, %llx\n", id);
			}
		}
		physics();

		t2 = RakNet::GetTimeMS();
		diff = t2 - t1;
		if(diff <= (1000 / TICK)){
			RakSleep((1000 / TICK) - diff);
		}else{
			printf("!!! Server running slow !!!\n");
		}
		t1 = RakNet::GetTimeMS();
	}
}

void net_moved(uint64_t id, double x, double y, double z, double rx, double ry, double rz){
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)ID_CAR);
	bs.Write(id);
	bs.Write(x);
	bs.Write(y);
	bs.Write(z);
	bs.Write(rx);
	bs.Write(ry);
	bs.Write(rz);
	peer->Send(&bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
