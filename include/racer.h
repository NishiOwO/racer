#ifndef __RACER_H__
#define __RACER_H__

#include <stb_ds.h>

#include <stdio.h>

#ifdef RACER_CLIENT
#define RGFW_OPENGL
#define RGFWDEF
#include <RGFW.h>

#include <GL/gl.h>
#include <GL/glu.h>
#endif
#ifdef RACER_SERVER
#include <stb_image.h>
#endif

#ifdef __cplusplus
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>

enum packet_enum {
	/* width(double) step(double) data(width^2 doubles) */
	ID_TERRAIN = ID_USER_PACKET_ENUM + 1,
	/* id(uint64_t) */
	ID_ASSIGNED,
	/* id(uint64_t) x(double) y(double) z(double) rx(double) ry(double) rz(double) */
	ID_CAR,
	/* id(uint64_t) */
	ID_DESTROYED,
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define HOST "127.0.0.1"
#define PORT 12831
#define MAXCLIENTS 32
#define FPS 60
#define TICK 50

extern double map_step;
extern double map_width;
extern double* map_height;

void net_init(void);
void net_loop(void);
#ifdef RACER_CLIENT
void yugo(void);

void draw(void);
void draw_set_first(void);
void draw_init(void);
void draw_set(uint64_t id, double x, double y, double z, double rx, double ry, double rz);
void draw_delete(uint64_t id);

extern uint64_t player_id;
#endif
#ifdef RACER_SERVER
void physics_init(void);
void physics(void);
void physics_car(uint64_t id, double x, double y, double z);
void physics_kill(uint64_t id);

void net_moved(uint64_t id, double x, double y, double z, double rx, double ry, double rz);
#endif

#ifdef __cplusplus
}
#endif

#endif
