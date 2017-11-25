//
// Created by anikethjr on 24/11/17.
//

#ifndef CLOTH_SIMULATION_CLOTH_H
#define CLOTH_SIMULATION_CLOTH_H

#include "Constraint.h"
#include "Particle.h"

#define PI 3.14159265
#define CONSTRAINT_ITERATIONS 15 // refers to the number of iterations required to satisfy the constraints per frame

using namespace std;
using namespace glm;

/**
 * structure to store color values
 */
struct Color {
    double r, g, b, a;
};

/**
 * Defines the cloth piece
 */
class Cloth {
    double height, width; //height and width of the cloth
    unsigned long num_particles_row, num_particles_col; //number of rows and columns of particles respectively
    double distance_row, distance_col, distance_diagonal; //rest distance between adjacent particles in a row, in a column and diagonally respectively
    vector<vector<Particle> > particles;
    vector<vector<Particle *> > triangles;
    vector<Constraint> constraints;

public:
    /**
     * Constructor to initialize the cloth
     * @param height height of the cloth
     * @param width width of the cloth
     * @param num_particles_row number of rows of particles
     * @param num_particles_col number of columns of particles
     * @param mass mass of each particle
     */
    Cloth(double height, double width, unsigned long num_particles_row, unsigned long num_particles_col, double mass) {
        this->height = height;
        this->width = width;
        this->num_particles_row = num_particles_row;
        this->num_particles_col = num_particles_col;
        distance_row = width / (double) num_particles_col;
        distance_col = height / (double) num_particles_row;
        distance_diagonal = sqrt(distance_row * distance_row + distance_col * distance_col);

        //add particles to the cloth according to the specifications
        particles.resize(num_particles_row);
        for (int i = 0; i < num_particles_row; ++i) {
            for (int j = 0; j < num_particles_col; ++j) {
                particles[i].push_back(Particle(dvec3((double) i * distance_row, (double) j * distance_col, 0), mass));
            }
        }

        //add structural constraints between adjacent particles,
        //shear constraints between diagonal particles and
        //bending constraints between a particle and the particle
        //at a distance of 2 along each row, column and diagonal
        //Also, create triangles for drawing and adding wind
        for (int i = 0; i < num_particles_row; ++i) {
            for (int j = 0; j < num_particles_col; ++j) {
                if (j < num_particles_col - 1)
                    constraints.push_back(Constraint(particles[i][j], particles[i][j + 1], distance_row));
                if (i < num_particles_row - 1)
                    constraints.push_back(Constraint(particles[i][j], particles[i + 1][j], distance_col));
                if (j < num_particles_col - 1 && i < num_particles_row - 1) {
                    constraints.push_back(Constraint(particles[i][j], particles[i + 1][j + 1], distance_diagonal));
                    constraints.push_back(Constraint(particles[i + 1][j], particles[i][j + 1], distance_diagonal));

                    //creating triangles
                    vector<Particle *> tri1, tri2;
                    tri1.push_back(&particles[i][j]);
                    tri1.push_back(&particles[i + 1][j]);
                    tri1.push_back(&particles[i + 1][j + 1]);

                    tri2.push_back(&particles[i][j]);
                    tri2.push_back(&particles[i][j + 1]);
                    tri2.push_back(&particles[i + 1][j + 1]);

                    triangles.push_back(tri1);
                    triangles.push_back(tri2);
                }
                if (j < num_particles_col - 2)
                    constraints.push_back(Constraint(particles[i][j], particles[i][j + 2], distance_row * 2));
                if (i < num_particles_row - 2)
                    constraints.push_back(Constraint(particles[i][j], particles[i + 2][j], distance_col * 2));
                if (j < num_particles_col - 2 && i < num_particles_row - 2) {
                    constraints.push_back(Constraint(particles[i][j], particles[i + 2][j + 2], distance_diagonal * 2));
                    constraints.push_back(Constraint(particles[i + 2][j], particles[i][j + 2], distance_diagonal * 2));
                }
            }
        }
    }

    /**
     * Makes the particle at index i,j in the grid immovable. Used to hang the cloth.
     * @param i Row number of the particle
     * @param j Column number of the particle
     */
    void makeParticleImmovable(int i, int j) {
        particles[i][j].makeImmovable();
    }

    /**
     * Draws the cloth by dividing it into a set of triangles
     */
    void draw(Color color) {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < triangles.size(); ++i) {
            glColor3d(color.r, color.g, color.b);
            for (int j = 0; j < triangles[i].size(); ++j) {
                glVertex3d(triangles[i][j]->getCurrentPos().x, triangles[i][j]->getCurrentPos().y,
                           triangles[i][j]->getCurrentPos().z);
            }
        }
        glEnd();
    }

    /**
     * Funtion to simulate the cloth by constraint satisfaction and subsequent update of particles
     */
    void simulateCloth() {
        // Satisfying the constraints

        for (int i = 0; i < CONSTRAINT_ITERATIONS; i++) // iterating over the constraints multiple times
        {
            for (auto constraint: constraints) {
                constraint.correctParticlePositions(); // correct each particle pair position (constraint satisfaction)
            }
        }

        // Now updating the positions of the particles
        for (int i = 0; i < particles.size(); i++) {
            for (auto particle: particles[i]) {
                particle.timeStep();
            }
        }

    }

    /**
     * Function to uniformly add gravity (or other force vectors) to all particles
     * @param force_direction refers to the direction of the force vector
     */
    void applyUniformForceAll(dvec3 force_direction) {
        for (int i = 0; i < particles.size(); i++) {
            for (auto particle: particles[i]) {
                particle.applyForce(force_direction); // apply the force to each particle
            }
        }
    }

    /**
     * Function to apply a force (eg. wind force) which acts on all the particles in the direction
     * of the normal to the triangle of which the particle is a vertex
     * @param force_direction refers to the vector containing the wind force attributes (direction and magnitude)
     */
    void applyTriangleNormalForce(dvec3 force_direction) {
        for (int i = 0; i < triangles.size(); ++i) {
            dvec3 normal_to_triangle = triangleNormal(triangles[i][0]->getCurrentPos(),
                                                      triangles[i][1]->getCurrentPos(),
                                                      triangles[i][2]->getCurrentPos());
            normal_to_triangle = normalize(normal_to_triangle);
            double force_magnitude = dot(force_direction, normal_to_triangle);
            dvec3 force = normal_to_triangle * force_magnitude;
            for (int j = 0; j < triangles[i].size(); ++j) {
                triangles[i][j]->applyForce(force);
            }
        }
    }

    /**
     * In case of collision of cloth particles with sphere,
     * add a velocity along the vector
     * from the centre of the sphere to the point of collision.
     * Its magnitude is equal to the difference in length between
     * radius and distance from centre of sphere to particle.
     * @param pos centre of the sphere
     * @param radius radius of the sphere
     */
    void resolveSphereCollision(dvec3 pos, double radius) {
        for (int i = 0; i < particles.size(); ++i) {
            for (int j = 0; j < particles[i].size(); ++j) {
                if (radius < (particles[i][j].getCurrentPos() - pos).length()) {
                    dvec3 update = normalize(particles[i][j].getCurrentPos() - pos);
                    update = update * ((particles[i][j].getCurrentPos() - pos).length() - radius);
                    particles[i][j].updatePosition(update);
                }
            }
        }
    }
};


#endif //CLOTH_SIMULATION_CLOTH_H
