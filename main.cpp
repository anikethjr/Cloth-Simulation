//
// Created by anikethjr on 24/11/17.
//

#include "Cloth.h"

using namespace std;
using namespace glm;

dvec3 ball1Position(7, -5, 0); // the center of the first ball
dvec3 ball2Position(4, -5, 2); // the center of the second ball
double ballRadius = 2; // the radius of the ball
double ballZ = 0; // counter for used to calculate the z coordinates of the ball
double ballX = 0;
double cloth_width = 14;
double cloth_height = 10;
unsigned long cloth_nrow = 55;
unsigned long cloth_ncol = 45;
double cloth_mass = 1;
Color ballColor = {0.5, 0.6, 0.1};

Color clothColorPrimary = {0.9, 0.1, 0.1};
Color clothColorSecondary = {0.1, 0.1, 0.1};
dvec3 clothPosition(0, -2, 0);

Cloth cloth1(clothPosition, cloth_height, cloth_width, cloth_ncol, cloth_nrow, cloth_mass);
dvec3 cameraPosition = dvec3(-6.5, 6, -9.0);
double roll_angle = 0, pitch_angle = 25, yaw_angle = 0;

/**
 * Function which assembles the various objects and creates the scene with the ball
 */
void renderSceneBall() {
    // calculating positions
    ballZ++;
    ball1Position.z = cos(ballZ / 50.0) * 7;
    ballX++;
    ball2Position.x = cos(ballX / 50.0) * 7;


    cloth1.applyUniformForceAll(dvec3(0, -0.2, 0) * pow(TIME_STEP, 2)); // add gravity
    cloth1.applyTriangleNormalForce(dvec3(0.001, 0, 0.01) * pow(TIME_STEP, 2)); // add wind
    cloth1.simulateCloth(); // calculate the particle positions
    cloth1.resolveSphereCollision(ball1Position, ballRadius);
    cloth1.resolveSphereCollision(ball2Position, ballRadius);

    //draw cloth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glDisable(GL_LIGHTING); // drawing some smooth shaded background
    glBegin(GL_POLYGON);
    glColor3d(0.8f, 0.8f, 1.0f);
    glVertex3d(-200.0f, -100.0f, -100.0f);
    glVertex3d(200.0f, -100.0f, -100.0f);
    glColor3d(0.4f, 0.4f, 0.8f);
    glVertex3d(200.0f, 100.0f, -100.0f);
    glVertex3d(-200.0f, 100.0f, -100.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    //place camera
    glTranslated(cameraPosition.x, cameraPosition.y, cameraPosition.z);

    //rotate it based on the user input
    glRotated(roll_angle, 0, 0, 1);
    glRotated(pitch_angle, 0, 1, 0);
    glRotated(yaw_angle, 1, 0, 0);

    //draw cloth
    cloth1.draw(clothColorPrimary, clothColorSecondary);

    glPushMatrix();
    glTranslated(ball1Position.x, ball1Position.y,
                 ball1Position.z);
    glColor3d(ballColor.r, ballColor.b, ballColor.g);
    glutSolidSphere(ballRadius - 0.1, 64, 64); // draw the sphere. radius reduced a bit to avoid mminute collisions
    glPopMatrix();

    glPushMatrix();
    glTranslated(ball2Position.x, ball2Position.y,
                 ball2Position.z);
    glColor3d(ballColor.r, ballColor.b, ballColor.g);
    glutSolidSphere(ballRadius - 0.1, 64, 64); // draw the sphere. radius reduced a bit to avoid mminute collisions
    glPopMatrix();


    glutSwapBuffers();
    glutPostRedisplay();
}

/**
 * Handles key presses
 * @param key the keyboard input given by the user
 * @param x x coordinate of the input
 * @param y y coordinate of the input
 */
void keyPress(unsigned char key, int x, int y) {
    switch (key) {
        //Move forward
        case 'w':
            cameraPosition += dvec3(0, 0, 0.2);
            break;
            //Move back
        case 's':
            cameraPosition += dvec3(0, 0, -0.2);
            break;
            //Move left
        case 'a':
            cameraPosition += dvec3(0.2, 0, 0);
            break;
            //Move right
        case 'd':
            cameraPosition += dvec3(-0.2, 0, 0);
            break;
            //Move up
        case 'r':
            cameraPosition += dvec3(0, -0.2, 0);
            break;
            //Move down
        case 'f':
            cameraPosition += dvec3(0, 0.2, 0);
            break;
            //Look up
        case 'i':
            yaw_angle += 1;
            break;
            //Look down
        case 'k':
            yaw_angle -= 1;
            break;
            //Look left
        case 'l':
            pitch_angle -= 1;
            break;
            //Look right
        case 'j':
            pitch_angle += 1;
            break;
            //Roll anticlockwise
        case 'z':
            roll_angle -= 1;
            break;
            //Roll clockwise
        case 'x':
            roll_angle += 1;
            break;
        default:
            return;
    }
    glutPostRedisplay();
}

/**
 * Adds lighting to the scene
 */
void light() {
    glShadeModel(GL_SMOOTH);
    glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_COLOR_MATERIAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[4] = {-1.0, 1.0, 0.5, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *) &lightPos);

    glEnable(GL_LIGHT1);

    GLfloat lightAmbient1[4] = {0.0, 0.0, 0.0, 0.0};
    GLfloat lightPos1[4] = {1.0, 0.0, -0.2, 0.0};
    GLfloat lightDiffuse1[4] = {0.5, 0.5, 0.3, 0.0};

    glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat *) &lightPos1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat *) &lightAmbient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat *) &lightDiffuse1);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

/**
 * Function to handle window resizing
 * @param w width of new window
 * @param h height of new window
 */
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (h == 0)
        gluPerspective(80, (float) w, 1.0, 5000.0);
    else
        gluPerspective(80, (float) w / (float) h, 1.0, 5000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main()
{
    // init GLUT and create Window
    int x = 0;
    glutInit(&x, nullptr);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Cloth Simulation");

    // register callbacks
    glutReshapeFunc(reshape);
    glutDisplayFunc(renderSceneBall);
    glutKeyboardFunc(keyPress);

    //light the scene
    light();

    //anchor cloth
    for (int i = 0; i < cloth_nrow; ++i) {
        cloth1.makeParticleImmovable(0, i);
        cloth1.makeParticleImmovable(0, cloth_nrow - i - 1);
    }

    // enter GLUT event processing cycle
    glutMainLoop();
}

