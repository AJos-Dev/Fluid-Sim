//"g++ main.cpp -o fluid_sim -lsfml-graphics -lsfml-window -lsfml-system -fopenmp -Ofast" to compile in terminal
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include<iostream>
#include <algorithm>
#include<vector>
#include<cmath>
#include <omp.h>

using namespace std;

 //Initialize fluid-related

const int particle_num = 3600; 
const int particle_mass = 1;
const int particle_radius = 5;
const float collision_damping = 0.75f;
double smoothing_radius = 50.f;
const float dt = 1.f/60.f;
const float gravity = 9.81; //ms^-2

struct particle{
    sf::CircleShape droplet{particle_radius};
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
};

vector<particle> particles(particle_num);


void placeParticles(){
    int particlesPerRow = sqrt(particle_num);
    int particlesPerColumn = (particle_num - 1)/particlesPerRow + 1;
    int spacing = 2.5*particle_radius; 
    for (int i = 0; i < particle_num; i++){
        particles[i].position.x = (i % particlesPerRow + particlesPerRow / 5.f + 0.5f) * spacing;
        particles[i].position.y = (i / particlesPerRow + particlesPerColumn / 5.f + 0.5f) * spacing;
    }
}

void resolveGravity(int i){
    particles[i].velocity.y += gravity * dt;
    particles[i].position.y += particles[i].velocity.y * dt;
}

float smoothingKernel(double dst){
    float volume = 3/(2 * smoothing_radius);  
    float value = max((float)0.0, (float)pow(smoothing_radius - dst, 3.0));
    return value/volume;
}

/*
float calculateDensity(sf::Vector2f sample_point){
    float density = 0;
    for (auto &particlet: particles){
        float dst = sqrt(pow((double)(particlet.position.x - sample_point.x), 2.0) + pow((double)(particlet.position.y - sample_point.y), 2.0));
        float influence = smoothingKernel(dst);
        density += particle_mass * influence; 
    }
    return density;
}
*/
float calculateDensity(sf::Vector2f sample_point){
    float density = 0;
    //#pragma omp parallel for num_threads(5) 
    for (int j =0; j < particle_num; j++){
        float dst = sqrt(pow((double)(particles[j].position.x - sample_point.x), 2.0) + pow((double)(particles[j].position.y - sample_point.y), 2.0));
        float influence = smoothingKernel(dst);
        density += particle_mass * influence; 
    }
    return density;
}

void resolveBoundingBox(int i, sf::Vector2u window_size){
    if (particles[i].position.x > window_size.x || particles[i].position.x < 0){
        particles[i].position.x = clamp((int)particles[i].position.x, 0, (int)window_size.x);
        particles[i].velocity.x *= -1 * collision_damping;
    }
    if (particles[i].position.y >= window_size.y || particles[i].position.y <= 0){
        particles[i].position.y = clamp((int)particles[i].position.y, 0, (int)window_size.y);
        particles[i].velocity.y *= -1 * collision_damping;   
    }
}
//shape.setFillColor(sf::Color::Green);

int main()
{
    //Initialize SFML
    sf::RenderWindow window(sf::VideoMode(900, 900), "Smoothed Particle Hydrodynamics Simulation");
    window.setFramerateLimit(120);
    sf::View view = window.getDefaultView();

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
            //gravity step  
            resolveGravity(i);
            // window bounding box
            resolveBoundingBox(i, window_size);
            //calculate densities
            calculateDensity(particles[i].position);
            //set particle position with radius offset 
            particles[i].droplet.setPosition(particles[i].position.x-particle_radius, particles[i].position.y-particle_radius);

            window.draw(particles[i].droplet);
        }
        window.display();
    }

    return 0;
}