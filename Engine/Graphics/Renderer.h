#pragma once
#include "Base/Main/Common.h"
#include "Math/Color.h"

#include "Libs/imgui/imgui.h"
#include "Libs/imgui/imgui_impl_win32.h"
#include "Libs/imgui/imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//This macro enable debug layer, disable shader optimization and enable shader debug mode
#define S2DE_DEBUG_RENDER_MODE 

//Name of default font in resources  
#define S2DE_DEFAULT_FONT_NAME "default"

namespace S2DE
{
	namespace Editor
	{
		class EditorCenterCursor;
	}
}

namespace S2DE::Render
{
	enum class RenderFillMode : std::int32_t
	{
		Solid = D3D11_FILL_MODE::D3D11_FILL_SOLID,
		Wireframe = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME,
	};

	class S2DE_API Renderer
	{
	public:
		Renderer();
		~Renderer();

		// Initialize all renderer components.
		bool								Create();
		// Recreate: depthStencil, targetView, etc. Resize buffers and update viewport.
		bool								Reset();
		// Destroy all renderer components
		void								Destroy();
		// Update render and presents a rendered image
		void								Render();
		// Set new fill mode
		void								SwitchFillMode(RenderFillMode mode);
		// Set new back color
		void								SetBackColor(Math::Color<float> color);
		// Set disabled depth state 
		void								TurnZBufferOff();
		// Set enabled depth state 
		void								TurnZBufferOn();
		// Set enabled blend state 
		void								TurnOnAlphaBlending();
		// Set disabled blend state 
		void								TurnOffAlphaBlending();
		// Toggle ImGui Windows visible (Works only for windows in storage)
		void								ToggleImGuiWindowsVisible();
		// Toggle ImGui Demo window
		void								ToggleImGuiDemoWindowVisible();

		inline ID3D11Device*				GetDevice() { return m_device; }
		inline ID3D11DeviceContext*			GetContext() { return m_context; }
		inline IDXGISwapChain*				GetSwapChain() { return m_swapChain; }
		inline ID3D11RenderTargetView*		GetRenderTargetView() { return m_targetView; }
		inline bool							GetVsync() const { return m_vsync; }
		inline D3D11_VIEWPORT				GetViewport() const { return m_viewport; }
		inline ID3D11Texture2D*				GetDepthStencilBuffer() { return m_depthStencilBuffer; }
		inline ID3D11DepthStencilView*		GetDepthStencilView() { return    m_depthStencilView; }
		inline ID3D11ShaderResourceView*	GetFramebufferShaderResource() const { return m_frameBufferShaderResourceView; }
		inline ID3D11Texture2D*				GetFramebufferTextureData() const { return m_frameBufferData; }
		inline ID3D11RasterizerState*		GetRasterStateCullDisabled() const { return m_rasterStateCullDisabled; }
		inline ID3D11RasterizerState*		GetRasterStateCullEnabled() const { return m_rasterStateCullEnabled; }

	private:
		// Initialize ImGui Library
		bool								InitImGui();
		// Update ImGui windows saved in storage
		void								UpdateImGuiWindows();
		// Render ImGui windows saved in storage
		void								RenderImGui();
		// Load custom theme
		void								LoadCustomImguiTheme();
		// Destroy ImGui Library
		void								DestroyImGui();
		// Create device with correct driver type with swap chain 
		bool								CreateDeviceAndSwapChain();
		// Create DepthStencil and RenderTarget
		bool								CreateDepthStencil();
		// Create RenderTarget and frame buffer
		bool								CreateRenderTarget();
		// Update viewport 
		void								UpdateViewport();
		// Clear 
		void								Clear();
		// Presents a rendered image to the user.
		void								End();
		// Create rasterizer
		bool								CreateRasterizerState();
		// Create blend state
		bool								CreateBlendState();
		// Create framebuffer texture by back buffer
		bool								CreateFramebufferTexture(ID3D11Texture2D* sw_buff);
		// Update framebuffer
		void								UpdateFramebufferShaderResource();
		// Create debug layer if we are in debug build and added special macro S2DE_DEBUG_RENDER_MODE
		void								CreateDebugLayer();
		// Capture messages from debug layer and print it to logger
		bool								CaptureMessages();
		// Create engine window and editor ui if it's we are have editor flag
		void								CreateEngineWindowsAndEditorUI();

		IDXGISwapChain*				m_swapChain;
		ID3D11Device*				m_device;
		ID3D11DeviceContext*		m_context;
		ID3D11RenderTargetView*		m_targetView;
		ID3D11RasterizerState*		m_rasterStateCullDisabled;
		ID3D11RasterizerState*		m_rasterStateCullEnabled;		
		ID3D11Texture2D*			m_backBuffer;
		ID3D11Texture2D*			m_depthStencilBuffer;
		ID3D11DepthStencilView*		m_depthStencilView;
		ID3D11DepthStencilState*	m_depthStateEnabled;
		ID3D11DepthStencilState*	m_depthStateDisabled;
		bool						m_vsync;
		D3D11_VIEWPORT				m_viewport;
		std::uint32_t				m_deviceFlags;
		Math::Color<float>			m_clearColor;
		RenderFillMode				m_fillMode;

		ID3D11Texture2D*			m_frameBufferData;
		ID3D11ShaderResourceView*	m_frameBufferShaderResourceView;
		
		ID3D11Debug*				m_d3dDebug;
		ID3D11InfoQueue*			m_d3dInfoQueue;


		ID3D11BlendState*			m_blendStateOn; //TODO: Many modes, and use not like this
		ID3D11BlendState*			m_blendStateOff; 

		bool						m_showImguiWindows;
		bool						m_showImguiDemoWindow;

		class Editor::EditorCenterCursor*				m_editorCenterCursor;
		class ImGui_Window*								m_editorToolStrip;
		std::vector<std::pair<std::string, class ImGui_Window*>>	m_windowsStorage;

	public:  
		inline class ImGui_Window*	GetImGui_Window(std::string name) const;
		ImGui_Window*				AddImGuiWindow(std::string name, ImGui_Window* wnd, bool visible = false);
		void						DeleteImGuiWindow(std::string name);
	};
}