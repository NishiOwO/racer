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

	dJointID* joints;
	dGeomID* geoms;
	dBodyID* wheels;

	uint64_t value;
} bodykv_t;

bodykv_t* bodies = NULL;

static dReal height_callback(void* ptr, int x, int z){
	int ind = z * (map_width / map_step) + x;

//	return 1;
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
	int i;

	for(i = 0; i < hmlen(bodies); i++){
		int j;
		for(j = 0; j < arrlen(bodies[i].geoms); j++){
			if(o1 == bodies[i].geoms[j]){
				if(o2 != gheight) return;
				break;
			}else if(o2 == bodies[i].geoms[j]){
				if(o1 != gheight) return;
				break;
			}
		}
	}

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
	int i;
	for(i = 0; i < hmlen(bodies); i++){
		const dReal* p1 = dBodyGetPosition(bodies[i].key);
		const dReal* p = dBodyGetPosition(bodies[i].wheels[0]);
		printf("car %f %f %f\nwheel %f %f %f\n", p1[0], p1[1], p1[2], p[0], p[1], p[2]);

		dJointSetHinge2Param(bodies[i].joints[0], dParamVel2, 200);
		dJointSetHinge2Param(bodies[i].joints[1], dParamVel2, 200);
		dJointSetHinge2Param(bodies[i].joints[2], dParamVel2, 0);
		dJointSetHinge2Param(bodies[i].joints[3], dParamVel2, 0);
		dJointSetHinge2Param(bodies[i].joints[0], dParamFMax2, 0.75);
		dJointSetHinge2Param(bodies[i].joints[1], dParamFMax2, 0.75);
		dJointSetHinge2Param(bodies[i].joints[2], dParamFMax2, 0.75);
		dJointSetHinge2Param(bodies[i].joints[3], dParamFMax2, 0.75);
	}

	dSpaceCollide(space, 0, &near_callback);
	dWorldStep(world, 1.0 / TICK);
	dJointGroupEmpty(contact_joint);
}

void physics_car(uint64_t id, double x, double y, double z){
	dBodyID body = dBodyCreate(world);
	dGeomID st;
	dMass m;
	bodykv_t kv;
	int i;

	dMassSetBox(&m, 1, 0.45 / 2, 0.75 / 2, 0.6 / 2);
	dMassAdjust(&m, 2);

	dBodySetMass(body, &m);
	st = dCreateBox(space, 0.45 / 2, 0.75 / 2, 0.6 / 2);
	dGeomSetBody(st, body);
	dBodySetPosition(body, x, z, y);

	dBodySetMovedCallback(body, move_callback);

	kv.wheels = NULL;
	kv.geoms = NULL;
	kv.joints = NULL;
	for(i = 0; i < 4; i++){
		dGeomID g;
		dBodyID b = dBodyCreate(world);
		dJointID j;
		const dReal* p;
		dMass m;
		dVector3 unit1 = {0, 1, 0};
		dVector3 unit2 = {1, 0, 0};

		dMassSetSphere(&m, 1, TIRE_RADIUS);
		dMassAdjust(&m, 1);
		dBodySetMass(b, &m);
		
		g = dCreateSphere(space, TIRE_RADIUS);
		dGeomSetBody(g, b);

		if(i == 0) dBodySetPosition(b, x - 0.45 / 2, z + 0.75 / 2, y - 0.6 / 2);
		if(i == 1) dBodySetPosition(b, x + 0.45 / 2, z + 0.75 / 2, y - 0.6 / 2);
		if(i == 2) dBodySetPosition(b, x - 0.45 / 2, z - 0.75 / 2, y - 0.6 / 2);
		if(i == 3) dBodySetPosition(b, x + 0.45 / 2, z - 0.75 / 2, y - 0.6 / 2);

		p = dBodyGetPosition(b);

		j = dJointCreateHinge2(world, 0);
		dJointAttach(j, body, b);
		dJointSetHinge2Anchor(j, p[0], p[1], p[2]);
		dJointSetHinge2Axes(j, unit1, unit2);

		dJointSetHinge2Param(j, dParamLoStop, 0);
		dJointSetHinge2Param(j, dParamHiStop, 0);
		dJointSetHinge2Param(j, dParamSuspensionERP, 0.1);
		dJointSetHinge2Param(j, dParamSuspensionCFM, 0.2);

		arrput(kv.geoms, g);
		arrput(kv.wheels, b);
		arrput(kv.joints, j);
	}

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
			int j;
			
			dBodyDestroy(body);
			dGeomDestroy(bodies[i].geom);
			for(j = 0; j < arrlen(bodies[i].wheels); j++){
				dBodyDestroy(bodies[i].wheels[j]);
			}
			for(j = 0; j < arrlen(bodies[i].joints); j++){
				dJointDestroy(bodies[i].joints[j]);
			}
			for(j = 0; j < arrlen(bodies[i].geoms); j++){
				dGeomDestroy(bodies[i].geoms[j]);
			}

			hmdel(bodies, body);
			break;
		}
	}
}
