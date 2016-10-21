#include "solarsystem.h"
#include <iostream>
#include <cmath>
using namespace std;

SolarSystem::SolarSystem() :
    m_kineticEnergy(0),
    m_potentialEnergy(0)
{
}

CelestialBody& SolarSystem::createCelestialBody(vec3 position, vec3 velocity, double mass) {
    m_bodies.push_back( CelestialBody(position, velocity, mass) );
    return m_bodies.back(); // Return reference to the newest added celestial body
}

void SolarSystem::calculateForcesAndEnergy()
{
    m_kineticEnergy = 0;
    m_potentialEnergy = 0;
    m_angularMomentum.zeros();

    for(CelestialBody &body : m_bodies) {
        // Reset forces on all bodies
        body.force.zeros();
    }

    for(int i=0; i<numberOfBodies(); i++) {
        CelestialBody &body1 = m_bodies[i];
        for(int j=i+1; j<numberOfBodies(); j++) {
            CelestialBody &body2 = m_bodies[j];
            vec3 deltaRVector = body1.position - body2.position;
            double dr = deltaRVector.length();
            // Calculate the force and potential energy here

            vec3 force = -4*pow(3.141592654,2) * body1.mass * body2.mass / pow(dr,3) * deltaRVector;
            // see if 4pi^2 factor can be put outside to reduce flops + check minus sign
            // check directionality of deltaRVector, among other things
            body1.force += force;
            body2.force -= force; // this sign is correct, otherwise cannot get circular orbit in ES

            // Potential energy, check minus sign again
            m_potentialEnergy += -4*pow(3.141592654,2) * body1.mass * body2.mass / dr;
        }

        m_kineticEnergy += 0.5*body1.mass*body1.velocity.lengthSquared();
    }
}

void SolarSystem::calculateForcesAndEnergyREL()
{
    m_kineticEnergy = 0;
    m_potentialEnergy = 0;
    m_angularMomentum.zeros();

    double c = 63239.7263; // AU/year

    for(CelestialBody &body : m_bodies) {
        // Reset forces on all bodies
        body.force.zeros();
    }

    for(int i=0; i<numberOfBodies(); i++) {
        CelestialBody &body1 = m_bodies[i];
        for(int j=i+1; j<numberOfBodies(); j++) {
            CelestialBody &body2 = m_bodies[j];
            vec3 deltaRVector = body1.position - body2.position;
            double dr = deltaRVector.length();
            double l = (body2.position.cross(body2.velocity)).length();
            // Calculate the force and potential energy here

            vec3 force = -4*pow(3.141592654,2) * body1.mass * body2.mass / pow(dr,3) * (1 + (3*pow(l, 2))/(pow(dr,2)*pow(c, 2))) * deltaRVector;
            // see if 4pi^2 factor can be put outside to reduce flops + check minus sign
            // check directionality of deltaRVector, among other things
            body1.force += force;
            body2.force -= force; // this sign is correct, otherwise cannot get circular orbit in ES

            // Potential energy, check minus sign again
            m_potentialEnergy += -4*pow(3.141592654,2) * body1.mass * body2.mass / dr;
        }

        m_kineticEnergy += 0.5*body1.mass*body1.velocity.lengthSquared();
    }
}

int SolarSystem::numberOfBodies() const
{
    return m_bodies.size();
}

double SolarSystem::totalEnergy() const
{
    return m_kineticEnergy + m_potentialEnergy;
}

double SolarSystem::potentialEnergy() const
{
    return m_potentialEnergy;
}

double SolarSystem::kineticEnergy() const
{
    return m_kineticEnergy;
}

void SolarSystem::writeToFile(string filename)
{
    if(!m_file.good()) {
        m_file.open(filename.c_str(), ofstream::out);
        if(!m_file.good()) {
            cout << "Error opening file " << filename << ". Aborting!" << endl;
            terminate();
        }
    }

    m_file << numberOfBodies() << endl;
    m_file << "Comment line that needs to be here. Balle." << endl;
    for(CelestialBody &body : m_bodies) {
        m_file << "1 " << body.position.x() << " " << body.position.y() << " " << body.position.z() << "\n";
    }
}

vec3 SolarSystem::angularMomentum() const
{
    return m_angularMomentum;
}

std::vector<CelestialBody> &SolarSystem::bodies()
{
    return m_bodies;
}
