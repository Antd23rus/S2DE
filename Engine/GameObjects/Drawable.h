#pragma once
#include "GameObjects/GameObject.h"

namespace S2DE::GameObjects
{
	//Base class for drawable objects

	class S2DE_API Drawable : public GameObject
	{
	public:
		Drawable() = default;
		virtual ~Drawable() { }

		virtual void				 UpdateShader() { }
		virtual void				 UpdateTexture() { }
		virtual bool				 LoadTexture(std::string name, bool unload_texture = false, bool auto_load_texture = true) { return false; }
		virtual bool				 LoadShader(std::string name) { return false; }

	protected:
		virtual void				 OnPositionChanged()  override { }
		virtual void				 OnRotationChanged()  override { }
		virtual void				 OnScaleChanged()     override { }
		virtual void				 OnCreate() override { }
		virtual void				 OnDestroy() override { }
		virtual void				 OnRender()  override { }
		virtual void				 OnRenderImGUI() override { }
		virtual void				 OnUpdate(float DeltaTime) override { }
		virtual void				 OnUpdateInput() override { }
		virtual bool 				 CheckOnIntersection() override { return false; }

		virtual void				 CreateVertexBuffer() { }
		virtual void				 CreateIndexBuffer() { }
		virtual void				 SetDefaultShader() { }
		virtual void				 SetDefaultTexture() { }
	};
}

