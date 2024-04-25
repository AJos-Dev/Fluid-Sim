#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include<iostream>

using namespace std;


class droplet {
    public:
        int mass = 1; 
        vector<float> position({23.f, 23.f});
        vector<float> velocity({0.f, 0.f});
};


//"g++ test.cpp -o test -lsfml-graphics -lsfml-window -lsfml-system" to compile in terminal

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    window.setFramerateLimit(60);
    sf::CircleShape shape(10.f);
    shape.setFillColor(sf::Color::Green);

    sf::View view = window.getDefaultView();

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
        shape.move(1.f, 2.f);
        window.draw(shape);
        window.display();
    }

    return 0;
}
