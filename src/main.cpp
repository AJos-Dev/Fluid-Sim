#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include<iostream>

using namespace std;

//"g++ test.cpp -o test -lsfml-graphics -lsfml-window -lsfml-system" to compile in terminal

class droplet {
    public:
        int mass = 1;
        sf::CircleShape shape{10.f};
        sf::Vector2f position;
        sf::Vector2f velocity{0, 0};
        droplet(){
        shape.setFillColor(sf::Color::Green);
        }
        
};

int main()
{
    //Initialize SFML
    sf::RenderWindow window(sf::VideoMode(500, 750), "SFML works!");
    window.setFramerateLimit(60);
    sf::View view = window.getDefaultView();

    //Initialize fluid-related
    //const int droplet_num; //add eventually
    const float dt = 1.f/60.f;
    const float gravity = 9.81; //ms^-2
    droplet sample;


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
        sample.velocity.y += gravity * dt;
        sample.shape.move(sample.velocity.x * dt, sample.velocity.y);
        window.draw(sample.shape);
        
        
        window.display();
    }

    return 0;
}
