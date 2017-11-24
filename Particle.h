//
// Created by anikethjr on 24/11/17.
//


#include <bits/stdc++.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


#ifndef CLOTH_SIMULATION_PARTICLE_H
#define CLOTH_SIMULATION_PARTICLE_H

using namespace std;
using namespace glm;

/**
 * Class which is used to define the particles of the cloth
 */
class Particle
{
private:
    bool is_movable; // to check whether the particle is movable or not
    double mass; // defines the mass of the particle (default = 1)
    dvec3 current_pos; // defines the current position of the particle
    dvec3 old_pos; // defines the old position of the particle
    dvec3 acceleration; // defines the acceleration of the particle

public:
    Particle(dvec3 current_pos, double mass)
    {
        is_movable = true;
        this->mass = mass;
        this->current_pos = current_pos;
        this->old_pos = current_pos;
        acceleration = dvec3(0, 0, 0);
    }

    void applyForce(dvec3 force)
    {
        acceleration += force / mass;
    }

    dvec3 getCurrentPos()
    {
        return current_pos;
    }



};
#endif //CLOTH_SIMULATION_PARTICLE_H
