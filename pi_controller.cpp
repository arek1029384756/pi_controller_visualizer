#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <random>

using DotList = std::list<std::int32_t>;

constexpr std::size_t gameWidth = 1500;
constexpr std::size_t gameHeight = 1000;

void addDot(DotList& list, std::int32_t yPos) {
    list.emplace_front(yPos);
    if(list.size() >= gameWidth / 2) {
        list.pop_back();
    }
}

void drawLines(sf::RenderWindow& window, const DotList& list, sf::Color color) {
    auto getY = [](const auto v) { return gameHeight - v * 8 / 10 - 100; };

    for(auto it = list.begin(); it != list.end(); std::advance(it, 1)) {
        auto itn = std::next(it, 1);
        if(itn != list.end()) {
            auto i = std::distance(list.begin(), it);
            auto j = std::distance(list.begin(), itn);
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(gameWidth / 2 - i, getY(*it)), color),
                sf::Vertex(sf::Vector2f(gameWidth / 2 - j, getY(*itn)), color)
            };

            window.draw(line, 2, sf::Lines);
        }
    }
}

std::int32_t process(double control) {
    // Example transfer function implementing process
    return static_cast<std::int32_t>(control * 1.47);
}

auto piUpdate(std::int32_t setVal) {
    static double controlSignal = 0;
    static double errIntegral = 0;

    double err = setVal - process(controlSignal);
    errIntegral += err;

    controlSignal += err * 0.4f;
    controlSignal += errIntegral * 0.005f;

    //controlSignal += err * 0.06f;
    //controlSignal += errIntegral * 0.1f;

    return process(controlSignal);
}

std::int32_t randomGen(std::int32_t rndMin, std::int32_t rndMax) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(rndMin, rndMax);
    return dis(gen);
}

void handleEvents(sf::RenderWindow& window, std::int32_t& setValue) {
    sf::Event event;
    while(window.pollEvent(event)) {
        // Window closed or escape key pressed: exit
        if((event.type == sf::Event::Closed) ||
                ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
            window.close();
            break;
        }

        if(event.type == sf::Event::KeyPressed) {
            if(event.key.code == sf::Keyboard::Num0) {
                setValue = 0;
            } else if(event.key.code == sf::Keyboard::Num1) {
                setValue = 1000;
            } else if(event.key.code == sf::Keyboard::Num5) {
                setValue = 500;
            }
        }

        // Window size changed, adjust view appropriately
        if (event.type == sf::Event::Resized) {
            sf::View view;
            view.setSize(gameWidth, gameHeight);
            view.setCenter(gameWidth / 2.f, gameHeight / 2.f);
            window.setView(view);
        }
    }
}

auto getNoise() {
    static std::int32_t noise = 0;
    static std::size_t noisePeriod = 1;
    static std::size_t ticks = 0;

    if(++ticks >= noisePeriod) {
        ticks = 0;
        noise = randomGen(-20, 20);
        //noise = -noise;

        static std::size_t cnt = 0;
        if(++cnt >= 10) {
            cnt = 0;
            ++noisePeriod;
        }
    }

    return noise;
}

void updateValues(DotList& setList, DotList& piList, std::int32_t setValue) {
    auto noise = getNoise();
    addDot(setList, setValue + noise);
    addDot(piList, piUpdate(setValue + noise));
}

void updateWindow(sf::RenderWindow& window, const DotList& setList, const DotList& piList) {
    window.clear(sf::Color(150, 150, 150));

    drawLines(window, piList, sf::Color(191, 0, 0));
    drawLines(window, setList, sf::Color::Black);

    window.display();
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 0;

    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "PI Controller Simulator",
            sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    std::int32_t setValue = 0;

    DotList setList;
    DotList piList;

    const sf::Time updateTime = sf::milliseconds(10);
    sf::Clock clock;

    while(window.isOpen()) {
        handleEvents(window, setValue);

        if(clock.getElapsedTime() >= updateTime) {
            clock.restart();
            updateValues(setList, piList, setValue);
        }

        updateWindow(window, setList, piList);
    }

    return EXIT_SUCCESS;
}
