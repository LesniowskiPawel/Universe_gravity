/*
OneLoneCoder.com - Code-It-Yourself! Asteroids at the command prompt (quick and simple c++)
"This one just kept growing..." - @Javidx9
License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9
GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE
From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!
Background
~~~~~~~~~~
I needed a quick video to keep up with my upload schedule, and thought "oh asteroids
must be simple", and indeed it was, but it was a bit rubbish, and didn't really
offer anything educational, until I made the decision to ditch sprites and use
vector models like the original game. This made the game considerably more sophisticated,
has opened up new possibilities for the game engine, and just goes to show, jus 'cos
it's old, doesn't mean it's irrelevant.
I ended up with a demonstration of physics, wireframe graphics, 2D matrix transformations,
toroidal space mapping and vector manipulation.
Controls
~~~~~~~~
Left and Right arrow keys steer the ship. Up arrow is thrust. There is no reverse thrust.
Space bar fires bullets. Get the highest score by surviving waves of asteroids.
Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com
YouTube: www.youtube.com/javidx9
Video:
~~~~~~
https://youtu.be/QgDR8LrRZhk
Last Updated: 30/08/2017
*/


/* Addition Joseph21 - 20220715
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 */

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class OneLoneCoder_Asteroids : public olc::PixelGameEngine
{
public:
	OneLoneCoder_Asteroids()
	{
		sAppName = "UNIVERSE";
	}

private:
	struct sSpaceObject
	{
		float nSize;
		double x;
		double y;
		double vx;
		double vy;
		float angle;
		double m;
		olc::Pixel color;
		double infoX;
		double infoY;
		double ax, ay;
	};

	std::vector<sSpaceObject> vecAsteroids;
	std::vector<sSpaceObject> vecBullets;
	sSpaceObject player;
	bool bDead = false;
	int nScore = 0;
	const double _G_CONST = 0.0667f; // 0.0000000000667f; 

	std::vector<std::pair<float, float>> vecModelShip;
	std::vector<std::pair<float, float>> vecModelAsteroid;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		vecModelShip =
		{
			{ 0.0f, -5.0f},
			{-2.5f, +2.5f},
			{+2.5f, +2.5f}
		}; // A simple Isoceles Triangle

		// Create a "jagged" circle for the asteroid. It's important it remains
		// mostly circular, as we do a simple collision check against a perfect
		// circle.
		int verts = 20;
		for (int i = 0; i < verts; i++)
		{
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			vecModelAsteroid.push_back(std::make_pair(noise * sinf(((float)i / (float)verts) * 6.28318f),
				noise * cosf(((float)i / (float)verts) * 6.28318f)));
		}

		ResetGame();
		return true;
	}

	void ResetGame()
	{
		// Initialise Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.vx = 0.0f;
		player.vy = 0.0f;
		player.angle = 0.0f;

		vecBullets.clear();
		vecAsteroids.clear();

		/*
		int nSize;
		double x;
		double y;
		double vx;
		double vy;
		float angle;
		double m;
		olc::Pixel color;
		double infoX;
		double infoY;
		double ax, ay;
		*/

		// Put in two asteroids
		vecAsteroids.push_back({ 15.0f, 550.0f, 340.0f, 0.0f, 0.0f, 0.0f, 32000.0f, olc::YELLOW });
		vecAsteroids.push_back({ 6.0f, 310.0f, 160.0f, -2.1f, 1.0f, 0.0f, 380.0f, olc::BLUE });
		vecAsteroids.push_back({ 4.0f, 560.0f, 558.0f, 1.4f, -0.2f, 0.0f, 120.0f, olc::CYAN });
		vecAsteroids.push_back({ 6.0f, 510.0f, 100.0f, -2.0f, 1.0f, 0.0f, 280.0f, olc::GREEN });

		vecAsteroids.push_back({ 1.0f, 502.0f, 92.0f, -1.0f, 0.0f, 0.0f, 1.0f, olc::GREY });
		vecAsteroids.push_back({ 1.0f, 512.0f, 108.0f, 0.0f, 0.2f, 0.0f, 1.0f, olc::GREEN });

		vecAsteroids.push_back({ 5.0f, 70.0f, 357.0f, 0.3f, 0.5f, 0.0f, 180.0f, olc::DARK_CYAN });
		vecAsteroids.push_back({ 5.0f, 850.0f, 400.0f, 0.1f, -0.7f, 0.0f, 180.0f, olc::DARK_BLUE });
		vecAsteroids.push_back({ 6.0f, 910.0f, 160.0f, -2.1f, 1.0f, 0.0f, 380.0f, olc::BLUE });
		vecAsteroids.push_back({ 4.0f, 60.0f, 558.0f, 1.4f, -0.2f, 0.0f, 120.0f, olc::CYAN });
		vecAsteroids.push_back({ 6.0f, 910.0f, 610.0f, -0.0f, -2.8f, 0.0f, 280.0f, olc::GREEN });

		/*
		for (uint8_t x = 5; x < 15; x++)
		{
			double rand_x = 600 + (x * 2);// (((float)rand() / (float)8.0) * 0.1) + ((float)rand() / (float)10.0 * 0.2);
			double rand_y = (((float)rand() / (float)x) * 0.1) + ((float)rand() / (float)50.0 * 0.2);
			vecAsteroids.push_back({ 1.0, rand_x, rand_y, -0.3f, -0.1f, 0.0f, 10.0f, olc::GREY });
		}

		for (uint8_t x = 16; x < 30; x++)
		{
			double rand_x = 650 + (x * 2);// (((float)rand() / (float)8.0) * 0.1) + ((float)rand() / (float)10.0 * 0.2);
			double rand_y = (((float)rand() / (float)x) * 0.1) + ((float)rand() / (float)50.0 * 0.3);
			vecAsteroids.push_back({ 1.0, rand_x, rand_y, -0.3f, -0.2f, 0.0f, 10.0f, olc::GREY });
		}

		for (uint8_t x = 31; x < 49; x++)
		{
			double rand_x = 700 + (x * 2);// (((float)rand() / (float)8.0) * 0.1) + ((float)rand() / (float)10.0 * 0.2);
			double rand_y = (((float)rand() / (float)x) * 0.2) + ((float)rand() / (float)50.0 * 0.4);
			vecAsteroids.push_back({ 1.0, rand_x, rand_y, -0.3f, -0.3f, 0.0f, 10.0f, olc::GREY });
		}

		for (uint8_t x = 50; x < 90; x++)
		{
			double rand_x = 800 + (x * 2);// (((float)rand() / (float)8.0) * 0.1) + ((float)rand() / (float)10.0 * 0.2);
			double rand_y = (((float)rand() / (float)x) * 0.3) + ((float)rand() / (float)50.0 * 0.5);
			vecAsteroids.push_back({ 1.0, rand_x, rand_y, -0.3f, -0.3f, 0.0f, 10.0f, olc::GREY });
		}
		*/

		// Reset game
		bDead = false;
		nScore = false;
	}

	// Implements "wrap around" for various in-game sytems
	void WrapCoordinates(float ix, float iy, float& ox, float& oy)
	{
		ox = ix;
		oy = iy;
		if (ix < 0.0f)	ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth())	ox = ix - (float)ScreenWidth();
		if (iy < 0.0f)	oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
	}

	// Overriden to handle toroidal drawing routines
	virtual bool Draw(int x, int y, olc::Pixel col = olc::WHITE)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olc::PixelGameEngine::Draw(fx, fy, col);
		return true;
	}

	bool IsPointInsideCircle(float cx, float cy, float radius, float x, float y)
	{
		return sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) < radius;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bDead)
			ResetGame();

		// Clear Screen
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		// Steer Ship
		if (GetKey(olc::Key::LEFT).bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld)
			player.angle += 5.0f * fElapsedTime;

		// Thrust? Apply ACCELERATION
		if (GetKey(olc::Key::UP).bHeld)
		{
			// ACCELERATION changes VELOCITY (with respect to time)
			player.vx += sin(player.angle) * 20.0f * fElapsedTime;
			player.vy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		// VELOCITY changes POSITION (with respect to time)
		player.x += player.vx * fElapsedTime;
		player.y += player.vy * fElapsedTime;

		// Keep ship in gamespace
		//WrapCoordinates(player.x, player.y, player.x, player.y);

		// Check ship collision with asteroids
		for (auto& a : vecAsteroids)
			if (IsPointInsideCircle(a.x, a.y, a.nSize, player.x, player.y))
				//bDead = true; // Uh oh...

		// Fire Bullet in direction of player
				if (GetKey(olc::Key::SPACE).bReleased)
					vecBullets.push_back({ 0, player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 100.0f });

		// Update and draw asteroids
		for (auto& a : vecAsteroids)
		{
			// VELOCITY changes POSITION (with respect to time)
			a.vx += ((fElapsedTime)*a.ax);
			a.vy += ((fElapsedTime)*a.ay);
			a.x += a.vx * fElapsedTime;
			a.y += a.vy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime; // Add swanky rotation :)

			vecBullets.push_back({ 0, a.x, a.y, 0, 0, 0 });

			static float days;
			static uint16_t years;
			days += 0.1;
			DrawString(2, 2, "DAYS: " + std::to_string(days), olc::CYAN, 1);
			DrawString(2, 10, "YEARS: " + std::to_string(years), olc::CYAN, 1);

			uint16_t time_elapsed = (uint16_t)days;
			if (time_elapsed >= 365)
			{
				if (a.color == olc::YELLOW)
				{
					days = 0;
					years++;					
					a.nSize += 0.5;
					a.m += 200;
				}
			}
			// Asteroid coordinates are kept in game space (toroidal mapping)
			//WrapCoordinates(a.x, a.y, a.x, a.y);

			// Draw Asteroids
			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, (float)a.nSize, a.color);
		}

		/* Calc new VELOCITY */
		float dx, dy, rr = 0, r3 = 0;
		for (auto& a : vecAsteroids)
		{
			a.ax = 0;
			a.ay = 0;
		}

		for (auto& actual : vecAsteroids)
		{
			for (auto& nextPos : vecAsteroids)
			{
				if (&nextPos != &actual)
				{
					dx = nextPos.x - actual.x;
					dy = nextPos.y - actual.y;
					rr = sqrt((dx * dx) + (dy * dy));
					r3 = (rr * rr * rr);
					//actual.ax += ((nextPos.m * dx) / r3);
					//actual.ay += ((nextPos.m * dy) / r3);
					actual.ax += ((_G_CONST * nextPos.m * dx) / r3);
					actual.ay += ((_G_CONST * nextPos.m * dy) / r3);

					if (actual.color == olc::YELLOW)
					{
						nextPos.angle = sqrt((dx * dx) + (dy * dy));
						DrawString(2, 26, "MASS SUN: " + std::to_string(actual.m), olc::YELLOW, 1);
					}
				}
			}

			if (GetKey(olc::Key::SPACE).bHeld)
			{
				float fvx = (float)actual.vx;
				float fvy = (float)actual.vy;
				//DrawString((actual.x + 5), (actual.y - 26), "X: " + std::to_string(actual.x), actual.color, 1);
				if (actual.color != olc::GREY)
				{
					DrawString((actual.x + 20), (actual.y - 18), "VX: " + std::to_string(fvx), actual.color, 1);
					DrawString((actual.x + 20), (actual.y - 10), "VY: " + std::to_string(fvy), actual.color, 1);
					//DrawString((actual.x + 20), (actual.y - 2), "S: " + std::to_string(actual.nSize), actual.color, 1);
				}

				DrawLine(actual.x, actual.y, ((actual.x - (actual.vx * 10))), ((actual.y - (actual.vy * 10))), olc::WHITE);
			}
		}

		for (auto& actual : vecAsteroids)
		{
			for (auto& nextPos : vecAsteroids)
			{
				if (&nextPos != &actual)
				{
					if (IsPointInsideCircle(actual.x, actual.y, actual.nSize, nextPos.x, nextPos.y))
					{
						// Create child asteroids
						DrawCircle(actual.x, actual.y, 3, olc::YELLOW);
						if (actual.nSize > 2.0)
						{
							if (actual.color == olc::YELLOW)
							{
								//actual.vx -= 0.001;
								//actual.vy -= 0.001;
								//actual.nSize = (actual.nSize - 1);
								actual.m = (actual.m - nextPos.m);
							}

							else
							{
								float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
								actual.vx = ((float)rand() / (float)100.0) * 0.001;
								actual.vy = -((float)rand() / (float)100.0) * 0.001;
								if (actual.nSize > 0)
								{
									actual.nSize = (actual.nSize - 1);
									actual.color = olc::RED;
								}

								else
								{
									//actual.x = 400;
									//actual.y = 340;
									actual.m = 0.0;
								}

								actual.angle = 10.0f * sinf(angle1);
							}

							if (nextPos.color == olc::YELLOW)
							{
								//actual.vx -= 0.001;
								//actual.vy -= 0.001;
								//actual.nSize = (actual.nSize - 1);
								nextPos.m = (actual.m - nextPos.m);
							}

							else
							{
								float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
								nextPos.vx = ((float)rand() / (float)100.0) * 0.001;
								nextPos.vy = -((float)rand() / (float)100.0) * 0.001;
								if (nextPos.nSize > 0)
								{
									nextPos.nSize = (nextPos.nSize - 1);
									nextPos.color = olc::RED;
								}

								else
								{
									//actual.x = 400;
									//actual.y = 340;
									nextPos.m = 0.0;
								}

								nextPos.angle = 10.0f * sinf(angle2);
							}
							//newAsteroids.push_back({ (int)actual.nSize >> 2 ,actual.x, actual.y, actual.vx, actual.vy, 10.0f * sinf(angle1), 10.0f * cosf(angle1), 0.0f });
							//vecAsteroids.push_back({ (int)actual.nSize, actual.x, actual.y, actual.vx, actual.vy, actual.angle, actual.m, actual.color });

							//newAsteroids.push_back({ (int)actual.nSize >> 2 ,actual.x, actual.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), 0.0f });
							//DrawString((actual.x + 5), (actual.y - 18), "VX: " + std::to_string(actual.vx), actual.color, 1);
						}
					}
				}
			}
		}
		/*
		// Update Bullets
		for (auto& b : vecBullets)
		{
			b.x += b.vx * fElapsedTime;
			b.y += b.vy * fElapsedTime;
			//WrapCoordinates(b.x, b.y, b.x, b.y);
			b.angle -= 1.0f * fElapsedTime;

			// Check collision with asteroids
			for (auto& a : vecAsteroids)
			{
				//if (IsPointInsideRectangle(a.x, a.y, a.x + a.nSize, a.y + a.nSize, b.x, b.y))
				if (IsPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y))
				{
					// Asteroid Hit - Remove bullet
					// We've already updated the bullets, so force bullet to be offscreen
					// so it is cleaned up by the removal algorithm.
					b.x = -100;

					// Create child asteroids
					if (a.nSize > 4)
					{
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), 0.0f });
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), 0.0f });
					}

					// Remove asteroid - Same approach as bullets
					a.x = -100;
					nScore += 100; // Small score increase for hitting asteroid
				}
			}
		} */

		// Any new asteroids created after collision detection are stored
		// in a temporary vector, so we don't interfere with the asteroids
		// vector iterator in the for(auto)
		std::vector<sSpaceObject> newAsteroids;
		// Append new asteroids to existing vector
		//for (auto a : newAsteroids)
			//vecAsteroids.push_back(a);

		// Clear up dead objects - They are out of game space

		// Remove asteroids that have been blown up
		if (vecAsteroids.size() > 0)
		{
			auto i = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.nSize == 0); });
			if (i != vecAsteroids.end())
				vecAsteroids.erase(i);
		}

		if (vecAsteroids.size() > 0)
		{
			auto j = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.x > 1375); });
			if (j != vecAsteroids.end())
				vecAsteroids.erase(j);
		}

		if (vecAsteroids.size() > 0)
		{
			auto k = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.y > 725); });
			if (k != vecAsteroids.end())
				vecAsteroids.erase(k);
		}

		/*
		if (vecAsteroids.empty()) // If no asteroids, level complete! :) - you win MORE asteroids!
		{
			// Level Clear
			nScore += 1000; // Large score for level progression
			vecAsteroids.clear();
			vecBullets.clear();

			// Add two new asteroids, but in a place where the player is not, we'll simply
			// add them 90 degrees left and right to the player, their coordinates will
			// be wrapped by th enext asteroid update
			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle - 3.14159f / 2.0f) + player.x,
											  30.0f * cosf(player.angle - 3.14159f / 2.0f) + player.y,
											  10.0f * sinf(player.angle), 10.0f * cosf(player.angle), 0.0f });

			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle + 3.14159f / 2.0f) + player.x,
											  30.0f * cosf(player.angle + 3.14159f / 2.0f) + player.y,
											  10.0f * sinf(-player.angle), 10.0f * cosf(-player.angle), 0.0f });
		}
		*/

		// Remove bullets that have gone off screen
		//if (vecBullets.size() > 0)
		//{
			//auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) { return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
			//if (i != vecBullets.end())
				//vecBullets.erase(i);
		//}

		// Draw Bullets
		//for (auto b : vecBullets)
			//Draw(b.x, b.y);

		// Draw Ship
		//DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		// Draw Score
		//DrawString(2, 2, "SCORE: " + std::to_string(nScore), olc::YELLOW, 1);
		return true;
	}

	void DrawWireFrameModel(const std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, olc::Pixel col = olc::WHITE)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
				vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, col);
		}
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_Asteroids game;
	if (game.Construct(1350, 700, 1, 1))
		game.Start();
	return 0;
}