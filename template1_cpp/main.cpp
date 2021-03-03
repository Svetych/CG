#include "common.h"
#include "Image.h"
#include "Player.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = 640, WINDOW_HEIGHT = 640;

enum Tiles
{ GRASS, WALL, HOLE, CRASH, FINISH };

int Map [WW*WH];

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

void processPlayerMovement(Player &player)
{
  if (Input.keys[GLFW_KEY_W])
    player.ProcessInput(MovementDir::UP, Map);
  else if (Input.keys[GLFW_KEY_S])
    player.ProcessInput(MovementDir::DOWN, Map);
  if (Input.keys[GLFW_KEY_A])
    player.ProcessInput(MovementDir::LEFT, Map);
  else if (Input.keys[GLFW_KEY_D])
    player.ProcessInput(MovementDir::RIGHT, Map);
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
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

static Pixel blend(Pixel oldPixel, Pixel newPixel)
{
	newPixel.r = newPixel.a / 255.0 * (newPixel.r - oldPixel.r) + oldPixel.r;
	newPixel.g = newPixel.a / 255.0 * (newPixel.g - oldPixel.g) + oldPixel.g;
	newPixel.b = newPixel.a / 255.0 * (newPixel.b - oldPixel.b) + oldPixel.b;
	newPixel.a = 255;

	return newPixel;
}

void drawImage(Image &src, Point cords, Image &dest)
{
	int Width = src.Width();
	int Height = src.Height();

	for (int y = cords.y; y < cords.y + Height && y < dest.Height(); ++y)
		for (int x = cords.x; x < cords.x + Width && x < dest.Width(); ++x)
			dest.PutPixel(x, y, src.GetPixel(x - cords.x, Height - y - 1 + cords.y));
}

void bgImage(Image &src, Point cords, Image &dest)
{
	int Width = src.Width();
	int Height = src.Height();

	for (int y = cords.y; y < cords.y + Height && y < dest.Height(); ++y)
		for (int x = cords.x; x < cords.x + Width && x < dest.Width(); ++x)
			dest.PutPixel(x, y, src.GetPixel(x, y));
}

void drawMap(char *map, Image &dest)
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
					c = '.';
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
					c = fgetc(f); Map[n++] = FINISH; 
					break;
				case '%':
					drawImage(cw,{tileSize*(n % WW), tileSize*(n / WH)},dest);
					c = fgetc(f); Map[n++] = CRASH; 
					break;
				default: c = fgetc(f); break;
			}
	}	
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
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

	Point player_pos{.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2};
	Point starting_pos{.x = 0, .y = 0};
	Player player{player_pos};

	Image img("../resources/bg.jpg");
	Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	Image bg (WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	char str[] = "../lvl/Map.txt";
	drawMap(str, bg);
	

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

	bgImage(bg, starting_pos, screenBuffer);
	
  //game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
    glfwPollEvents();

    processPlayerMovement(player);
    player.Draw(screenBuffer, bg);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
