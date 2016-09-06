#ifndef _DARK_CPP__WINDOWS_HACKER_HOOK__HPP_
#define _DARK_CPP__WINDOWS_HACKER_HOOK__HPP_

#include <boost/smart_ptr.hpp>

namespace dark
{
	namespace windows
	{
		namespace hacker
		{
			class hook_code_t
			{
			public:
				//hook λ��
				DWORD address;

				//�º��� jmp ��ԭ����λ��
				DWORD jump;
	
				//ԭ��������Q�ֹ��L��
				DWORD size;

				//ԭ��������Q���a
				boost::shared_array<BYTE> code;

				hook_code_t()
				{
					address	=	jump	=	size	=	0;
				}
				hook_code_t(const hook_code_t& copy)
				{
					address	=	copy.address;
					jump	=	copy.jump;
					size	=	copy.size;
					code	=	copy.code;
				}
				hook_code_t& operator=(const hook_code_t& copy)
				{
					address	=	copy.address;
					jump	=	copy.jump;
					size	=	copy.size;
					code	=	copy.code;
					return *this;
				}
			};
			typedef boost::shared_ptr<hook_code_t> hook_code_ptr_t;

			class hook
			{
			public:
				inline static hook_code_ptr_t inline_hook(const DWORD address,		//��hookλ��
					const BYTE* code,	//hook ���a
					DWORD size	//hook���a�L��
					)
				{
					return inline_hook((PVOID)address,code,size);
				}
				static hook_code_ptr_t inline_hook(const PVOID address,		//��hookλ��
					const BYTE* code,	//hook ���a
					DWORD size	//hook���a�L��
					)
				{
					if(!address
						|| !code
						|| !size
						)
					{
						return hook_code_ptr_t();
					}
					MEMORY_BASIC_INFORMATION mbi_thunk;  
					//��ԃ���Ϣ  
					VirtualQuery(address, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
	
					//��׃퓱��o���Ԟ��x��
					if(!VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize,PAGE_EXECUTE_READWRITE, &mbi_thunk.Protect))
					{
						return hook_code_ptr_t();
					}
					//���汻hook���a
					hook_code_ptr_t code_ptr = boost::make_shared<hook_code_t>();
					code_ptr->code	=	boost::shared_array<BYTE>(new BYTE[size]);
					code_ptr->size	=	size;
					code_ptr->address	=	(DWORD)address;
					memcpy(code_ptr->code.get(),address,size);

					//����hook���a
					memcpy(address,code,size);

					//�֏�퓱��o����
					VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize, mbi_thunk.Protect, NULL);
					return code_ptr;
				}
				
				//�֏�hook���a
				static bool un_inline_hook(hook_code_ptr_t code_ptr)
				{
					if(!code_ptr
						)
					{
						return false;
					}
					PVOID address = (PVOID)code_ptr->address;

					MEMORY_BASIC_INFORMATION mbi_thunk;  
					//��ԃ���Ϣ  
					VirtualQuery(address, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
	
					//��׃퓱��o���Ԟ��x��
					if(!VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize,PAGE_EXECUTE_READWRITE, &mbi_thunk.Protect))
					{
						return false;
					}

					//�֏�hook���a
					memcpy((void*)code_ptr->address,code_ptr->code.get(),code_ptr->size);

					//�֏�퓱��o����
					VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize, mbi_thunk.Protect, NULL);
					return true;
				}


				inline static hook_code_ptr_t inline_hook(const DWORD address,		//��hookλ��
					const DWORD myaddress,	//�Զ��x���� λ��
					DWORD size				//hook���a�L��
					)
				{
					return inline_hook((PVOID)address,(PVOID)myaddress,size);
				}
				static hook_code_ptr_t inline_hook(const PVOID address,		//��hookλ��
					const PVOID myaddress,	///�Զ��x���� λ��
					DWORD size				//hook���a�L��
					)
				{
					if(!address
						|| !myaddress
						|| !size
						)
					{
						return hook_code_ptr_t();
					}
					MEMORY_BASIC_INFORMATION mbi_thunk;  
					//��ԃ���Ϣ  
					VirtualQuery(address, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
	
					//��׃퓱��o���Ԟ��x��
					if(!VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize,PAGE_EXECUTE_READWRITE, &mbi_thunk.Protect))
					{
						return false;
					}

					hook_code_ptr_t code_ptr = boost::make_shared<hook_code_t>();
					//�º������D��ԭ������ַ
					code_ptr->jump	=	(DWORD)address + size;

					//���汻hook���a
					code_ptr->code	=	boost::shared_array<BYTE>(new BYTE[size]);
					code_ptr->size	=	size;
					code_ptr->address	=	(DWORD)address;
					memcpy(code_ptr->code.get(),address,size);

					//hook code
					__asm
					{
						push ebx;

						mov eax,myaddress;
						sub eax,address;
						sub eax,5;

						mov ebx,address;
						mov byte ptr [ebx],0xE9;
						mov [ebx+1],eax;

						pop ebx;
					}

					//�֏�퓱��o����
					VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize, mbi_thunk.Protect, NULL);
					return code_ptr;
				}
			};
		};
	};
};
#endif	//_DARK_CPP__WINDOWS_HACKER_HOOK__HPP_