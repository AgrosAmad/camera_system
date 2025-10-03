// STL
#include <iostream>

// X3
#include <X3.h>

// Setup scene
void LoadTest(std::shared_ptr<X3::Scene> scene);

int main()
{

	// Init engine
	X3::Engine* engine = new X3::Engine();

	// Setup scene
	engine->GetScene()->CallbackLoader(LoadTest);
	engine->GetScene()->Load();

	// Run
	engine->Run();
	return 0;
}

void LoadTest(std::shared_ptr<X3::Scene> scene)
{

	// Objects
	scene->AddCube();
	scene->AddMesh("../data/mesh/backpack/backpack.obj");

	// Lights
	scene->AddLight(X3::LightType::Point);
	scene->AddLight(X3::LightType::Directional);

}