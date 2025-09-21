#include <racer.h>

#include <math.h>

#include <ode/ode.h>

dWorldID world;
dSpaceID space;
dHeightfieldDataID height;
dGeomID gheight;
dJointGroupID contact_joint;

typedef struct bodykv {
	dBodyID key;
	dGeomID geom;
	uint64_t value;
} bodykv_t;

bodykv_t* bodies = NULL;

static dReal height_callback(void* ptr, int x, int z){
	int ind = z * (map_width / map_step) + x;

	return map_height[ind] + 1;
}

void physics_init(void){
	dMatrix3 R;

	dInitODE();

	world = dWorldCreate();
	dWorldSetGravity(world, 0, 0, -9.81);

	space = dSimpleSpaceCreate(0);

	contact_joint = dJointGroupCreate(0);

	dCreatePlane(space, 0, 0, 1, 0);

	height = dGeomHeightfieldDataCreate();

	dGeomHeightfieldDataBuildCallback(height, NULL, height_callback,
			map_width, map_width, map_width / map_step, map_width / map_step,
			1.0, 0.0, 0.0, 0);

	gheight = dCreateHeightfield(space, height, 1);

	dRSetIdentity(R);
	dRFromAxisAndAngle(R, 1, 0, 0, 90.0 / 180 * M_PI);

	dGeomSetRotation(gheight, R);
	dGeomSetPosition(gheight, 0, 0, 0);
}

static void near_callback(void* data, dGeomID o1, dGeomID o2){
	const int N = 256;
	dContact contact[256];
	if(dGeomIsSpace(o1) || dGeomIsSpace(o2)){

		dSpaceCollide2(o1, o2, data, &near_callback);
		if(dGeomIsSpace(o1)) dSpaceCollide((dSpaceID)o1, data, &near_callback);
		if(dGeomIsSpace(o2)) dSpaceCollide((dSpaceID)o2, data, &near_callback);
	}else{
		int how_many = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
		int i;
		for(i = 0; i < how_many; i++){
			dJointID joint;
			contact[i].surface.mode = dContactApprox1;
			contact[i].surface.mu = 2;

			joint = dJointCreateContact(world, contact_joint, &contact[i]);
			dJointAttach(joint, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
		}
	}
}

static void move_callback(dBodyID id){
	uint64_t pkt = hmget(bodies, id);
	const dReal* pos = dBodyGetPosition(id);

	/** https://demura.net/robot/9ode/511.html **/
	const dReal *rot = dBodyGetRotation(id);
	dReal r11,r12,r13,r21,r22,r23,r31,r32,r33;
	dReal pitch, yaw, roll;

	r11 = rot[0];    r12 = rot[1];    r13 = rot[2];
	r21 = rot[4];    r22 = rot[5];    r23 = rot[6];
	r31 = rot[8];    r32 = rot[9];    r33 = rot[10];

	pitch = atan2(-r31, sqrt(r32*r32+r33*r33));
	yaw   = atan2(r21,r11);
	roll  = atan2(r32,r33);
	/** ********** **/

	roll *= 180.0 / M_PI;
	yaw *= 180.0 / M_PI;
	pitch *= 180.0 / M_PI;

	net_moved(pkt, pos[0], pos[2], pos[1], roll, yaw, pitch);
}

void physics(void){
	dSpaceCollide(space, 0, &near_callback);
	dWorldStep(world, 1.0 / TICK);
	dJointGroupEmpty(contact_joint);
}

void physics_car(uint64_t id, double x, double y, double z){
	dBodyID body = dBodyCreate(world);
	dGeomID st;
	dMass m;
	bodykv_t kv;

	dMassSetBox(&m, 1, 1, 1, 1);
	dMassAdjust(&m, 100);

	dBodySetMass(body, &m);
	st = dCreateBox(space, 0.45 / 2, 0.75 / 2, 0.6 / 2);
	dGeomSetBody(st, body);
	dBodySetPosition(body, x, z, y);

	dBodySetMovedCallback(body, move_callback);

	kv.key = body;
	kv.value = id;
	kv.geom = st;
	hmputs(bodies, kv);
}

void physics_kill(uint64_t id){
	int i;
	for(i = 0; i < hmlen(bodies); i++){
		if(bodies[i].value == id){
			dBodyID body = bodies[i].key;
			
			dBodyDestroy(body);
			dGeomDestroy(bodies[i].geom);

			hmdel(bodies, body);
			break;
		}
	}
}
