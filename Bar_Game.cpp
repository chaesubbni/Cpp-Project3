#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

using namespace sf;
using namespace std;

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    RenderWindow window(VideoMode(600, 480), "ARKANOID");
    window.setFramerateLimit(60);

    // ---------- Fonts & HUD ----------
    Font font;
    if (!font.loadFromFile("assets/nanum.ttf")) {
        cout << "폰트 업로드 되지 않음" << endl;
    }
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setPosition(10, 10);

    int score = 0;
    int life = 3;
    int count = 0;

    // ---------- Textures & Sprites ----------
    Texture texBg, texTouch, texSuccess, texTouchTxt, texFail;
    texBg.loadFromFile("assets/space_background.png");
    texTouch.loadFromFile("assets/touch_boom.png");
    texSuccess.loadFromFile("assets/success_picture.png");
    texTouchTxt.loadFromFile("assets/space_touch_success.png");
    texFail.loadFromFile("assets/fail_picture.png");

    Sprite background(texBg), touch_boom(texTouch), success(texSuccess), touch(texTouchTxt), fail(texFail);
    background.setPosition(0, 0);

    // ---------- Ball & Paddle ----------
    CircleShape ball(10.f);
    ball.setFillColor(Color::White);
    ball.setPosition(300.f, 240.f);

    RectangleShape paddle(Vector2f(100.f, 43.f));
    paddle.setFillColor(Color(200, 200, 200));
    paddle.setPosition(300.f, 460.f);

    float dx = 3.f, dy = 3.f;

    // Falling bombs (red circles)
    vector<shared_ptr<CircleShape>> boom;

    // ---------- Clocks for short effects ----------
    Clock touchClock;   bool showTouch = false;
    Clock boomClock;    bool showTouch_Boom = false;

    // ---------- Sounds ----------
    SoundBuffer buffer_paddle_touch, buffer_moving, buffer_fail, buffer_success;
    buffer_moving.loadFromFile("assets/moving.mp3");
    buffer_paddle_touch.loadFromFile("assets/touch_sound.mp3");
    buffer_fail.loadFromFile("assets/booster_sound.mp3");   // 스샷 기준 이름 유지
    buffer_success.loadFromFile("assets/clap.mp3");

    Sound sound_paddle_touch, sound_moving, sound_boom, sound_clap;
    sound_moving.setBuffer(buffer_moving);
    sound_paddle_touch.setBuffer(buffer_paddle_touch);
    sound_boom.setBuffer(buffer_fail);
    sound_clap.setBuffer(buffer_success);

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();
            if (e.type == Event::KeyReleased) {
                if (e.key.code == Keyboard::Left || e.key.code == Keyboard::Right)
                    sound_moving.stop();
            }
        }

        // -------- Paddle move --------
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            paddle.move(5, 0);
            if (sound_moving.getStatus() != Sound::Playing) sound_moving.play();
        }
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            paddle.move(-5, 0);
            if (sound_moving.getStatus() != Sound::Playing) sound_moving.play();
        }
        // keep paddle in bounds
        if (paddle.getPosition().x < 0)
            paddle.setPosition(0, paddle.getPosition().y);
        if (paddle.getPosition().x + paddle.getGlobalBounds().width > 600)
            paddle.setPosition(600 - paddle.getGlobalBounds().width, paddle.getPosition().y);

        // -------- Ball physics --------
        Vector2f bpos = ball.getPosition();
        bpos.x += dx; bpos.y += dy;

        if (bpos.x < 0 || bpos.x > 600 - ball.getRadius() * 2) dx = -dx;
        if (bpos.y <= 0) dy = -dy;

        // miss: ball touch bottom
        if (bpos.y >= 480) {
            life--;
            bpos.x = static_cast<float>(rand() % 481);
            bpos.y = 0;
            dy = 5.f;
        }

        // -------- Game over sequence --------
        if (life == 0) {
            window.clear();
            window.draw(fail);
            sound_boom.play();
            window.display();
            sleep(seconds(3));

            window.clear(Color::Magenta);
            Text loseText;
            loseText.setFont(font);
            loseText.setFillColor(Color::Black);
            loseText.setString("You Lose!");
            loseText.setCharacterSize(60);
            loseText.setPosition(200, 200);
            window.draw(loseText);
            window.display();

            // boom 3 times
            for (int i = 0; i < 3; ++i) {
                sound_boom.play();
                sleep(seconds(1));
            }
            window.close();
            break;
        }

        // -------- periodic bomb spawns --------
        count++;
        if (count % 50 == 0) {
            int n = rand() % 3 + 2;
            for (int i = 0; i < n; ++i) {
                auto c = make_shared<CircleShape>(10.f);
                c->setPosition(static_cast<float>(rand() % 601), 0.f);
                c->setFillColor(Color::Red);
                boom.push_back(c);
            }
        }
        // move bombs
        for (auto it = boom.begin(); it != boom.end();) {
            if ((*it)->getPosition().y >= 480) {
                it = boom.erase(it);
                continue;
            }
            (*it)->setPosition((*it)->getPosition().x, (*it)->getPosition().y + 7);
            ++it;
        }
        // bomb vs paddle
        for (auto it = boom.begin(); it != boom.end();) {
            if (FloatRect((*it)->getPosition().x, (*it)->getPosition().y, 12, 12)
                .intersects(paddle.getGlobalBounds())) {
                life--;
                Vector2f boomPos = (*it)->getPosition();
                it = boom.erase(it);
                touch_boom.setPosition(boomPos.x - (touch_boom.getGlobalBounds().width / 2),
                    paddle.getPosition().y - touch_boom.getGlobalBounds().height);
                sound_boom.play();
                boomClock.restart();
                showTouch_Boom = true;
            }
            else ++it;
        }

        // secret: press P to get life
        if (Keyboard::isKeyPressed(Keyboard::P)) {
            life += 5;
            scoreText.setFillColor(Color(255, 223, 0));
            scoreText.setCharacterSize(60);
        }

        // ball vs paddle
        if (FloatRect(bpos.x, bpos.y, 12, 12).intersects(paddle.getGlobalBounds())) {
            dy = -(rand() % 5 + 2.f);
            score += 10;
            if (score % 50 == 0) { dx += (dx > 0 ? 5 : -5); dy += (dy > 0 ? 5 : -5); }
            ball.setFillColor(Color(rand() % 256, rand() % 256, rand() % 256));
            paddle.setFillColor(ball.getFillColor());
            touch.setPosition(bpos.x - (touch.getGlobalBounds().width / 2),
                paddle.getPosition().y - touch.getGlobalBounds().height);
            sound_paddle_touch.play();
            touchClock.restart();
            showTouch = true;
        }

        ball.setPosition(bpos);
        scoreText.setString("Score: " + to_string(score) + "\nLife: " + to_string(life));

        // -------- Render --------
        window.clear();
        window.draw(background);
        window.draw(ball);
        window.draw(paddle);
        window.draw(scoreText);
        for (auto& c : boom) window.draw(*c);

        if (showTouch && touchClock.getElapsedTime().asSeconds() < 1) window.draw(touch);
        else showTouch = false;

        if (showTouch_Boom && boomClock.getElapsedTime().asSeconds() < 1) window.draw(touch_boom);
        else showTouch_Boom = false;

        window.display();

        // -------- Win sequence --------
        if (score >= 150) {
            Text winText;
            winText.setFont(font);
            winText.setString("You Win!");
            winText.setCharacterSize(40);
            winText.setPosition(250, 240);
            sound_clap.play();

            vector<Vector2f> pos = { {0,0},{400,300},{0,300},{400,0} };
            for (auto& p : pos) {
                success.setPosition(p);
                window.clear();
                window.draw(success);
                if (p == pos.back()) window.draw(winText);
                window.display();
                sleep(seconds(1));
            }
            sleep(seconds(3));
            window.close();
        }
    }
    return 0;
}
