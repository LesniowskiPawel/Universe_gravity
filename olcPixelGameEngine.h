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
#include <time.h>
using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define size_x_axis_screen 1300.0f
#define size_y_axis_screen 630.0f

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

	struct SpaceObjectTrace
	{
		double trace_x;
		double trace_y;
	};

	std::vector<sSpaceObject> vecPlanets;
	//std::vector<SpaceObjectTrace> tracePlanets;
	vector<sSpaceObject> tracePlanets;

	sSpaceObject player;
	bool bDead = false;
	int nScore = 0;
	const double _G_CONST = 0.0667f; // 0.0000000000667f; 

	bool showWelcomeText;

	uint16_t powerBank;

	struct tm newtime;
	time_t now;

	std::vector<std::pair<float, float>> vecModelShip;
	std::vector<std::pair<float, float>> vecModelPlanets;

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

		// Create a "jagged" circle for the planets. It's important it remains
		// mostly circular, as we do a simple collision check against a perfect
		// circle.
		int verts = 20;
		for (int i = 0; i < verts; i++)
		{
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			vecModelPlanets.push_back(std::make_pair(noise * sinf(((float)i / (float)verts) * 6.28318f),
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

		vecPlanets.clear();
		tracePlanets.clear();

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

		// Put planets as objects. First object is STAR with great MASS
		vecPlanets.push_back({ 19.0f, 570.0f, 340.0f, -0.1f, 0.0f, 0.0f, 10500.0f, olc::DARK_YELLOW }); // STAR always as first object!

		//vecPlanets.push_back({ 8.0f, 710.0f, 160.0f, -1.9f, 0.5f, 0.0f, 380.0f, olc::BLUE });
		vecPlanets.push_back({ 2.0f, 750.0f, 558.0f, 1.3f, -1.0f, 0.0f, 180.0f, olc::MAGENTA });
		vecPlanets.push_back({ 1.0f, 440.0f, 598.0f, 1.4f, 0.2f, 0.0f, 100.0f, olc::DARK_RED });

		/* Earth - Moon system :) */
		//vecPlanets.push_back({ 6.0f, 570.0f, 100.0f, -2.2f, 1.0f, 0.0f, 280.0f, olc::GREEN });
		//vecPlanets.push_back({ 2.0f, 561.5f, 91.0f, -1.4f, 0.0f, 0.0f, 5.0f, olc::GREY });

		//vecPlanets.push_back({ 2.0f, 430.0f, 185.0f, -1.7f, 1.0f, 0.0f, 248.0f, olc::GREEN });
		//vecPlanets.push_back({ 1.0f, 420.0f, 174.0f, -1.2f, 0.0f, 0.0f, 1.8f, olc::GREY });

		vecPlanets.push_back({ 2.0f, 510.0f, 235.0f, -2.0f, 1.0f, 0.0f, 248.0f, olc::GREEN });
		vecPlanets.push_back({ 1.0f, 500.0f, 227.0f, -1.5f, 0.0f, 0.0f, 1.8f, olc::GREY });

		/* Asteroids Zone */
		for (uint8_t y = 4; y < 6; y++)
		{
			for (uint8_t i = 5; i < 10; i++)
			{
				float x_rand = (((size_x_axis_screen - 100) + (y * y) + (i * 3)));
				float y_rand = ((i * i) + (y * y * y)) + (i / 2);
				vecPlanets.push_back({ 1.0f, x_rand, y_rand, 0.0f, 0.5f, 0.0f, 2.0f, olc::GREY });
			}
		}

		// Reset game
		bDead = false;
		nScore = false;

		powerBank = 1000; // FULL
		now = time(0);
		localtime_s(&newtime, &now);
		srand(time(NULL));
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

	void resizeStar(sSpaceObject* s)
	{
		if (s != NULL)
		{
			if (s->color == vecPlanets.at(0).color)
			{
				//s->nSize += 0.5;
				//s->m += 200;
			}
		}
	}

	void animationStarBlink(void)
	{
		static uint8_t revState;

		float x_posStar = vecPlanets.at(0).x;
		float y_posStar = vecPlanets.at(0).y;

		switch (revState)
		{
		case 0:
		{
			DrawCircle(x_posStar, y_posStar, 3, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 5, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 7, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 9, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 11, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 13, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 15, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 17, olc::DARK_YELLOW);
			vecPlanets.at(0).color = olc::DARK_YELLOW;
			revState++;
		} break;

		case 1:
		{
			DrawCircle(x_posStar, y_posStar, 3, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 5, olc::VERY_DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 7, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 9, olc::VERY_DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 11, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 13, olc::VERY_DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 15, olc::DARK_YELLOW);
			DrawCircle(x_posStar, y_posStar, 17, olc::VERY_DARK_YELLOW);
			vecPlanets.at(0).color = olc::VERY_DARK_YELLOW;
			revState = 0;
		} break;
		}
	}

	void timeForChoosePlaceForUser(void)
	{
		static uint16_t timerChoosePosition = 1023;
		static uint16_t timerShowTextForUser = 512;
		float catch_Xpos = GetMouseX();
		float catch_Ypos = GetMouseY();

		/* Choose a place for your ship */
		if ((GetKey(olc::Key::ENTER).bHeld) && (timerChoosePosition))
		{
			vecPlanets.push_back({ 1.0f, catch_Xpos, catch_Ypos, 0.0f, 0.0f, 0.0f, 5.0f, olc::DARK_CYAN });
			timerChoosePosition = 0;
			showWelcomeText = 1;
		}

		if (timerChoosePosition)
		{
			timerChoosePosition--;
			DrawString(catch_Xpos, (catch_Ypos + 22), "PRESS ENTER IF YOU WANT TO BE HERE", olc::CYAN, 1);
			DrawString((catch_Xpos), (catch_Ypos + 40), "TIME END... " + std::to_string(timerChoosePosition), olc::CYAN, 1);
		}

		if ((showWelcomeText) && (timerShowTextForUser))
		{
			timerShowTextForUser--;
			DrawString(catch_Xpos, (catch_Ypos + 22), "OK! Now your ship is pulled by gravity of star...", olc::CYAN, 1);
			//DrawString(catch_Xpos, (catch_Ypos + 42), "If You want to check speed your ship and other objects...", olc::CYAN, 1);
			//DrawString(catch_Xpos, (catch_Ypos + 62), "Press & hold SPACE button!", olc::CYAN, 1);

			if ((GetKey(olc::Key::SPACE).bHeld))
			{
				//DrawString(catch_Xpos, (catch_Ypos + 82), "EXCELLENT! Now You see a speed objects in x and y axis", olc::CYAN, 1);
				DrawString(catch_Xpos, (catch_Ypos + 102), "WHITE line at the end of object is a VECTOR MOVING", olc::CYAN, 1);
			}
		}

		if (!timerShowTextForUser)
		{
			showWelcomeText = 0;
			//DrawString(catch_Xpos, (catch_Ypos + 22), "Use a buttons UP, LEFT and RIGHT to control your ship", olc::CYAN, 1);
			//DrawString(catch_Xpos, (catch_Ypos + 44), "GOOD LUCK !", olc::CYAN, 1);
		}
	}

	void engineShipStart(void)
	{
		powerBank -= 5;
	}

	void chargePowerBank(sSpaceObject* s)
	{
		if (s != NULL)
		{
			if (powerBank < (1000))
			{
				static uint8_t charge;
				charge += 1;

				int diffX = (s->x - ((int16_t)vecPlanets.at(0).x));
				int diffY = (s->y - ((int16_t)vecPlanets.at(0).y));

				uint32_t diff_X_abs = abs(diffX);
				uint32_t diff_Y_abs = abs(diffY);

				uint16_t distanceFromStar = sqrt((diff_X_abs * diff_X_abs) + (diff_Y_abs * diff_Y_abs));
				uint16_t speedOfLoad = distanceFromStar / 10;

				if (charge > speedOfLoad) // as delay...
				{
					charge = 0;
					powerBank += 1;
				}
			}
		}
	}

	void powerBankStatusShow(void)
	{
		DrawString((size_x_axis_screen - 140), (size_y_axis_screen - 10), "POWER BANK: " + std::to_string(powerBank), olc::CYAN, 1);
	}

	void shipControl(sSpaceObject* s, float elapsedT)
	{
		if (s != NULL)
		{
			if (s->color == olc::DARK_CYAN)
			{
				if ((GetKey(olc::Key::LEFT).bHeld)
					|| (GetKey(olc::Key::RIGHT).bHeld)
					|| (GetKey(olc::Key::UP).bHeld)
					|| (GetKey(olc::Key::DOWN).bHeld))
				{
					// Steer Ship
					if (GetKey(olc::Key::LEFT).bHeld)
					{
						//float readAngle = s->angle;
						//readAngle -= 100.0f * elapsedT;
						//s->angle = readAngle;
						s->angle -= 0.2f * elapsedT;
					}

					if (GetKey(olc::Key::RIGHT).bHeld)
					{
						//float readAngle = s->angle;
						//readAngle += 100.0f * elapsedT;
						//s->angle = readAngle;
						s->angle += 0.2f * elapsedT;
					}

					// Thrust? Apply ACCELERATION
					if (GetKey(olc::Key::UP).bHeld)
					{
						// ACCELERATION changes VELOCITY (with respect to time)
						s->vx += sin(player.angle) * 0.2f * elapsedT;
						s->vy += -cos(player.angle) * 0.2f * elapsedT;

						engineShipStart();
					}

					// Thrust? Apply ACCELERATION
					if (GetKey(olc::Key::DOWN).bHeld)
					{
						// ACCELERATION changes VELOCITY (with respect to time)
						s->vx -= sin(player.angle) * 0.2f * elapsedT;
						s->vy -= -cos(player.angle) * 0.2f * elapsedT;

						engineShipStart();
					}
				}

				else
				{
					chargePowerBank(s);
				}
			}
		}
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//if (bDead)
			//ResetGame();

		// Clear Screen
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::VERY_DARK_MAGENTA);

		// Update and draw objects
		for (auto& a : vecPlanets)
		{
			// VELOCITY changes POSITION (with respect to time)
			a.vx += ((fElapsedTime)*a.ax);
			a.vy += ((fElapsedTime)*a.ay);
			a.x += a.vx * fElapsedTime;
			a.y += a.vy * fElapsedTime;
			a.angle += 0.1f * fElapsedTime; // Add swanky rotation :)

			static float days;
			static uint16_t years;
			days += 0.008;

			animationStarBlink();

			/* THERE YOU CAN ADD YOUR DATA FOR DISPLAY AT RIGHT SIDE OF THE SCREEN */
			DrawString((size_x_axis_screen - 140), (size_y_axis_screen - 50), "DAYS: " + std::to_string(days), olc::CYAN, 1);
			DrawString((size_x_axis_screen - 140), (size_y_axis_screen - 38), "YEARS: " + std::to_string(years), olc::CYAN, 1);


			uint16_t time_elapsed = (uint16_t)days;
			if (time_elapsed >= 365)
			{
				days = 0;
				years++;

				resizeStar(&a);
			}
			// Planets coordinates are kept in game space (toroidal mapping)
			//WrapCoordinates(a.x, a.y, a.x, a.y);

			// Draw Planetss
			DrawWireFrameModel(vecModelPlanets, a.x, a.y, a.angle, (float)a.nSize, a.color);

			/* CHOOSE A PLACE FOR YOUR SHIP !*/
			timeForChoosePlaceForUser();

			/*... AND CONTROL */
			shipControl(&a, fElapsedTime);
			powerBankStatusShow();

			tracePlanets.push_back({ 0, a.x, a.y, 0.0, 0.0, 0.0, 0.0, a.color, 0.0, 0.0 });
		}

		/* Clear VELOCITY... */
		float dx, dy, rr = 0, r3 = 0;
		for (auto& a : vecPlanets)
		{
			a.ax = 0;
			a.ay = 0;
		}

		/*...And calculate again. This part calculates the movement based on gravity */
		for (auto& actual : vecPlanets)
		{
			for (auto& nextPos : vecPlanets)
			{
				if (&nextPos != &actual) // -> 
				{
					dx = nextPos.x - actual.x;
					dy = nextPos.y - actual.y;
					rr = sqrt((dx * dx) + (dy * dy));
					r3 = (rr * rr * rr);

					actual.ax += ((_G_CONST * nextPos.m * dx) / r3);
					actual.ay += ((_G_CONST * nextPos.m * dy) / r3);

					if (actual.color == vecPlanets.at(0).color)
					{
						actual.vx = 0.0f;
						actual.vy = 0.0f;
						nextPos.angle = sqrt((dx * dx) + (dy * dy));
						uint16_t convertMass = (uint16_t)actual.m;
						DrawString((size_x_axis_screen - 160), (size_y_axis_screen - 24), "MASS OF STAR: " + std::to_string(convertMass), olc::YELLOW, 1);
					}
				}
			}

			/* SPACE button show coordinates of planets*/
			if (GetKey(olc::Key::F2).bHeld)
			{
				float fvx = (float)actual.vx;
				float fvy = (float)actual.vy;

				if (actual.color != olc::GREY)
				{
					DrawString((actual.x + 20), (actual.y - 18), "VX: " + std::to_string(fvx), actual.color, 1);
					DrawString((actual.x + 20), (actual.y - 10), "VY: " + std::to_string(fvy), actual.color, 1);

					int diffX = (actual.x - ((int16_t)vecPlanets.at(0).x));
					int diffY = (actual.y - ((int16_t)vecPlanets.at(0).y));

					uint32_t diff_X_abs = abs(diffX);
					uint32_t diff_Y_abs = abs(diffY);

					uint16_t distanceFromStar = sqrt((diff_X_abs * diff_X_abs) + (diff_Y_abs * diff_Y_abs));
					DrawString((actual.x + 20), (actual.y - 26), "DIST: " + std::to_string(distanceFromStar), actual.color, 1);

					DrawString(5, 5, "Start test time: " + std::to_string(newtime.tm_mon) + "_" + std::to_string(newtime.tm_mday) + "_" + std::to_string(newtime.tm_hour) + "_" + std::to_string(newtime.tm_hour) + "_" + std::to_string(newtime.tm_sec), olc::YELLOW, 1);
					//if (actual.color == olc::DARK_CYAN)
					//{
						//DrawString((actual.x + 20), (actual.y - 26), "A: " + std::to_string(actual.angle), actual.color, 1);
					//}
				}

				/* This is vector of moving */
				DrawLine(actual.x, actual.y, ((actual.x - (actual.vx * 10))), ((actual.y - (actual.vy * 10))), olc::WHITE);
			}
		}

		/* This part is a collision detect and animation */
		for (auto& actual : vecPlanets)
		{
			for (auto& nextPos : vecPlanets)
			{
				if (&nextPos != &actual)
				{
					if (IsPointInsideCircle(actual.x, actual.y, actual.nSize, nextPos.x, nextPos.y))
					{
						// Create child asteroids
						DrawCircle(actual.x, actual.y, 3, olc::YELLOW);
						if (actual.nSize >= 1.0)
						{
							if (actual.color == vecPlanets.at(0).color)
							{
								/* In this case we assume that collision star with planet it only change its mass */
								//actual.vx -= 0.001;
								//actual.vy -= 0.001;
								//actual.nSize = (actual.nSize - 1);
								nextPos.m = 0;
								auto k = remove_if(vecPlanets.begin(), vecPlanets.end(), [&](sSpaceObject o) { return (o.m == 0); });
								if (k != vecPlanets.end())
								{
									vecPlanets.erase(k);
								}
								actual.m = (actual.m - 2.0f); // MASS star in this case
							}

							/* If meet two planets we calculate speed after collision and change color on RED */
							else
							{
								float angle1 = ((float)(rand() % 360) + 1) * 6.283185f;

								double vx_calc = ((float)(rand() % 10)) * 0.2;
								if (vx_calc > 5.0f) { actual.vx = 5.0f; }
								if (vx_calc < -5.0f) { actual.vx = -5.0f; }
								else { actual.vx = vx_calc; }

								double vy_calc = -(((float)(rand() % 10)) * 0.1);
								if (vy_calc > 5.0f) { actual.vy = 5.0f; }
								if (vy_calc < -5) { actual.vy = -5.0f; }
								else { actual.vy = vy_calc; }

								if (actual.nSize > 0)
								{
									//actual.nSize = (actual.nSize - 1);
									actual.color = olc::RED;
								}

								//else
								//{
									//actual.m = 0.0;
								//}

								actual.angle = 10.0f * sinf(angle1);
							}

							if (nextPos.color == vecPlanets.at(0).color)
							{
								actual.m = 0;
								auto k = remove_if(vecPlanets.begin(), vecPlanets.end(), [&](sSpaceObject o) { return (o.m == 0); });
								if (k != vecPlanets.end())
								{
									vecPlanets.erase(k);
								}
								nextPos.m = (nextPos.m - 2.0f);
							}

							else
							{

								float angle2 = ((float)(rand() % 360) + 1) * 6.283185f;

								double vx_calc = ((float)(rand() % 10)) * -0.1;
								if (vx_calc > 5.0f) { nextPos.vx = 2.0f; }
								if (vx_calc < -5.0f) { nextPos.vx = -3.0f; }
								else { actual.vx = vx_calc; }

								double vy_calc = -(((float)(rand() % 10)) * -0.2);
								if (vy_calc > 5.0f) { nextPos.vy = 2.0f; }
								if (vy_calc < -5.0f) { nextPos.vy = -3.0f; }
								else { actual.vy = vy_calc; }

								if (nextPos.nSize > 0)
								{
									//nextPos.nSize = (nextPos.nSize - 1);
									nextPos.color = olc::RED;
								}

								nextPos.angle = 10.0f * sinf(angle2);
							}
						}
					}
				}
			}
		}

		// Any new asteroids created after collision detection are stored
		// in a temporary vector, so we don't interfere with the planets - next version...
		// vector iterator in the for(auto)
		std::vector<sSpaceObject> newAsteroids;
		// Append new planets to existing vector
		//for (auto a : newAsteroids)
			//vecAsteroids.push_back(a);

		// Remove planet when...
		// 

		// loaction on x axis is greather than 1375 (just off screen)
		if (vecPlanets.size() > 0)
		{
			auto j = remove_if(vecPlanets.begin(), vecPlanets.end(), [&](sSpaceObject o) { return (o.x > size_x_axis_screen); });
			if (j != vecPlanets.end())
				vecPlanets.erase(j);
		}

		// loaction on x axis is greather than 725 (just off screen)
		if (vecPlanets.size() > 0)
		{
			auto k = remove_if(vecPlanets.begin(), vecPlanets.end(), [&](sSpaceObject o) { return (o.y > size_y_axis_screen); });
			if (k != vecPlanets.end())
				vecPlanets.erase(k);
		}

		if ((GetKey(olc::Key::F1).bHeld))
		{
			// Draw trace of planets
			for (auto b : tracePlanets)
			{
				Draw(b.x, b.y, b.color);
			}

			for (uint16_t j = 1; j < 18; j++)
			{
				float horizontalLine = j * 34;
				DrawLine(0.0f, horizontalLine, size_x_axis_screen, horizontalLine, olc::VERY_DARK_GREY); // HORIZONTAL
				DrawString(10, (horizontalLine + 10), "" + std::to_string(j), olc::VERY_DARK_GREY, 1);
			}

			for (uint16_t k = 1; k < 22; k++)
			{
				float verticallLine = k * 57;
				DrawLine(verticallLine, 0, verticallLine, size_y_axis_screen, olc::VERY_DARK_GREY); // VERTICAL
				DrawString((verticallLine + 10), 10, "" + std::to_string(k), olc::VERY_DARK_GREY, 1);
			}
		}

		// Draw Ship - to next version...
		//DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		// Draw Score - to next version...
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

	/* Here You can change size of window and size models */
	if (game.Construct(size_x_axis_screen, size_y_axis_screen, 1, 1))
		game.Start();
	return 0;
}