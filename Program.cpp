#include "Program.h"
#include "World.h"

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/sound.h>
#include <ClanLib/gl.h>

CL_ClanApplication application(&Program::main);

int Program::main(const std::vector<CL_String> &args)
{
	// Initialize ClanLib base components
	CL_SetupCore setupCore;

	// Initialize the ClanLib display component
	CL_SetupDisplay setupDisplay;

	// Initialize the OpenGL drivers
	CL_SetupGL setupGL;

	// Initialize the Sound drivers
	CL_SetupSound setupSound;

	// Start the Application
	try
	{		
		// Create a window
		CL_DisplayWindowDescription desc;
		desc.set_title("Four Connected");
		desc.set_allow_resize(true);
		desc.set_size(CL_Size(800, 600), true);
		CL_DisplayWindow window(desc);

		CL_GraphicContext gc = window.get_gc();

		// Create world
		World world(window);

		world.run();
	}
	catch(CL_Exception &exception)
	{
		// Create a console window for text-output if not available
		CL_ConsoleWindow console("Console", 80, 160);
		CL_Console::write_line("Exception caught: " + exception.get_message_and_stack_trace());
		console.display_close_message();

		return -1;
	}

	return 0;
}
