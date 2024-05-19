//"g++ main_v_2.cpp -o new_sim -lsfml-graphics -lsfml-window -lsfml-system -fopenmp -Ofast" to compile in terminal
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include<iostream>
#include <algorithm>
#include<vector>
#include<cmath>
#include <omp.h>

using namespace std;

const int particle_num = 1600; //1600, 400, 1600
const float particle_mass = 1.f;
const int particle_radius = 5;
const float collision_damping = 0.8f;
const float gravity = 9.81f;
const float dt = 1.f/60.f;

struct particle{
    sf::CircleShape droplet{particle_radius};
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
    float local_density = 0.f;
    float local_pressure = 0.f;
    sf::Vector2f predicted_position{0.f, 0.f};
};

vector<particle> particles(particle_num);

void placeParticles(){
    int particlesPerRow = sqrt(particle_num);
    int particlesPerColumn = (particle_num - 1)/particlesPerRow + 1;
    int spacing = 2.5*particle_radius; 
    for (int i = 0; i < particle_num; i++){
        particles[i].position.x = (i % particlesPerRow + particlesPerRow / 2.5f +0.5f) * spacing;
        particles[i].position.y = (i / particlesPerRow + particlesPerColumn / 2.5f +0.5f) * spacing;
        particles[i].droplet.setFillColor(sf::Color::Cyan); // Remove after resolveColor is developed
    }
}

void resolveCollisions(int i, sf::Vector2u window_size){
    if (particles[i].position.x > window_size.x || particles[i].position.x < 0){
        particles[i].position.x = clamp((int)particles[i].position.x, 0, (int)window_size.x);
        particles[i].velocity.x *= -1 * collision_damping;
    }
    if (particles[i].position.y >= window_size.y || particles[i].position.y <= 0){
        particles[i].position.y = clamp((int)particles[i].position.y, 0, (int)window_size.y);
        particles[i].velocity.y *= -1 * collision_damping;
    }
}

void resolveGravity(int i){
    particles[i].velocity.y += gravity * dt
}

int main()
{
    //Initialize SFML
    sf::RenderWindow window(sf::VideoMode(900, 900), "New Smoothed Particle Hydrodynamics Simulation");
    window.setFramerateLimit(120);
    sf::View view = window.getDefaultView();
    sf::Vector2u window_size = window.getSize();
    placeParticles();
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized){
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }
        sf::Vector2u window_size = window.getSize();
        window.clear();
        //TO-DO: Calculate and cache densities
        
        for (int i = 0; i < particle_num; i++){
            resolveCollisions(i, window_size);
            resolveGravity(i);
            //calculate densities
            //convert density to pressure
            //Calculate pressure forces 
            //Calculate pressure acceleration and add to velocity
            //resolve colour
            //resolveColor(i); add at the end*/
            //set particle positions with radius offset 
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;
            particles[i].droplet.setPosition(particles[i].position.x-particle_radius, particles[i].position.y-particle_radius);
            
            //Draw particles
            window.draw(particles[i].droplet);
        }
        window.display();
    }
    return 0;
}