// test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <dark-cpp/bytes/buffer.hpp>

#include <gtest/gtest.h>
#include <boost/smart_ptr.hpp>

#ifdef _DEBUG
#pragma comment(lib,"gtest-mdd.lib")
#else
#pragma comment(lib,"gtest-md.lib")
#endif


int _tmain(int argc, _TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	int rs = RUN_ALL_TESTS();
	
	std::system("pause");
	return rs;
}


TEST(BytesBufferTest, HandleNoneZeroInput)
{
	{
		//������ ����
		dark::bytes::buffer_t buf(8);
		std::string str = "0123456789";
		//���딵��
		EXPECT_EQ(buf.write(str.data(),str.size()),str.size());
		//�@ȡ ���x���� ��С
		EXPECT_EQ(buf.size(),str.size());

		char bytes[11] = {0};
		//copy ���x ���n�^
		EXPECT_EQ(buf.copy_to(0,bytes,10),str.size());
		EXPECT_STREQ(bytes,str.c_str());

		memset(bytes,0,sizeof(bytes));
		int offset = 0;
		int len = 10;
		while (true)
		{
			int need = 3;
			if (need > len)
			{
				need = len;
			}

			//�xȡ ���n�^
			int n = buf.read(bytes + offset, need);
			if (!n)
			{
				//�o ���� ���x 
				EXPECT_STREQ(bytes,str.c_str());
				break;
			}
			offset += n;
			len -= n;
		}

		EXPECT_EQ(buf.size(),0);
	}
	{
		
		dark::bytes::buffer_t buf(8);
		std::string str = "0123456789abcdefghijklmnopqrstwxz";
		EXPECT_EQ(buf.write(str.data(),str.size()),str.size());

		std::size_t size = str.size();
		boost::shared_array<char> b(new char[size]);
		std::size_t n = buf.copy_to(b.get(),size);
		EXPECT_EQ(std::string(b.get(),n),str);

		std::size_t pos = 3;
		//���^�ֹ� copy
		n = buf.copy_to(pos,b.get(),size);
		EXPECT_EQ(std::string(b.get(),n),str.substr(pos));
	}
}