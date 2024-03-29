#include "Game.h"


// Initialization
void Game::initVars()
{
	this->window = nullptr;
}

void Game::input()
{
	std::string ans;
	std::cout << "Use config? (y/n): ";
	std::cin >> ans;
	if (ans == "y" || ans == "Y") {
		useConfig();
	} else {
		useKeyboard();
	}
}

void Game::useConfig()
{
	std::ifstream config("config.txt");
	std::string s;
	int c = 0;
	while (std::getline(config, s) && s.find(",") != -1) {
		int middle = s.find(",");
		float length = std::stod(s.substr(0, middle));
		float speed = std::stod(s.substr(middle + 1, s.length()));
		this->length_of_arms.push_back(length);
		this->speed_of_arms.push_back(speed * PI / 180);
		c++;
	}
	this->numberOfArms = c;
}

void Game::useKeyboard()
{
	do {
		std::cout << "Enter the number of arms (>= 1): ";
		std::cin >> this->numberOfArms;
	} while (this->numberOfArms < 1);
	for (int i = 1; i <= this->numberOfArms; i++) {
		std::cout << "Enter the length (pixels) of arm " << i << " : ";
		float in;
		std::cin >> in;
		this->length_of_arms.push_back(in);
	}
	for (int i = 1; i <= this->numberOfArms; i++) {
		std::cout << "Enter the speed (deg/s) of arm " << i << " : ";
		float in;
		std::cin >> in;
		in = in * PI / 180;
		this->speed_of_arms.push_back(in);
	}
}

void Game::saveConfig()
{
	std::ofstream config("config.txt");
	for (int i = 0; i < this->numberOfArms; i++) {
		config << this->length_of_arms[i] << "," << this->speed_of_arms[i] * 180 / PI << '\n';
	}
	std::cout << "Config saved\n";
}

void Game::initWindow()
{
	this->framerate = 60;
	this->videoMode.width = 1500;
	this->videoMode.height = 800;
	this->window = new RenderWindow(this->videoMode, "Spirograph", Style::Titlebar | Style::Close);
	this->window->setFramerateLimit(this->framerate);
}

void Game::initSpirograph()
{
	// Init colors
	this->armsColor = Color::Green;
	this->linesColor = Color::White;
	this->circlesColor = Color::Blue;
	// Init angles vector
	for (int i = 0; i < this->numberOfArms; i++) {
		this->current_angles.push_back(0);
	}
	// Init arms
	this->arms.setPrimitiveType(Lines);
	int k = 0;
	for (int i = 1; i < this->numberOfArms * 2; i += 2, k++) {
		if (k == 0) {
			this->arms.append(Vertex(Vector2f(this->window->getSize().x / 2, this->window->getSize().y / 2), this->armsColor));
			this->arms.append(Vertex(Vector2f(this->window->getSize().x / 2, this->window->getSize().y / 2 - this->length_of_arms[k]), this->armsColor));
		}
		else {
			this->arms.append(this->arms[i - 2]);
			this->arms.append(Vertex(Vector2f(this->arms[i - 1].position.x, this->arms[i - 1].position.y - this->length_of_arms[k]), this->armsColor));
		}
	}
	// Init lines
	this->lines.setPrimitiveType(LineStrip);
	// Init circles
	this->showCircles = false;
	for (int i = 0; i < this->numberOfArms; i++) {
		CircleShape* temp = new CircleShape;
		temp->setRadius(this->length_of_arms[i]);
		temp->setFillColor(Color::Transparent);
		temp->setOutlineColor(this->circlesColor);
		temp->setOutlineThickness(1.f);
		this->circles.push_back(temp);
	}
}

// Constructors & Destructors
Game::Game()
{
	this->input();
	this->initVars();
	this->initWindow();
	this->initSpirograph();
}

Game::~Game()
{
	for(auto circle : this->circles) {
		delete circle;
	}
	delete this->window;
}

// Accessors
bool Game::running() const
{
	return this->window->isOpen();
}

// Functions
void Game::pollEvents()
{
	while (this->window->pollEvent(this->event))
	{
		switch (this->event.type)
		{
		case Event::Closed:
			this->window->close();
			break;
		case Event::KeyPressed:
			if (this->event.key.code == Keyboard::Escape)
				this->window->close();
			if (this->event.key.code == Keyboard::Space)
				this->showCircles = !this->showCircles;
			if (this->event.key.code == Keyboard::E)
				this->saveConfig();
			break;
		}
	}
}

void Game::updateSpirograph()
{
	for (int i = 1; i < this->numberOfArms * 2; i += 2) {
		if (i/2 == 0) {
			this->arms[i].position.x = this->arms[i - 1].position.x + this->length_of_arms[i/2] * std::cos(this->current_angles[i/2]);
			this->arms[i].position.y = this->arms[i - 1].position.y + this->length_of_arms[i/2] * std::sin(this->current_angles[i/2]);
		}
		else {
			this->arms[i - 1] = this->arms[i - 2];
			this->arms[i].position.x = this->arms[i - 1].position.x + this->length_of_arms[i/2] * std::cos(this->current_angles[i/2]);
			this->arms[i].position.y = this->arms[i - 1].position.y + this->length_of_arms[i/2] * std::sin(this->current_angles[i/2]);
		}
		this->current_angles[i/2] += (this->speed_of_arms[i/2] / this->framerate);
		this->circles[i / 2]->setPosition(this->arms[i - 1].position.x - this->circles[i / 2]->getRadius(),
										 this->arms[i - 1].position.y - this->circles[i / 2]->getRadius());
	}
	this->lines.append(Vertex(this->arms[numberOfArms*2 - 1].position, this->linesColor));
}

void Game::update()
{
	this->pollEvents();
	this->updateSpirograph();
}

void Game::renderSpirograph()
{
	this->window->draw(this->lines);
	this->window->draw(this->arms);
	if (showCircles) {
		for (auto circle : this->circles) {
			this->window->draw(*circle);
		}
	}
}

void Game::render()
{
	this->window->clear();
	//
	this->renderSpirograph();
	//
	this->window->display();
}

