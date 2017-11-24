//
// Created by anikethjr on 24/11/17.
//

#ifndef CLOTH_SIMULATION_CLOTH_H
#define CLOTH_SIMULATION_CLOTH_H

#include "Constraint.h"
#include "Particle.h"

#define PI 3.14159265

using namespace std;
using namespace glm;

class Cloth
{
    double height, width; //height and width of the cloth
    int num_particles_row, num_particles_col; //number of rows and columns of particles respectively
    double distance_row, distance_col, distance_diagonal; //rest distance between adjacent particles in a row, in a column and diagonally respectively
    vector<vector<Particle> > particles;
    vector<Constraint> constraints;

public:
    Cloth(double height, double width, int num_particles_row, int num_particles_col, double mass) {
        this->height = height;
        this->width = width;
        this->num_particles_row = num_particles_row;
        this->num_particles_col = num_particles_col;
        distance_row = width / (double) num_particles_row;
        distance_col = height / (double) num_particles_col;
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
        for (int i = 0; i < num_particles_row; ++i) {
            for (int j = 0; j < num_particles_col; ++j) {
                if (j < num_particles_col - 1)
                    constraints.push_back(Constraint(particles[i][j], particles[i][j + 1], distance_row));
                if (i < num_particles_row - 1)
                    constraints.push_back(Constraint(particles[i][j], particles[i + 1][j], distance_col));
                if (j < num_particles_col - 1 && i < num_particles_row - 1) {
                    constraints.push_back(Constraint(particles[i][j], particles[i + 1][j + 1], distance_diagonal));
                    constraints.push_back(Constraint(particles[i + 1][j], particles[i][j + 1], distance_diagonal));
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


};
#endif //CLOTH_SIMULATION_CLOTH_H
