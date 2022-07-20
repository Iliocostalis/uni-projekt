#include <iostream>
#include <cam.h>
#include <iomanip>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static void glfwError(int id, const char* description)
{
  std::cout << description << std::endl;
}

int main()
{

	glfwSetErrorCallback(&glfwError);
    int res = glfwInit();
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "MyWindow", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glViewport(0, 0, 1000, 1000);


    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    
    std::cout << "Test" << std::endl;

	return 0;

	Cam cam;
	cam.init();
	cam.start();


	//std::this_thread::sleep_for(std::chrono::seconds(3));

	//cam.stop();
	
	return 0;
}
