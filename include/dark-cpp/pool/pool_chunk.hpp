#ifndef _DARK_CPP__POOL_POOL_CHUNK__HPP_
#define _DARK_CPP__POOL_POOL_CHUNK__HPP_
/*
	pool_chunk �ǹ� ���F�� һ�� �ȴ�� 
	ֻ���ԏ� �ȴ���� ��Ո �̶���С�� �YԴ chunk
	pool_chunk ���� ��Ո ���M

	pool_chunk �Ǟ� ���� ��Ոጷ� �󔵓��K �OӋ ��ʹ�� hashset deque ��ÿ�K chunk ��ӛ
	���ÿ�K chunk �� ������ 10��20�ֹ� �ȴ�
	�ʌ�� С�K �YԴ ʹ�� pool_chunk ���ܕ� ���� �^���ȴ�
*/

#include <boost/noncopyable.hpp>
#include <boost/unordered_set.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>

#include <deque>

#include <dark-cpp/thread/mutex.hpp>

namespace dark
{
	namespace pool
	{
		class pool_chunk
			: boost::noncopyable
		{
		protected:
			typedef void element_type;
			//ÿ�K chunk ��С
			std::size_t _chunk_size;
			//���� chunk
			std::deque<element_type*> _frees;

			//�ѷ��� chunk
			boost::unordered_set<element_type*> _mallocs;
	
			dark::thread::mutex_spt _mutex;
	
			virtual element_type* malloc_from_os()
			{
				return malloc(_chunk_size);
			}
			virtual void free_to_os(element_type* chunk)
			{
				free(chunk);
			}
			void * malloc_from_pool()
			{
				if(_frees.empty())
				{
					element_type * ret = malloc_from_os();
					if(ret)
					{
						_mallocs.insert(ret);
					}
					return ret;
				}

				element_type * ret =	_frees.back();
				_mallocs.insert(ret);

				_frees.pop_back();
				return ret;
			}
			void free_to_pool(element_type * chunk)
			{
				BOOST_AUTO(find,_mallocs.find(chunk));
				if(find == _mallocs.end())
				{
					return;
				}
				_mallocs.erase(find);

				_frees.push_back(chunk);
			}
		public:
			pool_chunk(std::size_t chunk_size,dark::thread::mutex_spt mutex = dark::thread::mutex_spt())
				:_chunk_size(chunk_size),_mutex(mutex)
			{
			}
			virtual ~pool_chunk()
			{
				purge_memory();
			}
			//���� �ȴ���� ���f�ȴ�
			inline std::size_t get_frees()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				return _frees.size();
			} 
			//���� �ȴ���� ʹ�õ� �ȴ�
			inline std::size_t get_mallocs()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);
				return _mallocs.size();
			}
			//�ăȴ���� ��Ո chunk
			inline void * malloc_chunk()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				return malloc_from_pool();
			}
			//�w߀ chunk ���ȴ��
			inline void free_chunk(element_type * chunk)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				free_to_pool(chunk);
			}
			
			//�� δʹ�� �ȴ� ߀�o os
			void release_memory()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				BOOST_FOREACH(element_type * chunk,_frees)
				{
					free_to_os(chunk);
				}
				_frees.clear();
			}
			//ͬ�� �� ���� n�� chunk �ڳ���
			void release_memory(std::size_t n)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				if(n >= _frees.size())
				{
					return;
				}
				for(std::size_t i=n;i<_frees.size();++i)
				{
					free_to_os(_frees[i]);
				}
				_frees.erase(_frees.begin()+n,_frees.end());
			}
			
			//������ �ȴ� ߀�o os 
			//�����r  �� �Ԅ��{��
			void purge_memory()
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				BOOST_FOREACH(element_type * chunk,_frees)
				{
					free_to_os(chunk);
				}
				_frees.clear();

				BOOST_FOREACH(element_type * chunk,_mallocs)
				{
					free_to_os(chunk);
				}
				_mallocs.clear();
			}

			//���� ָ� �Ƿ� ���ԃȴ��
			inline bool is_from(element_type *chunk)
			{
				dark::thread::scoped_mutex_lock lock(_mutex);

				return _mallocs.find(chunk) != _mallocs.end();
			}
		};
	};
};

#endif	//_DARK_CPP__POOL_POOL_CHUNK__HPP_

