//"g++ main.cpp -o fluid_sim -lsfml-graphics -lsfml-window -lsfml-system -fopenmp -Ofast" to compile in terminal
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include<iostream>
#include <algorithm>
#include<vector>
#include<cmath>
#include <omp.h>

using namespace std;

 //Initialize fluid-related

//4th preset is if pressure force is -ve
static int particle_num = 1600; //1600, 400, 1600, 625
const float particle_mass = 1.f;
const int particle_radius = 5;
const float collision_damping = 0.8f;
const float pi = 3.141f;
static float target_density = 0.01f;//0.001f, 0.00005, 0.001, 1.f
static float pressure_multiplier = 200.f;//2500, 2500, 50, 10
static float smoothing_radius = 50.f; //20, 100, 50, 200
const float dt = 1.f/60.f;
static float gravity = 9.81f; //1, 1, 25, 10

//3rd one most realistic due to liquid levelling itself out :)

struct particle{
    sf::CircleShape droplet{particle_radius};
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f velocity{0.f, 0.f};
    float local_density = 1.f;
    float local_pressure = 1.f;
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
/*
void placeParticles(sf::Vector2u window_size){
    for (int i =0; i< particle_num; i++){
        particles[i].position.x = rand() % window_size.x;
        particles[i].position.y = rand() % window_size.y;
        particles[i].droplet.setFillColor(sf::Color::Cyan);
    }
}*/

void resolveGravity(int i){
    particles[i].velocity.y += gravity * dt;
}

void predictPositions(int i){
    particles[i].predicted_position.x = particles[i].position.x + particles[i].velocity.x * dt;
    particles[i].predicted_position.y = particles[i].position.y + particles[i].velocity.y * dt;

}

float smoothingKernel(double dst){
    float volume = pi * (float)(pow(smoothing_radius, 5.0)/10);  
    float value = max((float)0.0, (float)pow(smoothing_radius - dst, 3.0));
    return value/volume;
}

float smoothingKernelDerivative(double dst){
    if (dst >= smoothing_radius) return 0.0;
    return ( 100 *(float) -30/(pi * pow(smoothing_radius, 5)) * pow(smoothing_radius - dst, 2));// 100.0f * was here before
}
//Test other smoothing kernels below
/////////////////////////////////////////////////////////
float calculateDensity(int i){
    float density = 0.f;
    //#pragma omp parallel for num_threads(9) 
    for (int j =0; j < particle_num; j++){
       // float dst = sqrt(pow((double)(particles[j].position.x - sample_point.x), 2.0) + pow((double)(particles[j].position.y - sample_point.y), 2.0));
        float dst = sqrt((particles[j].predicted_position.x - particles[i].predicted_position.x) * (particles[j].predicted_position.x - particles[i].predicted_position.x)  + (particles[j].predicted_position.y - particles[i].predicted_position.y) * (particles[j].predicted_position.y - particles[i].predicted_position.y));
        float influence = smoothingKernel(dst);
        density += particle_mass * influence; 
    }
    return density;
}

float densityToPressure(int j){
    float density_error = particles[j].local_density - target_density;
    float local_pressure = density_error * pressure_multiplier;
    return local_pressure;
}

float sharedPressure(int i, int j){
    float pressurei = densityToPressure(particles[i].local_density);
    float pressurej = densityToPressure(particles[j].local_density);
    return -((pressurei + pressurej) / 2.f);
}

sf::Vector2f calculatePressureForce(int i){
    sf::Vector2f pressure_force;
    //#pragma omp parallel for num_threads(9)
    for (int j = 0; j<particle_num; j++){
        if (i == j) continue;
        float x_offset;
        float y_offset;
        if (particles[i].predicted_position == particles[j].predicted_position){
            x_offset = (float)1+ (rand() % 20);
            y_offset = (float)1+ (rand() % 20);
        }
        else{
            x_offset = particles[j].predicted_position.x - particles[i].predicted_position.x;
            y_offset = particles[j].predicted_position.y - particles[i].predicted_position.y; 
        }
        double dst = sqrt(abs(x_offset * x_offset + y_offset * y_offset));
        float gradient = smoothingKernelDerivative(dst);
        float x_dir = x_offset/dst;
        float y_dir = y_offset/dst;
        float shared_pressure = sharedPressure(i, j); 
        //pressure_force.x += densityToPressure(j) * gradient * particle_mass/particles[j].local_density * x_dir;
        //pressure_force.y += densityToPressure(j) * gradient * particle_mass/particles[j].local_density * y_dir;// Newton's 3rd law implementation below
        pressure_force.x += shared_pressure * gradient * particle_mass/particles[j].local_density * x_dir;
        pressure_force.y += shared_pressure * gradient * particle_mass/particles[j].local_density * y_dir;
    }
    return pressure_force * 2.f;
}
/*
void resolveBoundingBox(int i, sf::Vector2u window_size){
    if (particles[i].position.x > window_size.x || particles[i].position.x < 0){
        particles[i].position.x = clamp((int)particles[i].position.x, 0, (int)window_size.x);
        particles[i].velocity.x *= -1 * collision_damping;
    }
    if (particles[i].position.y >= window_size.y || particles[i].position.y <= 0){
        particles[i].position.y = clamp((int)particles[i].position.y, 0, (int)window_size.y);
        particles[i].velocity.y *= -1 * collision_damping;   
    }
}*/

void resolveCollisions(int i, sf::Vector2u window_size){
    if (particles[i].position.x > window_size.x || particles[i].position.x < 0){
        particles[i].position.x = clamp((int)particles[i].position.x, 0, (int)window_size.x);
        particles[i].velocity.x *= -1;
    }
    if (particles[i].position.y >= window_size.y || particles[i].position.y <= 0){
        particles[i].position.y = clamp((int)particles[i].position.y, 0, (int)window_size.y);
        particles[i].velocity.y *= -1;
    }
    /*
    for (int k = 0; k < particle_num; k++){
        if (i == k) continue;
        float x_offset = particles[i].position.x - particles[k].position.x;
        float y_offset = particles[i].position.y - particles[k].position.y;
        float total_offset = x_offset * x_offset + y_offset * y_offset;
        if(total_offset < 2*particle_radius){
            float correction = 2*particle_radius - total_offset;
            particles[i].position.x += sqrt(correction / 2);
            particles[i].position.y -= sqrt(correction / 2);
        }
    }*/
}

/*
TO-DO!
void resolveColor(int i){
    if (abs(particles[i].velocity.x + particles[i].velocity.y) > 50){
        float offset = particles[i].velocity.x + particles[i].velocity.y - 50;
        particles[i].droplet.setFillColor(sf::Color(2*offset , 2*offset, 250-2*offset));
    }
    else{
        float offset = 50 - abs(particles[i].velocity.x) - abs(particles[i].velocity.y);
        particles[i].droplet.setFillColor(sf::Color(2*offset, 2*offset, 250-2*offset));
    }   
}*/

int main()
{
    //Initialize SFML
    sf::RenderWindow window(sf::VideoMode(900, 900), "Smoothed Particle Hydrodynamics Simulation");
    window.setFramerateLimit(120);
    sf::View view = window.getDefaultView();
    sf::Vector2u window_size = window.getSize();
    //placeParticles(window_size);
    placeParticles();
    //cout << calculateDensity(985) << "\n"; //show that this makes the density roughly constant
    ImGui::SFML::Init(window);


    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized){
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }
        sf::Vector2u window_size = window.getSize();
        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear();
        //TO-DO: Calculate and cache densities

        ImGui::Begin("Hello");
        ImGui::SliderFloat("Gravity", &gravity, 0.f, 100.f);
        ImGui::SliderFloat("Pressure Multiplier", &pressure_multiplier, 0.f, 1000.f);
        ImGui::SliderFloat("Target Density", &target_density, 0.f, 1.f);
        ImGui::SliderInt("Particle Num", &particle_num, 1, 1600);
        ImGui::SliderFloat("Smoothing Radius", &smoothing_radius, 10, 200);
        ImGui::End();
        
        for (int i = 0; i < particle_num; i++){
            resolveCollisions(i, window_size);
            //gravity step  
            resolveGravity(i);
            //Predict next positions
            predictPositions(i);
            // window bounding box
            //resolveBoundingBox(i, window_size);
            //calculate densities
            particles[i].local_density = calculateDensity(i);
            //convert density to pressure
            particles[i].local_pressure = densityToPressure(i);
            //Calculate pressure forces 
            sf::Vector2f pressure_force = calculatePressureForce(i);
            sf::Vector2f pressure_acceleration; 
            pressure_acceleration.x = pressure_force.x/particles[i].local_density;
            pressure_acceleration.y = pressure_force.y/particles[i].local_density;
            //cout << particles[i].position.x <<", " << particles[i].position.y << ", " << i << "\n";
            particles[i].velocity.x += pressure_acceleration.x * dt;
            particles[i].velocity.y += pressure_acceleration.y * dt;
            //resolve colour
            //resolveColor(i); add at the end*/
            //set particle positions with radius offset 
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;
            particles[i].droplet.setPosition(particles[i].position.x-particle_radius, particles[i].position.y-particle_radius);
            //cout<<particles[i].local_density<<"\n";
            
            window.draw(particles[i].droplet);
        }
        ImGui::SFML::Render(window);

        window.display();
    }
     ImGui::SFML::Shutdown();

    return 0;
}
