//"g++ main.cpp -o fluid_sim -lsfml-graphics -lsfml-window -lsfml-system" to compile in terminal
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include<cmath>

using namespace std;

 //Initialize fluid-related

const int particle_num = 1000; 
const int particle_mass = 1;
const int particle_radius = 5;
const float dt = 1.f/60.f;
const float gravity = 9.81; //ms^-2

struct particle{
    sf::CircleShape droplet{particle_radius};
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
};

vector<particle> particles(particle_num);


void place_particles(){
    int particlesPerRow = sqrt(particle_num);
    int particlesPerColumn = (particle_num - 1)/particlesPerRow + 1;
    int spacing = 2.5*particle_radius; 
    for (int i = 0; i < particle_num; i++){
        particles[i].position.x = (i % particlesPerRow + particlesPerRow / 2.f + 0.5f) * spacing;
        particles[i].position.y = (i / particlesPerRow + particlesPerColumn / 2.f + 0.5f) * spacing;
    }
}

//shape.setFillColor(sf::Color::Green);

int main()
{
    //Initialize SFML
    sf::RenderWindow window(sf::VideoMode(750, 750), "Smoothed Particle Hydrodynamics Simulation");
    window.setFramerateLimit(60);
    sf::View view = window.getDefaultView();

    place_particles();

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
        sf::Vector2u size = window.getSize();
        window.clear();
        
        for (int i = 0; i < particle_num; i++){
            //gravity step
            particles[i].velocity.y += gravity * dt;
            particles[i].position.y += particles[i].velocity.y * dt;

            //set particle position with radius offset 
            particles[i].droplet.setPosition(particles[i].position.x-particle_radius, particles[i].position.y-particle_radius);
            //
            window.draw(particles[i].droplet);
        }
        window.display();
    }

    return 0;
}