#include <stdio.h>
#include <SDL/SDL.h>
#include <iostream>
#include <stdlib.h>

struct Vector2
{
	float x, y;
	Vector2 operator + (Vector2& r)
	{
		Vector2 vec;
		vec.x = this->x + r.x;
		vec.y = this->y + r.y;
		return vec;
	}
	Vector2 operator * (float f)
	{
		Vector2 vec;
		vec.x = this->x * f;
		vec.y = this->y * f;
		return vec;
	}
};

struct PlayerData
{
	Vector2 position, velocity, frameVelocity;
	SDL_Rect rect, collisionRect;
	bool isGrounded;
	int rectOffset;
	PlayerData()
	{
		rect = SDL_Rect{};
		collisionRect = SDL_Rect{};
		rectOffset = -16;
		isGrounded = false;
		position = Vector2{ 0,0 };
		velocity = Vector2{ 0,0 };
		frameVelocity = Vector2{ 0,0 };
	}
};

struct Input
{
	bool jump = false;
	int horizontal = 0;
};

bool checkCollide(Vector2 pos, SDL_Rect& rect2)
{
	return (pos.x < rect2.x + rect2.w &&
		pos.x > rect2.x &&
		pos.y < rect2.y + rect2.h &&
		pos.y > rect2.y);
}

bool checkCollide(SDL_Rect& rect1, SDL_Rect& rect2)
{
	return (rect1.x < rect2.x + rect2.w &&
		rect1.x + rect1.w > rect2.x &&
		rect1.y < rect2.y + rect2.h &&
		rect1.y + rect1.h > rect2.y);
}

void updateVelocity(PlayerData& data, Input& input, float dt)
{
	data.velocity.x = (float)input.horizontal * 128;
	if (data.isGrounded)
	{
		data.velocity.y = 0;
		if (input.jump)
		{
			data.isGrounded = false;
			data.velocity.y = -128.f;
		}
	}
	else
	{
		data.velocity.y += 192.f * dt;
	}

	data.frameVelocity = data.velocity * dt;
}

void updatePosition(PlayerData& data, SDL_Rect* walls)
{
	Vector2 newPosition;
	
	newPosition = data.position + data.frameVelocity;

	SDL_Rect collisionCheck = SDL_Rect{ (int)newPosition.x + data.rectOffset, (int)newPosition.y + data.rectOffset, 32, 32 };
	bool lowerCollision = false;
	for (int i = 0; i < 5; i++)
	{
		if (checkCollide(collisionCheck, walls[i]))
		{
			newPosition = data.position;
			if (walls->y > data.position.y)
			{
				lowerCollision = true;
				data.velocity = data.frameVelocity = Vector2{ 0,0 };
			}
		}
	}
	data.isGrounded = lowerCollision;
	data.position = newPosition;

	data.collisionRect = { (int)data.position.x + data.rectOffset, (int)data.position.y + data.rectOffset,32,32 };
	data.rect = { (int)data.position.x, (int)data.position.y, 2, 2 };
}

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	bool running = true;

	PlayerData data;
	data.position = Vector2{ 128,128 };

	Input input;

	Uint64 previous_ticks = SDL_GetPerformanceCounter();


	SDL_Rect* walls = new SDL_Rect[5];
	walls[0] = SDL_Rect{ 128, 256, 250, 32 };
	walls[1] = SDL_Rect{ 300, 232, 32, 16 };
	walls[2] = SDL_Rect{ 372, 216, 32, 16 };
	walls[3] = SDL_Rect{ 444, 200, 32, 16 };
	walls[4] = SDL_Rect{ 516, 184, 32, 16 };
	
	while (running)
	{
		Uint64 ticks = SDL_GetPerformanceCounter();
		Uint64 delta_ticks = ticks - previous_ticks;
		previous_ticks = ticks;
		float delta_time = (float)delta_ticks / SDL_GetPerformanceFrequency();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			int scancode;
			switch (event.type)
			{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					scancode = event.key.keysym.scancode;
					if (scancode == SDL_SCANCODE_ESCAPE)
						running = false;
					else if (scancode == SDL_SCANCODE_LEFT)
						input.horizontal = -1;
					else if (scancode == SDL_SCANCODE_RIGHT)
						input.horizontal = 1;
					else if (scancode == SDL_SCANCODE_UP)
						input.jump = true;
					break;
				case SDL_KEYUP:
					scancode = event.key.keysym.scancode;
					if (scancode == SDL_SCANCODE_LEFT || scancode == SDL_SCANCODE_RIGHT)
						input.horizontal = 0;
					else if (scancode == SDL_SCANCODE_UP)
						input.jump = false;
					break;
			}
		}

		updateVelocity(data, input, delta_time);
		updatePosition(data, walls);

		SDL_SetRenderDrawColor(renderer, 0, 0, 40, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderDrawRect(renderer, &data.collisionRect);
		SDL_RenderFillRect(renderer, &data.rect);


		//render walls
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

		for (int i = 0; i < 5; i++)
		{
			SDL_RenderDrawRect(renderer, &walls[i]);
		}

		SDL_RenderPresent(renderer);

		SDL_Delay(16);
	}
}