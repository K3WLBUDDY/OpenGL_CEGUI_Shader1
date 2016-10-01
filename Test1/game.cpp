#include "game.h"

#include<iostream>
#include<string>
#include"errors.h"
#include"ImageLoader.h"
#include"OpenGLTexture.h"
#include<CEGUI\CEGUI.h>
#include<CEGUI\RendererModules\OpenGL\GL3Renderer.h>


using namespace std;


game::game() : _window(nullptr),
				games(gamestate::PLAY),
				width(1024),
				height(768),
				time(0.0f)
{

}

void game::run()
{
	initsystems();

	_sprite.init(-1.0f,-1.0f,2.0f,2.0f);//Sends the Coordinates of the Vertices of the Quad to Sprite Class.

	_playerTexture = ImageLoader::loadPNG("Textures/JimmyJump/PNG/CharacterRight_Standing.png"); // Loads the PNG file into picopng for decoding into raw pixel data

	while (games != gamestate::STOP)
	{
		process_input();
		time += 0.01f; //Adds to the time variable every frame
		drawGame();
	}
}

void game::process_input() //This funtion must whenever the frame is updated.
{
	SDL_Event evnt; //Creates a variable of type SDL_event

	while (SDL_PollEvent(&evnt)) //Checks for any pending events that must be processed. Returns 1 if there is an event pending or 0 for none
	{
		switch (evnt.type) //Gets the type of the event that is pending.
		{
		case SDL_QUIT: //Refers to the event of pressing the cross.
			games = gamestate::STOP;
			break;
		case SDL_MOUSEMOTION: //Refers to the movement of the mouse
			cout << "\nX: " << evnt.motion.x << " Y: " << evnt.motion.y;
			break;
		}
	}
}

void game::initsystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);//Primary initialisation of everything.
	_window = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);//Creates an SDL window
	if (_window == nullptr)
	{
		fatalError("SDL Window could not be created!");//Calls the global function defined in errors.cpp
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(_window); //Creates an OpenGL context in the created SDL window
	if (glContext == nullptr)
	{
		fatalError("SDL_GL Context could not be created");
	}

	GLenum error = glewInit(); //Initialises GLEW and checks for error at the same time. 
	if (error != GLEW_OK)
	{
		fatalError("Could not Initialize GLEW");
	}
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);//Sets the Attribute and its value. Here Double Buffer is set to 1.

	
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);//Sets the Color of the OpenGL context to the specified values.


	initShaders();

	
}

void game::drawGame()
{

	//glClearDepth(1.0);//Clears the Z-Buffer or the Depth Buffer

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Clears the Color and Depth buffers.

	_colorProgram.use();
	
	glActiveTexture(GL_TEXTURE0); //Selects the Texture unit that will be affected
	glBindTexture(GL_TEXTURE_2D, _playerTexture.ID); //Binds the Texture

	GLint textureLocation = _colorProgram.getUniformLocation("mySampler"); //Gets the location of the Texture Variable present in the Shader
	glUniform1i(textureLocation,0);
	GLuint timeLocation = _colorProgram.getUniformLocation("time"); //Gets the Location of the Time Variable and stores it in timeLocation
	glUniform1f(timeLocation, time); //Sends the Variable to the GPU so that it can be used by the GPU whereever the variable time appears in the shaders


	_sprite.draw();
	glBindTexture(GL_TEXTURE_2D, 0); //Unbinds the Texture
	_colorProgram.unuse();
	SDL_GL_SwapWindow(_window);//Swaps between the two buffers in the DoubleBuffer window
}

void game::initShaders()
{
	//TODO: ADD COMMENTS FOR THIS FUNCTION
	_colorProgram.compileShaders("Shaders/colorShading.vert.txt", "Shaders/colorShading.frag.txt");//Calls the CompileShaders function in GLSLProgram Class
	_colorProgram.addAttribute("vertexPosition"); //Binds the vertexPosition Attribute from the Vertex Shader
	_colorProgram.addAttribute("vertexColor"); //Binds the vertexColor attribute from the Vertex Shader
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();

	CEGUI::OpenGL3Renderer& myRenderer = CEGUI::OpenGL3Renderer::bootstrapSystem();
	
}



game::~game()
{
}