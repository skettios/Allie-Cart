#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <deque>

struct TntLine
{
	bool populated[5] = { 0 };
	float position = 0.f;

	sf::VertexArray array;

	TntLine(int difficulty)
	{
		array.setPrimitiveType(sf::Quads);

		int count = rand() % 4 + 1;
		for (int i = 0; i < count; i++)
		{
			int loc = rand() % 5;
			if (populated[loc])
			{
				int new_loc = rand() % 5;
				populated[new_loc] = true;
			}
			else
			{
				populated[loc] = true;
			}
		}

		sf::Vertex vertex;
		for (int col = 0; col < 5; col++)
		{
			if (populated[col])
			{
				vertex.position = { 20.f + (col * 16.f), position };
				vertex.texCoords = { 0.f, 0.f };
				array.append(vertex);
				vertex.position = { 36.f + (col * 16.f), position };
				vertex.texCoords = { 16.f, 0.f };
				array.append(vertex);
				vertex.position = { 36.f + (col * 16.f), 16.f + position };
				vertex.texCoords = { 16.f, 16.f };
				array.append(vertex);
				vertex.position = { 20.f + (col * 16.f), 16.f + position };
				vertex.texCoords = { 0.f, 16.f };
				array.append(vertex);
			}
		}
	}

	void Update(float dt)
	{
		array.clear();

		position += 200.f * dt;

		sf::Vertex vertex;
		for (int col = 0; col < 5; col++)
		{
			if (populated[col])
			{
				vertex.position = { 20.f + (col * 16.f), position };
				vertex.texCoords = { 0.f, 0.f };
				array.append(vertex);
				vertex.position = { 36.f + (col * 16.f), position };
				vertex.texCoords = { 16.f, 0.f };
				array.append(vertex);
				vertex.position = { 36.f + (col * 16.f), 16.f + position };
				vertex.texCoords = { 16.f, 16.f };
				array.append(vertex);
				vertex.position = { 20.f + (col * 16.f), 16.f + position };
				vertex.texCoords = { 0.f, 16.f };
				array.append(vertex);
			}
		}
	}

	void Render(sf::RenderWindow& window, sf::Texture* texture)
	{
		sf::RenderStates states;
		states.texture = texture;

		window.draw(array, states);
	}
};

struct AllieCart
{
	sf::View game_view;

	sf::Texture rail_texture;
	sf::Texture tnt_texture;
	sf::Texture player_texture;

	sf::Sprite player;
	sf::Sprite tnt;

	sf::VertexArray rails;

	int player_position = 2;

	std::deque<TntLine*> tnt_lines;

	sf::Font font;
	sf::Text text;
	sf::Text dead_text;

	sf::SoundBuffer sound_buffer;
	sf::Sound sound;

	int game_state = 0; // 0 = menu, 1 = game, 2 = dead

	// game vars
	bool is_dead = false;
	int difficulty = 0;
	float spawn_frequency = 2.f; // every x seconds
	float difficulty_ramp_freq = 10.f; // every x seconds
	int score = 0;

	AllieCart(sf::RenderWindow& window)
	{
		sound_buffer.loadFromFile("assets/explosion.wav");
		sound.setBuffer(sound_buffer);

		font.loadFromFile("assets/Circus.ttf");
		text.setFont(font);
		text.setString("Allie\nDumb\nCart\nGame\n\nPress Enter");

		dead_text.setFont(font);
		dead_text.setString("You Died\n\nEnter to Retry\nEscape to Exit");

		game_view = window.getDefaultView();
		game_view.setSize(120, 180);
		game_view.setCenter(window.getSize().x / 8.f, window.getSize().y / 8.f);

		rail_texture.loadFromFile("assets/rail.png");
		tnt_texture.loadFromFile("assets/tnt.png");
		player_texture.loadFromFile("assets/player.png");

		player.setTexture(player_texture);
		player.scale({ 0.75f, 0.75f });
		tnt.setTexture(tnt_texture);

		player.setPosition(100.f, 100.f);

		rails.setPrimitiveType(sf::Quads);
		sf::Vertex vertex;
		for (int row = 0; row < 20; row++)
		{
			for (int col = 0; col < 5; col++)
			{
				vertex.position = { 20.f + (col * 16.f), 0.f + (row * 16.f) };
				vertex.texCoords = { 0.f, 0.f };
				rails.append(vertex);
				vertex.position = { 36.f + (col * 16.f), 0.f + (row * 16.f) };
				vertex.texCoords = { 16.f, 0.f };
				rails.append(vertex);
				vertex.position = { 36.f + (col * 16.f), 16.f + (row * 16.f) };
				vertex.texCoords = { 16.f, 16.f };
				rails.append(vertex);
				vertex.position = { 20.f + (col * 16.f), 16.f + (row * 16.f) };
				vertex.texCoords = { 0.f, 16.f };
				rails.append(vertex);
			}
		}
	}

	void OnKeyPress(sf::Keyboard::Key key)
	{
		if (game_state == 1)
		{
			switch (key)
			{
			case sf::Keyboard::Left:
				if (player_position > 0)
					player_position--;
				break;
			case sf::Keyboard::Right:
				if (player_position < 4)
					player_position++;
				break;
			default:
				break;
			}
		}
		else if (game_state == 0)
		{
			switch (key)
			{
			case sf::Keyboard::Enter:
				game_state = 1;
				break;
			default:
				break;
			}
		}
		else if (game_state == 2)
		{
			switch (key)
			{
			case sf::Keyboard::Enter:
				game_state = 1;
				break;
			case sf::Keyboard::Escape:
				game_state = 0;
			default:
				is_dead = false;
				break;
			}
		}
	}

	void Update(float dt)
	{
		if (game_state == 1)
		{
			sf::Vector2f new_player_pos(22.f, 160.f);
			new_player_pos.x += (player_position * 16.f);
			player.setPosition(new_player_pos);

			static float diff_accumulator = 0;
			static float accumulator = 0;
			accumulator += dt;
			diff_accumulator += dt;
			if (accumulator >= spawn_frequency)
			{
				tnt_lines.push_back(new TntLine(difficulty));
				accumulator -= spawn_frequency;
			}

			if (diff_accumulator >= difficulty_ramp_freq)
			{
				spawn_frequency -= 0.15f;
				diff_accumulator -= difficulty_ramp_freq;
			}

			// boom
			std::vector<TntLine*> delete_queue;
			for (auto line: tnt_lines)
			{
				line->Update(dt);

				if (line->position > 150.f && line->position <= 180)
				{
					if (line->populated[player_position])
					{
						sound.play();

						is_dead = true;
						player_position = 2;
						game_state = 2;
					}
				}

				if (line->position >= 181)
				{
					delete_queue.push_back(line);
					score++;
				}
			}

			for (auto line: delete_queue)
			{
				tnt_lines.pop_front();
				delete line;
			}

			if (is_dead)
			{
				for (auto line : tnt_lines)
				{
					delete line;
					tnt_lines.clear();
				}
			}
		}
	}

	void Render(sf::RenderWindow& window)
	{
		if (game_state == 0)
			window.draw(text);

		if (game_state == 2)
			window.draw(dead_text);

		if (game_state == 1)
		{
			window.setView(game_view);

			sf::RenderStates states;
			states.texture = &rail_texture;
			window.draw(rails, states);
			window.draw(player);

			for (auto line: tnt_lines)
				line->Render(window, &tnt_texture);

			window.setView(window.getDefaultView());
		}
	}
};

int main(int argc, char** argv)
{
	sf::RenderWindow window({ 480, 720 }, "Allie's Cart Game", sf::Style::Close | sf::Style::Titlebar);

	AllieCart ac(window);

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event window_event;
		while (window.pollEvent(window_event))
		{
			switch (window_event.type)
			{
			case sf::Event::KeyPressed:
				ac.OnKeyPress(window_event.key.code);
				break;
			case sf::Event::Closed:
				window.close();
				break;
			default:
				break;
			}

		}

		ac.Update(clock.restart().asSeconds());

		window.clear(sf::Color::Black);
		ac.Render(window);
		window.display();
	}
	return 0;
}
