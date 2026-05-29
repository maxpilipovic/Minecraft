#include "Gui.h"
#include "mc.h"
#include "Window.h"

Gui::~Gui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::Init(Window& window)
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.GetNative(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void Gui::Update(bool showDebugMenu)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (showDebugMenu)
	{
		ImGui::Begin("Debug");
		ImGui::Text("Hello ImGui");
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
