#include "Player.h"

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

bool checktile(int x, int y, int *m) {return ! (m[y / tileSize * WW + x / tileSize]);}

void Player::ProcessInput(MovementDir dir, int *m)
{
  int move_dist = move_speed * 2;
  switch(dir)
  {
    case MovementDir::UP:
	  if (checktile(coords.x, coords.y - move_dist, m))
      {
	    old_coords.y = coords.y;
        coords.y -= move_dist;
	  }
      break;
    case MovementDir::DOWN:
	  if (checktile(coords.x, coords.y + move_dist + tileSize -1 , m))
      {
        old_coords.y = coords.y;
        coords.y += move_dist;
	  }	
      break;
    case MovementDir::LEFT:
	  if (checktile(coords.x - move_dist, coords.y, m))
      {
        old_coords.x = coords.x;
        coords.x -= move_dist;
	  }
      break;
    case MovementDir::RIGHT:
	  if (checktile(coords.x + move_dist + tileSize - 1, coords.y, m))
      {
        old_coords.x = coords.x;
        coords.x += move_dist;
	  }
      break;
    default:
      break;
  }
}

Pixel getbgcolor(Image &i, int x, int y){return i.GetPixel(x,y);}

void Player::Draw(Image &screen, Image &bg)
{
  if(Moved())
  {
    for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
    {
      for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
      {
        screen.PutPixel(x, y, getbgcolor(bg,x,y));
      }
    }
    old_coords = coords;
  }

  for(int y = coords.y; y <= coords.y + tileSize; ++y)
  {
    for(int x = coords.x; x <= coords.x + tileSize; ++x)
    {
      screen.PutPixel(x, y, color);
    }
  }
}

