#include "Player.h"

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}


bool Player::ProcessInput(MovementDir dir, int *m, Point pp)
{
  int move_dist = move_speed * 1;
  int k1, k2;
  switch(dir)
  {
    case MovementDir::UP:
	  k1 = m[(coords.y-move_dist)/tileSize*WW+coords.x/tileSize]; k2 = m[(coords.y-move_dist)/tileSize*WW+(coords.x-1)/tileSize+1];
	  if ((k1==0) && (k2==0))
      {
	    old_coords.y = coords.y;
        coords.y -= move_dist;
	  }
	  else if ((k1==1) || (k2==1)) {Reset(pp);}
	  else if ((k1==2) && (k2==2)) {return true;}
	  //else if ((k1==3) || (k2==3)) {Reset(pp);}
	  
      break;
    case MovementDir::DOWN:
      k1 = m[(coords.y+move_dist+tileSize-1)/tileSize*WW+coords.x/tileSize]; k2 = m[(coords.y+move_dist+tileSize-1)/tileSize*WW+(coords.x-1)/tileSize+1];
	  if ((k1==0) && (k2==0))
      {
        old_coords.y = coords.y;
        coords.y += move_dist;
	  }	
	  else if ((k1==1) || (k2==1)) {Reset(pp);}
	  else if ((k1==2) && (k2==2)) {return true;}
	  
      break;
    case MovementDir::LEFT:
      k1 = m[coords.y/tileSize*WW+(coords.x-move_dist)/tileSize]; k2 = m[((coords.y-1)/tileSize+1)*WW+(coords.x-move_dist)/tileSize];
	  if ((k1==0) && (k2==0))
      {
        old_coords.x = coords.x;
        coords.x -= move_dist;
	  }
	  else if ((k1==1) || (k2==1)) {Reset(pp);}
	  else if ((k1==2) && (k2==2)) {return true;}
	  
      break;
    case MovementDir::RIGHT:
      k1 = m[coords.y/tileSize*WW+(coords.x+move_dist+tileSize-1)/tileSize]; k2 = m[((coords.y-1)/tileSize+1)*WW+(coords.x+move_dist+tileSize-1)/tileSize];
	  if ((k1==0) && (k2==0))
      {
        old_coords.x = coords.x;
        coords.x += move_dist;
	  }
	  else if ((k1==1) || (k2==1)) {Reset(pp);}
	  else if ((k1==2) && (k2==2)) {return true;}
	  
      break;
    default:
      break;
  }
  return false;
}

Pixel getbgcolor(Image &i, int x, int y){return i.GetPixel(x,y);}
//Pixel getpcolor(Image &i, int x, int y){return i.GetPixel(x,tileSize - y - 1);}

void Player::Draw(Image &screen, Image &bg, Image &img)
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

