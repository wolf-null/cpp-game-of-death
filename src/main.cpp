#include <iostream>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>

#include "topology/tests/test_node.hpp"
#include "topology/tests/test_grid.hpp"
#include "topology/tests/test_conway.hpp"
#include "logger/tests/test_logger.h"

int tests() {
    using namespace topology;

    tests::test_node();
    conway::tests::test_conway();

    test_LogRecord();
    test_LogHandler();
    test_Logger(false);
    test_LoggerLogLevelHelper(false);

    {
        grid::tests::test_grid(3, 3, grid::GridTopology::RAW).perform_tests();
        grid::tests::test_grid(5, 3, grid::GridTopology::RAW).perform_tests();
        grid::tests::test_grid(3, 5, grid::GridTopology::TORUS).perform_tests();
    }
    return 0;
}

int main()
{
    tests();

    auto window = sf::RenderWindow(sf::VideoMode({800u, 600u}), "GameOfDeath SFML");
    // window.setVerticalSyncEnabled(true);

    sf::Clock clock;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                std::cout << clock.getElapsedTime().asSeconds() << "\n";
                window.close();
            } else if (const sf::Event::KeyPressed * key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scan::Escape) {
                    window.close();
                }
            }
        }

        window.clear();
        window.display();
    }
}
