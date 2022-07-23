#include "EditorToolstrip.h"
#include "Base/Engine.h"
#include "Base/ResourceManager.h"
#include "Scene/SceneManager.h"
#include "Render/Renderer.h"

#include "GameObjects/Camera.h"
#include "GameObjects/Sprite.h"
#include "GameObjects/TestObject.h"
#include "GameObjects/NoTextureTestObject.h"

#include "Editor/EditorColorPicker.h"

namespace S2DE::Editor
{
	EditorToolStrip::EditorToolStrip() 
	{

	}

	EditorToolStrip::~EditorToolStrip()
	{

	}

	void EditorToolStrip::Render()
	{
		if (!m_draw)
			return;

		m_inspector = reinterpret_cast<EditorObjectInspector*>(Core::Engine::GetRenderer()->GetImGui_Window("EditorObjectInspector"));
		S2DE_ASSERT(m_inspector);

		if (ImGui::BeginMainMenuBar()) 
		{
			auto maincamera = Scene::GetObjectByName<GameObjects::Camera>(S2DE_MAIN_CAMERA_NAME);
			
			if (ImGui::BeginMenu("File")) 
			{
				if (ImGui::MenuItem("Open"))
				{

				}

				if (ImGui::MenuItem("Save"))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Object inspector"))
				{
					m_inspector->ToggleDraw();
				}

				if (ImGui::MenuItem("Toggle objects visible"))
				{
					Core::Engine::GetSceneManager()->ToggleGameObjectVisibility();
				}

				if (ImGui::MenuItem("Toggle editor windows"))
				{
					Core::Engine::GetRenderer()->ToggleImGuiWindowsVisible();
				}

				if (ImGui::MenuItem("Toggle debug (in objects) windows"))
				{
					Core::Engine::GetSceneManager()->ToggleImGUIVisibility();
				}


				if (ImGui::BeginMenu("Fill mode"))
				{
					if (ImGui::MenuItem("Wireframe"))
					{
						Core::Engine::GetRenderer()->SwitchFillMode(Render::RenderFillMode::Wireframe);
					}

					if (ImGui::MenuItem("Solid"))
					{
						Core::Engine::GetRenderer()->SwitchFillMode(Render::RenderFillMode::Solid);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) 
			{
				if (ImGui::MenuItem("Undo"))
				{

				}

				if (ImGui::MenuItem("Redo"))
				{

				}

				if (ImGui::MenuItem("Delete"))
				{
					if (m_inspector->GetHandle() != nullptr)
					{
						//Get object name from handle
						std::string handle_name = m_inspector->GetHandle()->GetName();
						//Reset handle in inspector 
						m_inspector->Reset();
						//Delete object
						Core::Engine::GetSceneManager()->GetScene()->Delete(handle_name);
					}
				}

				if (ImGui::MenuItem("Clone"))
				{
				
				}

				if (ImGui::MenuItem("Rename"))
				{

				}
			
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::BeginMenu("Create"))
				{
					DirectX::SimpleMath::Vector3 spawn_vec = DirectX::SimpleMath::Vector3::Zero;

					if (maincamera != nullptr)
						spawn_vec = maincamera->GetPosition();

					if (ImGui::MenuItem("GameObject"))
					{
						Scene::CreateGameObject<GameObjects::GameObject>("GameObject", "GameObject", 1, spawn_vec);
					}
					if (ImGui::MenuItem("Sprite"))
					{
						Scene::CreateGameObject<GameObjects::Sprite>("Sprite", "GameObject", 1, spawn_vec);
					}

					if (ImGui::MenuItem("Camera"))
					{

					}

					if (ImGui::BeginMenu("Test's"))
					{
						if (ImGui::MenuItem("Basic Test object"))
						{
							Scene::CreateGameObject<GameObjects::TestObject>("TestObject", "GameObject", 1, spawn_vec);
						}

						if (ImGui::MenuItem("No texture test object"))
						{
							Scene::CreateGameObject<GameObjects::NoTextureTestObject>("NoTextureTestObject", "GameObject", 1, spawn_vec);
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Reload textures"))
				{
					Core::Engine::GetResourceManager().ReloadTextures();
					Core::Engine::GetSceneManager()->UpdateTextures();
				}

				if (ImGui::MenuItem("Reload shaders"))
				{
					Core::Engine::GetResourceManager().ReloadShaders();
					Core::Engine::GetSceneManager()->UpdateShaders();
				}

				if (ImGui::MenuItem("Clear scene"))
				{
					m_inspector->Reset();
					Core::Engine::GetSceneManager()->GetScene()->Clear();
				}

				if (ImGui::MenuItem("Toggle GameObject update"))
				{
					Core::Engine::GetSceneManager()->ToggleGameObjectUpdate();
				}

				if (ImGui::BeginMenu("Render"))
				{
					if (ImGui::MenuItem("Change background color"))
					{
						Core::Engine::GetRenderer()->GetImGui_Window("EditorBgColorPicker")->ToggleDraw();
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (maincamera != nullptr)
			{
				if (ImGui::Button("Orthographics"))
				{
					maincamera->SetProjectionMode(GameObjects::Camera::CameraProjectionMode::Orthographics);
				}

				if (ImGui::Button("Perspective"))
				{
					maincamera->SetProjectionMode(GameObjects::Camera::CameraProjectionMode::Perspective);
				}

				if (maincamera->GetProjectionMode() == GameObjects::Camera::CameraProjectionMode::Perspective)
				{
					static float fov = maincamera->GetFov();
					ImGui::PushItemWidth(100);
					if (ImGui::SliderFloat("FOV", &fov, 0.0f, 110.0f))
					{
						maincamera->SetFov(fov);
					}

					ImGui::Text("Speed boost %.3f x", maincamera->GetSpeedBoost());
				}
				else
				{
					static float zoom = maincamera->GetZoom();
					ImGui::PushItemWidth(100);
					if (ImGui::SliderFloat("Zoom", &zoom, 0.0001f, 0.13f))
					{
						maincamera->SetZoom(zoom);
					}
				}

			}
			ImGui::EndMainMenuBar();
		}
	}
}