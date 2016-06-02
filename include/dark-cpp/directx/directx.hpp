#ifndef _DARK_CPP__DIRECTX_DIRECTX__HPP_
#define _DARK_CPP__DIRECTX_DIRECTX__HPP_

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>

namespace dark
{
	namespace directx
	{
//���ڶ��x
#define DARK_DIRECTX_WINDOW_TITLE	L"dark directx window"
#define DARK_DIRECTX_WINDOW_CLASS	L"dark directx window class"

		//d3dx �O�� ���b
		class device
		{
		protected:
			//d3dx �O��
			IDirect3DDevice9*		_device;

			//���� ��/�� 
			int _width;
			int _height;
			int _fps;
		public:
			inline IDirect3DDevice9* device9()
			{
				return _device;
			}
			inline int width()
			{
				return _width;
			}
			inline int height()
			{
				return _height;
			}
			inline int fps()
			{
				return _fps;
			}

			bool init_device(HINSTANCE hInstance		//���ó�����
				,HICON hIcon										//�D��
				,HICON hIconSm										//�D��
				,WNDPROC window_fun									//���ں���
				,const wchar_t* title = DARK_DIRECTX_WINDOW_TITLE	//���ژ��}
				,int fps = 60
				,int width = 640									//���ڌ�
				,int height = 480									//���ڸ�
				,bool window = true									//����ģʽ ���� ȫ��
				,D3DDEVTYPE device_type= D3DDEVTYPE_HAL				//ʹ�� Ӳ��/ܛ�� �\��
			)
			{
				_fps = fps;
				_height = height;
				_width = width;

				//�]�Դ����
				WNDCLASSEX wcex;

				wcex.cbSize = sizeof(WNDCLASSEX);

				wcex.style			= CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc	= window_fun;
				wcex.cbClsExtra		= 0;
				wcex.cbWndExtra		= 0;
				wcex.hInstance		= hInstance;
				wcex.hIcon			= hIcon;
				wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
				wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
				wcex.lpszMenuName	= 0;//MAKEINTRESOURCE(IDC_DIRECT3DTEST);;
				wcex.lpszClassName	= DARK_DIRECTX_WINDOW_CLASS;
				wcex.hIconSm		= hIconSm;

				RegisterClassEx(&wcex);


				//�������� �K�@ʾ
				DWORD style	=	WS_EX_TOPMOST;				//플Ӵ���
				style	|=	WS_SYSMENU | WS_MINIMIZEBOX;	//��С�� �P�] ���o
				HWND hwnd	=	CreateWindow(DARK_DIRECTX_WINDOW_CLASS, title
					,style
					,CW_USEDEFAULT, 0, width, height
					,0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 
				if(!hwnd)
				{
					return false;
				}
				ShowWindow(hwnd, SW_SHOW);
				UpdateWindow(hwnd);

				//�_ʼ��ʼ�� Direct3D
				//1	�@ȡIDirect3D9�ӿ� ָ�
				IDirect3D9* _direct	=	Direct3DCreate9(D3D_SDK_VERSION);
	
				if(!_direct)
				{
					return false;
				}

				//2 �@ȡӲ������
				D3DCAPS9 caps;
				_direct->GetDeviceCaps(D3DADAPTER_DEFAULT, device_type, &caps);

				int vp = 0;
				if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
				{
					vp	=	D3DCREATE_HARDWARE_VERTEXPROCESSING;
				}
				else
				{
					vp	=	D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				}

				//3	��� D3DPRESENT_PARAMETERS
				D3DPRESENT_PARAMETERS d3dpp;
				d3dpp.BackBufferWidth            = width;
				d3dpp.BackBufferHeight           = height;
				d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
				d3dpp.BackBufferCount            = 1;
				d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
				d3dpp.MultiSampleQuality         = 0;
				d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
				d3dpp.hDeviceWindow              = hwnd;
				d3dpp.Windowed                   = window;
				d3dpp.EnableAutoDepthStencil     = true; 
				d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
				d3dpp.Flags                      = 0;
				d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
				d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;


				//4 ���� �O��
				HRESULT hr	=	_direct->CreateDevice(D3DADAPTER_DEFAULT
					,device_type
					,hwnd
					,vp
					,&d3dpp
					,&_device
					);
				if(FAILED(hr))
				{
					//ʹ��16λ�� ��ԇ
					d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
					hr	=	_direct->CreateDevice(D3DADAPTER_DEFAULT
						,device_type
						,hwnd
						,vp
						,&d3dpp
						,&_device
						);
					if(FAILED(hr))
					{
						_direct->Release();
						return false;
					}
				}

				if(!init())
				{
					_direct->Release();
					return false;
				}
				//ጷ�IDirect3D9�ӿ� ָ�
				_direct->Release();
				return true;
			}
			device()
			{
				_fps = 60;
				_device = NULL;
			}
			virtual ~device()
			{
				release();
			}

			//�M����Ϣѭ�h
			void msg_loop()
			{
				MSG msg;
				memset(&msg,0,sizeof(MSG));
				DWORD time_begin = GetTickCount(); 
				DWORD time_now;
				DWORD time_interval;
				DWORD fps_n = _fps / 10;
				DWORD fps_sum = 0;
				DWORD fps_now = time_begin;
				while(msg.message != WM_QUIT)
				{
					if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
					}
					
					//Ӌ�� �����L�u �r�g�g��
					time_now		=	GetTickCount(); 
					time_interval	=	time_now - time_begin;
					run(time_interval);
					_draw(time_interval);
					time_begin = time_now;	

					//ͬ�� fps
					++fps_sum;
					if(fps_sum == fps_n)
					{
						DWORD wait = GetTickCount() - fps_now;
						if(wait < 100)
						{
							Sleep(100  - wait);
						}
						fps_now = GetTickCount();
						fps_sum = 0;
					}
				}
			}

		protected:
			void _draw(const DWORD time_interval)
			{
				//�������
				_device->Clear(0,0,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ,0xffffffff,1.0f,0);
				//�L�u����
				_device->BeginScene();
				{
					draw(time_interval);
				}
				_device->EndScene();

				//���Q���n�^�� ��Ļ
				_device->Present(0,0,0,0);
			}

			//��ԓ���d�˺��� ���e������ �YԴጷŲ��� ጷ��YԴ
			virtual void release()
			{
				if(_device)
				{
					_device->Release();
					_device	=	NULL;
				}
			}
			//��ԓ���d�˺��� ���e������ ��ʼ�� �O��
			//���� false ���� init_device ʧ��
			virtual bool init()
			{
				return true;
			}
			//��ԓ���d�˺��� �Ը�׃�����L�u time_interval �Ǿ��x�ϴ��L�u�� �r�g�g��
			virtual void draw(const DWORD time_interval) = 0;
			//��ԓ���d�˺��� �ڴˈ��� �[�� �]��߉݋
			virtual void run(const DWORD time_interval) = 0;
		};
	};
};

#endif	//_DARK_CPP__DIRECTX_DIRECTX__HPP_