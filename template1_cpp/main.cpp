#include "common.h"
#include "Image.h"
#include "Player.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <string.h>
#include <unistd.h>


constexpr GLsizei WINDOW_WIDTH = 640, WINDOW_HEIGHT = 640;

enum Tiles
{ GRASS, HOLE, FINISH, CRASH, WALL};

int Map [WW*WH];
int ppx, ppy;
Point starting_pos{.x = 0, .y = 0};
Point player_pos = starting_pos;

const char *lvls[5] = {"../lvl/Map1.txt", "../lvl/Map2.txt", "../lvl/Map3.txt", "../lvl/Map4.txt", "../lvl/Map5.txt"};
int lvlnum;

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void drawImage(Image &src, Point cords, Image &dest)
{
	int Width = src.Width();
	int Height = src.Height();

	for (int y = cords.y; y < cords.y + Height && y < dest.Height(); ++y)
		for (int x = cords.x; x < cords.x + Width && x < dest.Width(); ++x)
			dest.PutPixel(x, y, blend(dest.GetPixel(x, y), src.GetPixel(x - cords.x, Height - y - 1 + cords.y)));
}

void bgImage(Image &src, Point cords, Image &dest)
{
	int Width = src.Width();
	int Height = src.Height();

	for (int y = cords.y; y < cords.y + Height && y < dest.Height(); ++y)
		for (int x = cords.x; x < cords.x + Width && x < dest.Width(); ++x)
			dest.PutPixel(x, y, blend(dest.GetPixel(x, y), src.GetPixel(x, y)));
}

void drawMap(const char *map, Image &dest, Image &anim)
{
	FILE *f;
	
	if (f = fopen(map, "r"))
	{
		char c = fgetc(f);
		int n = 0;
		Image g("../resources/Grass.jpg"), h("../resources/Hole.jpg"), w("../resources/Wall.jpg"), fin("../resources/Finish1.jpg"), cw("../resources/Crash.jpg");
		
		while(c != EOF)
			switch (c)
			{
				case '@':
					c = '.'; ppx = tileSize*(n % WW); ppy = tileSize*(n / WH); 
				case '.':
					drawImage(g, {tileSize*(n % WW), tileSize*(n / WH)},dest);
					c = fgetc(f); Map[n++] = GRASS; 
					break;
				case ' ':
					drawImage(h, {tileSize*(n % WW), tileSize*(n / WH)}, dest);
					c = fgetc(f); Map[n++] = HOLE; 
					break;
				case '#':
					drawImage(w,{tileSize*(n % WW), tileSize*(n / WH)},dest);
					c = fgetc(f); Map[n++] = WALL; 
					break;
				case 'x':
					drawImage(fin,{tileSize*(n % WW), tileSize*(n / WH)},dest);
					//drawImage(fin,{tileSize*(n % WW), tileSize*(n / WH)},anim);
					c = fgetc(f); Map[n++] = FINISH; 
					break;
				case '%':
					drawImage(g,{tileSize*(n % WW), tileSize*(n / WH)},dest);
					drawImage(cw,{tileSize*(n % WW), tileSize*(n / WH)},anim);
					c = fgetc(f); Map[n++] = CRASH; 
					break;
				default: c = fgetc(f); break;
			}
	}	
}

void cleanbuf(Image &anim)
{
	for (int y = 0; y < WINDOW_HEIGHT; ++y)
		for (int x = 0; x < WINDOW_WIDTH; ++x)
			anim.PutPixel(x, y, backgroundColor);
}

void startlvl(const char *map, Image &bg, Image &anim, Image &scr, Player &p)
{
	if (lvlnum < 6)
	{
		cleanbuf(anim);
		drawMap(map, bg, anim);
		bgImage(bg, starting_pos, scr);
		bgImage(anim, starting_pos, scr);
		player_pos = {.x = ppx, .y = ppy};
		p.Reset(player_pos);
	}
	else
	{
		Image end ("../resources/Win.png");
		drawMap("../lvl/Map1.txt", bg, anim);
		cleanbuf(anim); cleanbuf(bg); cleanbuf(scr);
		drawImage(end, starting_pos, bg);
		bgImage(bg, starting_pos, scr);
		player_pos = {.x = ppx, .y = ppy};
		p.Reset(player_pos);
	}
	
}

void restartlvl(GLFWwindow* w, Image &end, Image &bg, Image &anim, Image &scr, Player &p)
{
	drawImage(end, starting_pos, scr); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
	glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, scr.Data()); GL_CHECK_ERRORS;
	glfwSwapBuffers(w);
	sleep(3);
	startlvl(lvls[lvlnum-1], bg, anim, scr, p);
}

void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

void processPlayerMovement(GLFWwindow* w, Player &player, Image &bg, Image &anim, Image &scr)
{
  int flag;
  Image end ("../resources/Died.png");
  if (Input.keys[GLFW_KEY_W]){
	flag = player.ProcessInput(MovementDir::UP, Map, player_pos);
    if (flag == 2) {startlvl(lvls[lvlnum++], bg, anim, scr, player);}
    else if (flag == 1) {restartlvl(w, end, bg, anim, scr, player);}
	}
  else if (Input.keys[GLFW_KEY_S]){
	flag = player.ProcessInput(MovementDir::DOWN, Map, player_pos);
    if (flag == 2) {startlvl(lvls[lvlnum++], bg, anim, scr, player);}
    else if (flag == 1) {restartlvl(w, end, bg, anim, scr, player);}
	}
  if (Input.keys[GLFW_KEY_A]){
	flag = player.ProcessInput(MovementDir::LEFT, Map, player_pos);
    if (flag == 2) {startlvl(lvls[lvlnum++], bg, anim, scr, player);}
    else if (flag == 1) {restartlvl(w, end, bg, anim, scr, player);}		
	}
  else if (Input.keys[GLFW_KEY_D]){
	flag = player.ProcessInput(MovementDir::RIGHT, Map, player_pos);
    if (flag == 2) {startlvl(lvls[lvlnum++], bg, anim, scr, player);}
    else if (flag == 1) {restartlvl(w, end, bg, anim, scr, player);}		
	}
  if (Input.keys[GLFW_KEY_SPACE]){
	Point p = player.getcoords();
	int n = (p.y + tileSize/2)/tileSize, m = (p.x + tileSize/2)/tileSize;
	int j, i = n-1;
	while (i < n+2){
		j = m-1;
		while (j < m+2){	
			if (Map[i*WW+j] == CRASH)
			{
				Map[i*WW+j] = GRASS;
				int a = i*WW+j;
				for (int y = tileSize*(a / WH); y < tileSize*(a / WH) + tileSize && y < WINDOW_HEIGHT; ++y)
					for (int x = tileSize*(a % WW); x < tileSize*(a % WW) + tileSize && x < WINDOW_WIDTH; ++x)
						anim.PutPixel(x, y, backgroundColor);
				
				bgImage(bg, starting_pos, scr);
				bgImage(anim, starting_pos, scr);
				i = n+2; j = m+2;
			}
			j++;
		}
		i++;
	}

  }
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}

void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press SPACE to crash walls"<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}


int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 A0 base project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	Image bg (WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	Image anim (WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	Image ball ("../resources/Ball.png");
	
	Player player{player_pos};
	

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

	startlvl(lvls[lvlnum++], bg, anim, screenBuffer, player);
	
  //game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
    glfwPollEvents();

    processPlayerMovement(window, player, bg, anim, screenBuffer);
    //bgImage(anim, starting_pos, screenBuffer);
    player.Draw(screenBuffer, bg, ball);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
