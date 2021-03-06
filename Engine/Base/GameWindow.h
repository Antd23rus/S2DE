#pragma once
#include "Base/Main/Common.h"

#define S2DE_DEFAULT_WINDOW_WIDTH 1280
#define S2DE_DEFAULT_WINDOW_HEIGHT 768

namespace S2DE::Core
{
	class S2DE_API GameWindow
	{
	public:
		GameWindow();
		~GameWindow();

		//Create a game window function with default setup
		bool							Create(HINSTANCE hinstance, std::string name = std::string(), std::uint32_t w = S2DE_DEFAULT_WINDOW_WIDTH, std::uint32_t h = S2DE_DEFAULT_WINDOW_HEIGHT,
												std::int32_t top = 100, std::int32_t left = 100, bool Show_Cursor = false, bool Fullscreen = false);

		//Can be used for attaching render window to winform elements
		bool						   AttachChildWindow(HINSTANCE hinstance, std::uint32_t ClientRectangleX, std::uint32_t ClientRectangleY, 
															std::uint32_t Width, std::uint32_t Height, HWND Handle);
		//Close game window
		void						   Close();

		inline std::string             GetName() const { return m_Name; }
		inline std::uint32_t           GetWidth()  const { return m_Width; }
		inline std::uint32_t           GetHeight() const { return m_Height; }
		inline std::uint32_t           GetWidthFixed()  const {  RECT rc; GetClientRect(m_HWND, &rc); return rc.right - rc.left; }
		inline std::uint32_t           GetHeightFixed() const { RECT rc; GetClientRect(m_HWND, &rc); return  rc.bottom - rc.top; }
		inline std::int32_t            GetLeft()   const { return m_Left; }
		inline std::int32_t            GetTop()    const { return m_Top; }
		inline HWND&                   GetHWND() { return m_HWND; }
		inline HINSTANCE               GetInstance() const { return m_Instance; };
		inline RECT                    GetClientRes() const;
		inline bool					   isActive() const { return GetActiveWindow() == NULL ? false : true; }
		inline bool                    isShowingCursor() const { return m_ShowCursor; }
		inline bool                    isClosing() const { return m_isClosing; }
		inline bool                    isChild() const { return m_isChild; }
		inline bool					   isFullscreen() const { return m_Fullscreen; }
		inline static GameWindow*	   GetInstanceWindow() { return m_InstanceWindow; }

		void						   SetWidht(std::uint32_t w);
		void						   SetHeight(std::uint32_t h);
		void						   SetLeft(std::int32_t left);
		void						   SetTop(std::int32_t top);
		void						   SetFullscreen(bool fullscreen);
		void						   SetMouseVisible(bool visible);

		//Set width and height by client resoulution
		void						   SetClientScreenRes();
									   
		bool						   ProcessMessage();
		LRESULT CALLBACK			   WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		std::uint32_t				   m_Width;
		std::uint32_t				   m_Height;
		std::int32_t				   m_Top;
		std::int32_t				   m_Left;
		HWND						   m_HWND;
		HINSTANCE					   m_Instance;
		bool						   m_Fullscreen;
		bool						   m_ShowCursor;
		bool						   m_isChild;
		bool						   m_isClosing;
		LPCSTR						   m_Name;
		WINDOWPLACEMENT				   m_WindowPlacement;
		WNDCLASSEX					   m_WindowClass;
		static GameWindow*			   m_InstanceWindow;
									   
		//Temp vars for changing from fullscreen to window mode 
		std::uint32_t				   m_PreviousWidth;
		std::uint32_t				   m_PreviousHeight;

	private:						   
		void						   OnPaint(HWND hWnd);
		void						   OnMoving();
		void						   OnSizing();
		void						   OnClose();
		void						   OnDestroy();
		void						   OnSizeChanged();
									   
		static LRESULT CALLBACK		   InstanceWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
}