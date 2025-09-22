#include <racer.h>

#include <math.h>

typedef struct player {
	uint64_t key;
	double x;
	double y;
	double z;
	double rx;
	double ry;
	double rz;
	void* value;
} player_t;

static player_t* players = NULL;

static RGFW_window* win;
static GLUquadric* quad;

static double cam_x = -1;
static double cam_y = 2;
static double cam_z = -1;
static double cam_lr = 45;
static double cam_ud = -22.5;
static GLfloat fog_color[] = {
	0.6, 0.8, 1, 1
};

static GLuint map_id, yugo_id;

void draw_set(uint64_t id, double x, double y, double z, double rx, double ry, double rz){
	player_t p;

	p.key = id;
	p.x = x;
	p.y = y;
	p.z = z;
	p.rx = rx;
	p.ry = ry;
	p.rz = rz;

	hmputs(players, p);
}

void draw_delete(uint64_t id){
	hmdel(players, id);
}

static void camera(void){
	i32 w, h;
	double aspect;
	GLfloat light_pos[4];

	light_pos[0] = 0;
	light_pos[1] = 1;
	light_pos[2] = 0;
	light_pos[3] = 0;
	
	RGFW_window_getSize(win, &w, &h);
	glViewport(0, 0, w, h);

	aspect = (double)w / (double)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, aspect, 0.01, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam_x, cam_y, cam_z, cam_x + 5 * cos(cam_lr / 180 * M_PI), cam_y + 5 * sin(cam_ud / 180 * M_PI), cam_z + 5 * sin(cam_lr / 180 * M_PI), 0, 1, 0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

static void draw_cyl(double r, double h){
	const int c = 10;

	glPushMatrix();

	gluQuadricOrientation(quad, GLU_OUTSIDE);
	gluCylinder(quad, r, r, h, c, c);
	gluQuadricOrientation(quad, GLU_INSIDE);
	gluDisk(quad, 0, r, c, c);
	glTranslatef(0, 0, h);
	gluQuadricOrientation(quad, GLU_OUTSIDE);
	gluDisk(quad, 0, r, c, c);
	
	glPopMatrix();
}

static void draw_yugo(double x, double y, double z, double rx, double ry, double rz, double tirerot){
	const double tw = TIRE_WIDTH;
	const double tr = TIRE_RADIUS;

	glColor3f(0.7, 0.2, 0.1);

	glPushMatrix();

	glTranslatef(x, y - 0.3 / 2, z);

	glRotatef(-ry, 0, 1, 0);
	glRotatef(-rz, 0, 0, 1);
	glRotatef(-rx, 1, 0, 0);
	glCallList(yugo_id);

	glColor3f(0.2, 0.2, 0.2);

	glPushMatrix();
	glTranslatef(-0.45 / 2 - tw / 2, 0, 0.75 / 2 - tr * 1.5);
	glRotatef(90, 0, 1, 0);
	glRotatef(tirerot, 0, 0, 1);
	draw_cyl(tr, tw);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.45 / 2 - tw / 2, 0, -0.75 / 2 + tr * 1.5);
	glRotatef(90, 0, 1, 0);
	glRotatef(tirerot, 0, 0, 1);
	draw_cyl(tr, tw);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.45 / 2 - tw / 2, 0, 0.75 / 2 - tr * 1.5);
	glRotatef(90, 0, 1, 0);
	glRotatef(tirerot, 0, 0, 1);
	draw_cyl(tr, tw);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.45 / 2 - tw / 2, 0, -0.75 / 2 + tr * 1.5);
	glRotatef(90, 0, 1, 0);
	glRotatef(tirerot, 0, 0, 1);
	draw_cyl(tr, tw);
	glPopMatrix();

	glPopMatrix();
}

void draw_init(void){
	RGFW_setClassName("Racer");
	win = RGFW_createWindow("Racer", 0, 0, 800, 600, RGFW_windowCenter | RGFW_windowOpenGL);
	RGFW_window_setName(win, "Racer");

	RGFW_window_makeCurrentContext_OpenGL(win);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogi(GL_FOG_START, 2);
	glFogi(GL_FOG_END, 16);
	glFogfv(GL_FOG_COLOR, fog_color);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	quad = gluNewQuadric();

	camera();

	map_id = glGenLists(1);
	yugo_id = glGenLists(1);

	glNewList(yugo_id, GL_COMPILE);
	glPushMatrix();
		glScalef(0.5, 0.5, 0.5);
		yugo();
	glPopMatrix();
	glEndList();
}

double get_height(double x, double y){
	int pos;

	x /= map_step;
	y /= map_step;

	pos = y * (map_width / map_step) + x;

	if(pos >= arrlen(map_height)) return 0;

	return map_height[pos];
}

static int first = 0;
static double cx = 0, cz = 0;

void draw_set_first(void){
	first = 1;
}

static int look_behind = 0;

void draw(void){
	double y, x;
	int i;
	RGFW_event ev;

	while(RGFW_window_checkEvent(win, &ev)){
		if(ev.type == RGFW_keyPressed && ev.key.value == RGFW_b){
			look_behind = 1;
		}else if(ev.type == RGFW_keyReleased && ev.key.value == RGFW_b){
			look_behind = 0;
		}
	}

	glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera();

	if(first){
		first = 0;

		cam_x = map_width / 2;
		cam_y = 1.5;
		cam_z = map_width / 2;

		cx = cam_x;
		cz = cam_z;

		glNewList(map_id, GL_COMPILE);

		glDisable(GL_LIGHTING);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for(y = 0; y < map_width - map_step; y += map_step){
			glBegin(GL_TRIANGLE_STRIP);
			for(x = 0; x < map_width; x += map_step){
				double c;
	
				glNormal3f(0, 1, 0);
				c = get_height(x, y);
				glColor3f(c, 0.7, 0);
				glVertex3f(x, c, y);
	
				c = get_height(x, y + map_step);
				glColor3f(c, 0.7, 0);
				glVertex3f(x, c, y + map_step);
			}
			glEnd();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for(y = 0; y < map_width - map_step; y += map_step){
			glBegin(GL_QUAD_STRIP);
			for(x = 0; x < map_width; x += map_step){
				double c;
	
				c = get_height(x, y);
				glColor3f(0, 0, 0);
				glVertex3f(x, c, y);
	
				c = get_height(x, y + map_step);
				glColor3f(0, 0, 0);
				glVertex3f(x, c, y + map_step);
			}
			glEnd();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_LIGHTING);

		glEndList();
	}

	glCallList(map_id);

	if(player_id != 0){
		player_t p = hmgets(players, player_id);
		cam_x = p.x + cx;
		cam_y = p.y + 1;

		if(look_behind){
			cam_z = p.z + 1 + cz;
			cam_lr = -90;
		}else{
			cam_z = p.z - 1 + cz;
			cam_lr = 90;
		}

		cam_ud = -45;
	}

	for(i = 0; i < shlen(players); i++){
		draw_yugo(players[i].x + cx, players[i].y + 0.6 / 2, players[i].z + cz, players[i].rx, players[i].ry, players[i].rz, 0);
	}

	RGFW_window_swapBuffers_OpenGL(win);
}
