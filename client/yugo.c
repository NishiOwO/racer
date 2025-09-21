#include <racer.h>

void yugo(void){
	int i;
	GLbitfield attrbit = GL_ALL_ATTRIB_BITS;

	/* left/right */
	for(i = 0; i < 2; i++){
		glPushMatrix();
		if(i == 1) glRotatef(180, 0, 1, 0);

		glBegin(GL_QUADS);
			glNormal3f(-1, 0, 0);
			glVertex3f(-0.45, 0.3, 0.75);
			glVertex3f(-0.45, 0, 0.75);
			glVertex3f(-0.45, 0, -0.75);
			glVertex3f(-0.45, 0.3, -0.75);
		glEnd();

		glTranslatef(-0.001, 0, 0);

		glPushAttrib(attrbit);
		glColor3f(0.1, 0.1, 0.1);
		glBegin(GL_QUADS);
			glNormal3f(-1, 0, 0);
			glVertex3f(-0.45, 0.15, 0.75);
			glVertex3f(-0.45, 0.1, 0.75);
			glVertex3f(-0.45, 0.1, -0.75);
			glVertex3f(-0.45, 0.15, -0.75);
		glEnd();
		glPopAttrib();

		glPopMatrix();
	}

	/* front */
	glPushAttrib(attrbit);
	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glVertex3f(-0.45, 0.15, 0.75);
		glVertex3f(-0.45, 0, 0.75);
		glVertex3f(0.45, 0, 0.75);
		glVertex3f(0.45, 0.15, 0.75);
	glEnd();

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glVertex3f(-0.45, 0.3, 0.75);
		glVertex3f(-0.45, 0.15, 0.75);
		glVertex3f(0.45, 0.15, 0.75);
		glVertex3f(0.45, 0.3, 0.75);
	glEnd();

	/* light */
	for(i = 0; i < 2; i++){
		glPushMatrix();
		glTranslatef(-0.45 + (i == 0 ? 0.02 : (0.9 - 0.15)), 0, 0.75 + 0.001);

		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
			glNormal3f(0, 0, 1);
			glVertex3f(0, 0.28, 0);
			glVertex3f(0, 0.17, 0);
			glVertex3f(0.13, 0.17, 0);
			glVertex3f(0.13, 0.28, 0);
		glEnd();

		glPopMatrix();
	}
	glPopAttrib();

	/* bonnet */
	glBegin(GL_QUADS);
		glNormal3f(0, 0.7, 0.2);
		glVertex3f(-0.45, 0.35, 0.25);
		glVertex3f(-0.45, 0.3, 0.75);
		glVertex3f(0.45, 0.3, 0.75);
		glVertex3f(0.45, 0.35, 0.25);
	glEnd();

	/* bonnet left spacing */
	glBegin(GL_TRIANGLES);
		glNormal3f(-1, 0, 0);
		glVertex3f(-0.45, 0.3, 0.25);
		glVertex3f(-0.45, 0.3, 0.75);
		glVertex3f(-0.45, 0.35, 0.25);
	glEnd();

	/* bonnet right spacing */
	glBegin(GL_TRIANGLES);
		glNormal3f(1, 0, 0);
		glVertex3f(0.45, 0.3, 0.25);
		glVertex3f(0.45, 0.3, 0.75);
		glVertex3f(0.45, 0.35, 0.25);
	glEnd();

	/* back spacing left */
	glBegin(GL_QUADS);
		glNormal3f(-1, 0, 0);
		glVertex3f(-0.45, 0.6, -0.5);
		glVertex3f(-0.45, 0.3, -0.75);
		glVertex3f(-0.45, 0.3, -0.6);
		glVertex3f(-0.45, 0.6, -0.4);
	glEnd();

	/* back spacing right */
	glBegin(GL_QUADS);
		glNormal3f(1, 0, 0);
		glVertex3f(0.45, 0.6, -0.5);
		glVertex3f(0.45, 0.3, -0.75);
		glVertex3f(0.45, 0.3, -0.6);
		glVertex3f(0.45, 0.6, -0.4);
	glEnd();

	/* back */
	glPushAttrib(attrbit);
	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_QUADS);
		glNormal3f(0, 0.5, -0.7);
		glVertex3f(-0.45, 0.15, -0.75);
		glVertex3f(-0.45, 0, -0.75);
		glVertex3f(0.45, 0, -0.75);
		glVertex3f(0.45, 0.15, -0.75);
	glEnd();

	/* left light */
	glPushMatrix();
	glTranslatef(-0.45 + 0.15, 0.35 - 0.125, -0.75 - 0.0005);

	glColor3f(0.5, 0.5, 0);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.0725, 0.05, 0);
		glVertex3f(-0.0725, -0.05, 0);
		glVertex3f(0.0725, -0.05, 0);
		glVertex3f(0.0725, 0.05, 0);
	glEnd();

	glTranslatef(0, 0, -0.0005);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(0.0725, 0.05, 0);
		glVertex3f(0.0725, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.05, 0);
	glEnd();

	glPopMatrix();

	/* right light */
	glPushMatrix();
	glTranslatef(0.45 - 0.15, 0.35 - 0.125, -0.75 - 0.0005);

	glColor3f(0.5, 0.5, 0);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.0725, 0.05, 0);
		glVertex3f(-0.0725, -0.05, 0);
		glVertex3f(0.0725, -0.05, 0);
		glVertex3f(0.0725, 0.05, 0);
	glEnd();

	glTranslatef(0, 0, -0.0005);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.0725, 0.05, 0);
		glVertex3f(-0.0725, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.05, 0);
	glEnd();

	glPopMatrix();

	glPopAttrib();

	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.45, 0.3, -0.75);
		glVertex3f(-0.45, 0.15, -0.75);
		glVertex3f(0.45, 0.15, -0.75);
		glVertex3f(0.45, 0.3, -0.75);
	glEnd();

	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.45, 0.6, -0.5);
		glVertex3f(-0.45, 0.3, -0.75);
		glVertex3f(0.45, 0.3, -0.75);
		glVertex3f(0.45, 0.6, -0.5);
	glEnd();

	glPushAttrib(attrbit);
	glColor3f(0.8, 0.8, 1);

	/* left window */
	glBegin(GL_QUADS);
		glNormal3f(-1, 0, 0);
		glVertex3f(-0.45, 0.6, -0.4);
		glVertex3f(-0.45, 0.3, -0.6);
		glVertex3f(-0.45, 0.3, 0.2);
		glVertex3f(-0.45, 0.6, 0.05);
	glEnd();

	/* right window */
	glBegin(GL_QUADS);
		glNormal3f(1, 0, 0);
		glVertex3f(0.45, 0.6, -0.4);
		glVertex3f(0.45, 0.3, -0.6);
		glVertex3f(0.45, 0.3, 0.2);
		glVertex3f(0.45, 0.6, 0.05);
	glEnd();

	glPopAttrib();

	/* back window */
	glPushMatrix();
	glTranslatef(0, 0, -0.001);

	glPushAttrib(attrbit);
	glColor3f(0.8, 0.8, 1);

	glBegin(GL_QUADS);
		glNormal3f(0, 0.5, -0.7);
		glVertex3f(-0.35, 0.55, -0.55);
		glVertex3f(-0.35, 0.35, -0.7075);
		glVertex3f(0.35, 0.35, -0.7075);
		glVertex3f(0.35, 0.55, -0.55);
	glEnd();

	glTranslatef(0, 0, 0.001);

	/* front window */
	glBegin(GL_QUADS);
		glNormal3f(0, 0.5, 0.7);
		glVertex3f(-0.45, 0.6, 0.1);
		glVertex3f(-0.45, 0.35, 0.25);
		glVertex3f(0.45, 0.35, 0.25);
		glVertex3f(0.45, 0.6, 0.1);
	glEnd();

	glPopMatrix();

	/* down */
	glColor3f(0.1, 0.1, 0.1);

	glBegin(GL_QUADS);
		glNormal3f(0, -1, 0);
		glVertex3f(-0.45, 0, 0.75);
		glVertex3f(-0.45, 0, -0.75);
		glVertex3f(0.45, 0, -0.75);
		glVertex3f(0.45, 0, 0.75);
	glEnd();

	glPopAttrib();

	/* up */
	glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);
		glVertex3f(-0.45, 0.6, -0.5);
		glVertex3f(-0.45, 0.6, 0.1);
		glVertex3f(0.45, 0.6, 0.1);
		glVertex3f(0.45, 0.6, -0.5);
	glEnd();

	/* front spacing left */
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(-1, 0, 0);
		glVertex3f(-0.45, 0.6, 0.05);
		glVertex3f(-0.45, 0.6, 0.1);
		glVertex3f(-0.45, 0.35, 0.25);
		glVertex3f(-0.45, 0.3, 0.25);
		glVertex3f(-0.45, 0.3, 0.2);
	glEnd();

	/* door bar left */
	glPushMatrix();
	glTranslatef(-0.001, 0, 0);

	glBegin(GL_QUADS);
		glNormal3f(-1, 0, 0);
		glVertex3f(-0.45, 0.6, -0.2);
		glVertex3f(-0.45, 0.3, -0.2);
		glVertex3f(-0.45, 0.3, -0.15);
		glVertex3f(-0.45, 0.6, -0.15);
	glEnd();

	glPopMatrix();

	/* front spacing right */
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(1, 0, 0);
		glVertex3f(0.45, 0.6, 0.05);
		glVertex3f(0.45, 0.6, 0.1);
		glVertex3f(0.45, 0.35, 0.25);
		glVertex3f(0.45, 0.3, 0.25);
		glVertex3f(0.45, 0.3, 0.2);
	glEnd();

	/* door bar right */
	glPushMatrix();
	glTranslatef(0.001, 0, 0);

	glBegin(GL_QUADS);
		glNormal3f(1, 0, 0);
		glVertex3f(0.45, 0.6, -0.2);
		glVertex3f(0.45, 0.3, -0.2);
		glVertex3f(0.45, 0.3, -0.15);
		glVertex3f(0.45, 0.6, -0.15);
	glEnd();

	glPopMatrix();
}
