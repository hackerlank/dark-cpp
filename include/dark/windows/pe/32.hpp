#ifndef _DARK_CPP__WINDOWS_PE_32__HPP_
#define _DARK_CPP__WINDOWS_PE_32__HPP_


namespace dark
{
	namespace windows
	{
#define DARK_CPP_WINDOWS_PE_MAGIC_MZ 0x5A4D
#define DARK_CPP_WINDOWS_PE_MAGIC_PE 0x4550

		typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
			WORD   e_magic;                     // Magic number
			WORD   e_cblp;                      // Bytes on last page of file
			WORD   e_cp;                        // Pages in file
			WORD   e_crlc;                      // Relocations
			WORD   e_cparhdr;                   // Size of header in paragraphs
			WORD   e_minalloc;                  // Minimum extra paragraphs needed
			WORD   e_maxalloc;                  // Maximum extra paragraphs needed
			WORD   e_ss;                        // Initial (relative) SS value
			WORD   e_sp;                        // Initial SP value
			WORD   e_csum;                      // Checksum
			WORD   e_ip;                        // Initial IP value
			WORD   e_cs;                        // Initial (relative) CS value
			WORD   e_lfarlc;                    // File address of relocation table
			WORD   e_ovno;                      // Overlay number
			WORD   e_res[4];                    // Reserved words
			WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
			WORD   e_oeminfo;                   // OEM information; e_oemid specific
			WORD   e_res2[10];                  // Reserved words

			//ӛ� pe �^ ƫ��
			LONG   e_lfanew;                    // File address of new exe header
		  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

		typedef struct _IMAGE_EXPORT_DIRECTORY {
			DWORD   Characteristics;			//һ��Ϊ0δʹ��
			DWORD   TimeDateStamp;				//ʱ���
			WORD    MajorVersion;			//δ��
			WORD    MinorVersion;			//δ��
			DWORD   Name;				//ָ�� �������ļ��� ��\0����ansi
			DWORD   Base;				//����������С���
			DWORD   NumberOfFunctions;			//����������Ŀ ��ʵ�������-��С���+1
			DWORD   NumberOfNames;			//������ ���������� ��\0����ansi
			DWORD   AddressOfFunctions;     		// RVA from base of image	������ַ[NumberOfFunctions] ����û�õ������洢Ϊ0
			DWORD   AddressOfNames;         		// RVA from base of image	ָ�� ��������ַ����[NumberOfNames] ��ָ��
			DWORD   AddressOfNameOrdinals;  		// RVA from base of image	��������Ӧ��������WORD [NumberOfNames]
		} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
	};


};

#endif	//_DARK_CPP__WINDOWS_PE_32__HPP_