#include "framework.h"

#include "Render.h"
#include "SceneManager.h"
#include "ComputeShader.h"


// Callback resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	srand(time(0));

	// -----------------------------
	// Init GLFW
	// -----------------------------
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Triangle OpenGL", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// -----------------------------
	// Init GLAD
	// -----------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	AssetManager::Init();
	InputManager::Init(window);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	SceneManager manager(window);
	
	// 0
	auto simpleSpherePool = manager.AddScene<Pool<200, Shape::SPHERE, true, 2.0f, 4.0f, Shape::SPHERE, Shape::SPHERE>>();
	// 1
	auto simpleAABBPool = manager.AddScene<Pool<200, Shape::CUBE_AABB, true, 2.0f, 4.0f, Shape::CUBE_AABB, Shape::CUBE_AABB>>();
	// 2
	auto simpleOBBPool = manager.AddScene<Pool<200, Shape::CUBE_OBB, true, 2.0f, 4.0f, Shape::CUBE_OBB, Shape::CUBE_OBB>>();

	// 3
	auto SpherePool1000 = manager.AddScene<Pool<1000, Shape::SPHERE, true, 2.0f, 4.0f, Shape::SPHERE, Shape::SPHERE, glm::vec3{120, 80, 120}, PROFILE_PHYSIC_STEP_COUNT_HIGH>> ();
	// 4
	auto AABBPool1000 = manager.AddScene<Pool<1000, Shape::CUBE_AABB, true, 2.0f, 2.0f, Shape::CUBE_AABB, Shape::CUBE_AABB, glm::vec3{ 120, 80, 120 }, PROFILE_PHYSIC_STEP_COUNT_HIGH>> ();

	// 5
	auto shootingRange_2500_opened = manager.AddScene<ShootingRange< 2500, 1.5f, 2.5f, 5.f, glm::vec3{ 180.0f, 180.0f, 180.0f } >>();

	// 6
	auto water = manager.AddScene<SceneWater>();

	// 7
	auto all = manager.AddScene<SceneAll_Implement>();

	// 8
	auto AbusiveSpherePool = manager.AddScene<AbusivePool<5000>>();

	// 9
	auto AbusiveSpherePool2 = manager.AddScene<AbusivePool<10000>>();

	manager.SetActiveScene(simpleSpherePool);

	double sum_fps[10]{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double frame_count[10]{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	// Add Entities

	auto start = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window))
	{

		// input
		if (InputManager::KeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
		if (InputManager::KeyPressed(GLFW_KEY_ENTER)) manager.SetInactive();

		if (InputManager::KeyPressed(GLFW_KEY_KP_0)) manager.SetActiveScene(0);
		if (InputManager::KeyPressed(GLFW_KEY_KP_1)) manager.SetActiveScene(1);
		if (InputManager::KeyPressed(GLFW_KEY_KP_2)) manager.SetActiveScene(2);
		if (InputManager::KeyPressed(GLFW_KEY_KP_3)) manager.SetActiveScene(3);
		if (InputManager::KeyPressed(GLFW_KEY_KP_4)) manager.SetActiveScene(4);
		if (InputManager::KeyPressed(GLFW_KEY_KP_5)) manager.SetActiveScene(5);
		if (InputManager::KeyPressed(GLFW_KEY_KP_6)) manager.SetActiveScene(6);
		if (InputManager::KeyPressed(GLFW_KEY_KP_7)) manager.SetActiveScene(7);
		if (InputManager::KeyPressed(GLFW_KEY_KP_8)) manager.SetActiveScene(8);
		if (InputManager::KeyPressed(GLFW_KEY_KP_9)) manager.SetActiveScene(9);

		manager.Update();

		auto end = std::chrono::high_resolution_clock::now();
		auto frame_time = end - start;
		uint64_t dt_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(frame_time).count();
		double dt_s = static_cast<double>(dt_ns) * 0.000000001;

		start = end;

		unsigned int current_scene = manager.GetActiveScene();
		if (current_scene < 0 || current_scene >= 10) continue;

		double fps = 1.0 / dt_s;
		sum_fps[current_scene] += fps;
		frame_count[current_scene] += 1.0;
	};

	std::cout << "Average fps over each simulation : " << std::endl;
	for (int i = 0; i < 10; ++i)
	{
		if (frame_count[i] == 0.0)
		{
			std::cout << "Scene " << i << ": No frame recorded" << std::endl;
			continue;
		}
		std::cout << "Scene " << i << ": " << sum_fps[i] / frame_count[i] << " fps" << std::endl;
	}

	// -----------------------------
	// Cleanup
	// -----------------------------

	AssetManager::Destroy();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}