//
// Created by anikethjr on 24/11/17.
//

#ifndef CLOTH_SIMULATION_CONSTRAINT_H
#define CLOTH_SIMULATION_CONSTRAINT_H

#include "Particle.h"

class Constraint
{
    double rest_length;
    pair<Particle*,Particle*> particles;
public:
    Constraint(Particle &first_particle, Particle &second_particle,double distance)
    {
        rest_length = distance;
        particles.first = &first_particle;
        particles.second = &second_particle;
    }

    void correctParticlePositions()
    {
        //calculate the compensations to be made to bring back the particles to their rest positions
        dvec3 current_displacement = particles.second->getCurrentPos() - particles.first->getCurrentPos();
        dvec3 correction_first_particle = normalize(current_displacement)*((1-rest_length/current_displacement.length())/2);
        dvec3 correction_second_particle = -correction_first_particle;

        //update the positions of the particles
        particles.first->updatePosition(correction_first_particle);
        particles.second->updatePosition(correction_second_particle);
    }
};
#endif //CLOTH_SIMULATION_CONSTRAINT_H
