#pragma once

#include <SFML/Window.hpp>

class KeyManager
{
public:
	KeyManager()
	{
		keys = new int[sf::Keyboard::KeyCount];
		reset();
	}

	void update(sf::Event event)
	{
        if(event.type == sf::Event::KeyPressed)
        {
			keys[event.key.code] = true;
        }
		if(event.type == sf::Event::KeyReleased)
		{
			keys[event.key.code] = false;
		}
	}

	bool isDown(int key)
	{
		return keys[key];
	}

	void reset()
	{
		for(int i = 0; i < sf::Keyboard::KeyCount; i++)
		{
			keys[i] = false;
		}
	}

	void reset(int key)
	{
		keys[key] = false;
	}
private:
	int *keys;
};