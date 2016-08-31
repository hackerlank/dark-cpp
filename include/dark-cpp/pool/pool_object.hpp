#ifndef _DARK_CPP__POOL_POOL_OBJECT__HPP_
#define _DARK_CPP__POOL_POOL_OBJECT__HPP_
/*
	pool_object �ǹ� �� pool_chunk ֮�� ���F�� һ�� c++ ����ȴ�� 
*/

#include <dark-cpp/pool/pool_chunk.hpp>
namespace dark
{
	namespace pool
	{
		template<typename T>
		class pool_object
			: boost::noncopyable
		{
		protected:
			typedef T element_type;
			
			pool_chunk _pool;
			dark::thread::mutex_spt _mutex;

			inline element_type * malloc_from_pool()
			{
				return (element_type*)_pool.malloc_chunk();
			}
			inline void free_to_pool(element_type * chunk)
			{
				_pool.free_chunk(chunk);
			}
		public:
			pool_object(dark::thread::mutex_spt mutex = dark::thread::mutex_spt())
				:_mutex(mutex),_pool(sizeof(T))
			{
			}
			virtual ~pool_object()
			{
				purge_memory();
			}
			//���� �ȴ���� ���f�ȴ�
			inline std::size_t get_frees()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				return _pool.get_frees();
			} 
			//���� �ȴ���� ʹ�õ� �ȴ�
			inline std::size_t get_mallocs()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				return _pool.get_mallocs();
			}

			//�� δʹ�� �ȴ� ߀�o os
			inline void release_memory()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				_pool.release_memory();
			}
			//ͬ�� �� ���� n�� chunk �ڳ���
			inline void release_memory(std::size_t n)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				_pool.release_memory(n);
			}
			//������ �ȴ� ߀�o os 
			//�����r  �� �Ԅ��{��
			//�ѱ���Ո�� object ���������������{��
			inline void purge_memory()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				_pool.purge_memory();
			}
			//���� ָ� �Ƿ� ���ԃȴ��
			inline bool is_from(element_type *chunk)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				return _pool.is_from(chunk);
			}

			
			//�w߀ object ���ȴ��
			inline void destory(element_type * chunk)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				chunk->~T();
				_pool.free_chunk(chunk);
			}
			//�ăȴ�� ��Ո object
			element_type * construct()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0>
			element_type * construct(const T0 & a0)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1>
			element_type * construct(const T0 & a0,const T1 & a1)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2,typename T3>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2,const T3 & a3)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2,a3); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2,typename T3,typename T4>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2,const T3 & a3,const T4 & a4)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2,a3,a4); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2,const T3 & a3,const T4 & a4,const T5 & a5)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2,a3,a4,a5); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2,const T3 & a3,const T4 & a4,const T5 & a5,const T6 & a6)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2,a3,a4,a5,a6); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2,const T3 & a3,const T4 & a4,const T5 & a5,const T6 & a6,const T7 & a7)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2,a3,a4,a5,a6,a7); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2,const T3 & a3,const T4 & a4,const T5 & a5,const T6 & a6,const T7 & a7,const T8 & a8)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2,a3,a4,a5,a6,a7,a8); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}
			template <typename T0,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
			element_type * construct(const T0 & a0,const T1 & a1,const T2 & a2,const T3 & a3,const T4 & a4,const T5 & a5,const T6 & a6,const T7 & a7,const T8 & a8,const T9 & a9)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				element_type * const ret = malloc_from_pool();
				if (ret == 0)
					return ret;
				try { new (ret) element_type(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9); }
				catch (...) { free_to_pool(ret); throw; }
				return ret;
			}

		};
	};
};

#endif	//_DARK_CPP__POOL_POOL_OBJECT__HPP_

