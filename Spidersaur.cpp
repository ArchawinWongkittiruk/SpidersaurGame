#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

const int windowWidth = 1280;
const int windowHeight = 720;

Sprite dinoSprite;
Sprite groundSprite;
Sprite skySprite1;
Sprite skySprite2;
Sprite cactuses[5];
Sprite bricks[10];
FloatRect skySpriteRect;

const int x = 100;
int y;
float dy;
int lastCactus;
int lastBrick;
bool gameOver;

void startGame() {
    dinoSprite.setPosition(x, windowHeight - 200);
    groundSprite.setPosition(0, windowHeight - 100);
    skySprite1.setPosition(0, 0);
    skySprite2.setPosition(skySpriteRect.width, 0);
    for (int i = 0; i < 5; i++) {
        cactuses[i].setPosition(Vector2f(-1000, -1000));
    }
    for (int i = 0; i < 10; i++) {
        bricks[i].setPosition(Vector2f(-1000, -1000));
    }

    y = 100;
    dy = 0;
    lastCactus = 0;
    lastBrick = 0;
    gameOver = false;
}

int main() {
    // create the window
    RenderWindow app(VideoMode(windowWidth, windowHeight), "\"Spidersaur: A Game\" by Archawin Wongkittiruk");
    app.setFramerateLimit(100);

    Font font;
    font.loadFromFile("fonts/consola.ttf");

    Text highScore;
    highScore.setFont(font);
    highScore.setCharacterSize(50);
    highScore.setPosition(50, 0);
    Text score;
    score.setFont(font);
    score.setCharacterSize(50);
    score.setPosition(50, 50);

    Texture dinoTexture1; dinoTexture1.loadFromFile("images/dinosprite1.png");
    Texture dinoTexture2; dinoTexture2.loadFromFile("images/dinosprite2.png");
    Texture dinoTexture3; dinoTexture3.loadFromFile("images/dinosprite3.png");
    Texture groundTexture; groundTexture.loadFromFile("images/ground.png");
    Texture skyTexture; skyTexture.loadFromFile("images/sky.jpg");
    Texture cactusTexture; cactusTexture.loadFromFile("images/cactus.png");
    Texture brickTexture; brickTexture.loadFromFile("images/brick.png");

    dinoSprite.setTexture(dinoTexture1);
    int runTexture = 1;
    groundSprite.setTexture(groundTexture);
    skySprite1.setTexture(skyTexture);
    skySprite2.setTexture(skyTexture);
    skySpriteRect = skySprite1.getGlobalBounds();
    for (int i = 0; i < 5; i++) {
        cactuses[i] = Sprite(cactusTexture);
    }
    for (int i = 0; i < 10; i++) {
        bricks[i] = Sprite(brickTexture);
    }

    VertexArray line(LineStrip, 2);
    bool shootLine = false;

    startGame();
    Clock clock;
    int highScoreInt = 0;
    
    // run the program as long as the window is open
    while (app.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (app.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == Event::Closed) app.close();
        }

        if (!gameOver) {
            // get and update scores, and increase the game speed
            int time = (int) clock.getElapsedTime().asSeconds();
            if (time > highScoreInt) {
                highScoreInt = time;
            }
            highScore.setString("High score: " + std::to_string(highScoreInt));
            score.setString("Score: " + std::to_string(time));
            float speed = -5.f - clock.getElapsedTime().asSeconds() / 10;

            // jump when pressing space and slow descent when holding
            if (Keyboard::isKeyPressed(Keyboard::Space)) {
                if (y > windowHeight - 200) {
                    dy = -12;
                }
                if (dy > 0) {
                    dy -= 0.3;
                }
            }

            // spawn next cactus randomly
            if (rand() % 75 == 0 && cactuses[lastCactus-1].getPosition().x < windowWidth / 2) {
                cactuses[lastCactus].setPosition(windowWidth, windowHeight - 220);
                lastCactus = (lastCactus + 1) % 5;
            }

            // spawn next brick randomly
            if (rand() % 50 == 0) {
                bricks[lastBrick].setPosition(windowWidth, 150);
                lastBrick = (lastBrick + 1) % 10;
            }

            // gravity
            dy += 0.35;
            y += dy;
            if (y > windowHeight - 200) {
                dy = 0;
            }

            dinoSprite.setPosition(x, y);

            // check for collision with cactuses
            for (int i = 0; i < 5; i++) {
                if (dinoSprite.getGlobalBounds().intersects(cactuses[i].getGlobalBounds())) {
                    gameOver = true;
                }
            }

            // move the sky background
            skySprite1.move(Vector2f(speed/2, 0.f));
            skySprite2.move(Vector2f(speed/2, 0.f));
            if (skySprite1.getPosition().x < -skySpriteRect.width) {
                skySprite1.setPosition(skySpriteRect.width, 0);
            }
            if (skySprite2.getPosition().x < -skySpriteRect.width) {
                skySprite2.setPosition(skySpriteRect.width, 0);
            }

            // move the cactuses and bricks
            for (int i = 0; i < 5; i++) {
                cactuses[i].move(Vector2f(speed, 0.f));
            }
            for (int i = 0; i < 10; i++) {
                bricks[i].move(Vector2f(speed, 0.f));
            }

            // shoot and pull a sling to a brick when pressing left mouse button
            Vector2i mousePos = Mouse::getPosition(app);
            shootLine = false;
            line[0].position = Vector2f(x + 75, y + 50);
            line[1].position = Vector2f(mousePos.x, mousePos.y);
            for (int i = 0; i < 10; i++) {
                FloatRect brickRect = bricks[i].getGlobalBounds();
                if (brickRect.intersects(line.getBounds()) && Mouse::isButtonPressed(Mouse::Left)) {
                    line[1].position = Vector2f(brickRect.left+50, brickRect.top+50);
                    shootLine = true;
                    if (y > 300) {
                        dy -= 0.8;
                    }
                }
            }

            // 'animate' running
            bool onQuarterCycle = ((int)clock.getElapsedTime().asMilliseconds() / 10) % 8 == 0;
            bool onHalfCycle = ((int)clock.getElapsedTime().asMilliseconds() / 10) % 16 == 0;
            bool onFullCycle = ((int)clock.getElapsedTime().asMilliseconds() / 10) % 32 == 0;
            if (y > windowHeight - 200) {
                if (onHalfCycle && !onFullCycle) {
                    dinoSprite.setTexture(dinoTexture1);
                    runTexture = 1;
                }
                else if (onFullCycle) {
                    dinoSprite.setTexture(dinoTexture2);
                    runTexture = 2;
                }
                else if (onQuarterCycle && runTexture == 1) {
                    dinoSprite.setTexture(dinoTexture3);
                    runTexture = 3;
                }
                else if (onQuarterCycle && runTexture == 2) {
                    dinoSprite.setTexture(dinoTexture3);
                    runTexture = 3;
                }
            }
            else {
                dinoSprite.setTexture(dinoTexture1);
                runTexture = 1;
            }
        }
        else if (Keyboard::isKeyPressed(Keyboard::Space)) {
            startGame();
            clock.restart();
        }

        // clear the window
        app.clear();

        // draw everything here...
        app.draw(skySprite1);
        app.draw(skySprite2);
        app.draw(dinoSprite);
        app.draw(groundSprite);
        for (int i = 0; i < 5; i++) {
            app.draw(cactuses[i]);
        }
        for (int i = 0; i < 10; i++) {
            app.draw(bricks[i]);
        }
        app.draw(highScore);
        app.draw(score);
        if (shootLine) {
            app.draw(line);
        }

        // end the current frame
        app.display();
    }

    return 0;
}
