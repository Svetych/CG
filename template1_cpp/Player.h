#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};

  bool Moved() const;
  bool ProcessInput(MovementDir dir, int *m, Point pp);
  void Draw(Image &screen, Image &bg, Image &img);
  void Reset(Point p) {old_coords = coords; coords = p;}

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 0, .g = 255, .b = 255, .a = 255};
  int move_speed = 2;

};

#endif //MAIN_PLAYER_H
