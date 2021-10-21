#pragma once
#include "Base/Main/Common.h"
#include "Graphics/ImGui_Window.h"

namespace S2DE
{
	class S2DE_API Debug_Info : public ImGui_Window
	{
	public:
		Debug_Info();
		~Debug_Info();

		virtual void Render() final;

	};

}