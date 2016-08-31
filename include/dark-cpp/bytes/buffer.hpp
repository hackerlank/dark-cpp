#ifndef _DARK_CPP__BYTES_BUFFER__HPP_
#define _DARK_CPP__BYTES_BUFFER__HPP_

#include <boost/smart_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>

#include <list>

namespace dark
{
	namespace bytes
	{
		typedef boost::shared_array<char> bytes_t;

		typedef boost::function<bytes_t(int)> create_bytes_t_bf_t;
		//һ�K�B�m�� �ȴ� 
		//�ð� ���ǌ�һ�� char ���M�� ���b�T��
		class fragmentation_t
		{
		public:
			 static bytes_t create_bytes_t(int capacity)
			 {
				 return boost::shared_array<char>(new char[capacity]);
			 }
		protected:
			//��Ƭ����
			boost::shared_array<char> _data;
			//��Ƭ����
			std::size_t _capacity;
			//��Ч���� ƫ��
			std::size_t _offset;
			//��Ч���� ��С
			std::size_t _size;
		public:
			fragmentation_t(int capacity,create_bytes_t_bf_t create_bytes_t_f = create_bytes_t)
			{
				_capacity = capacity;
				_data = create_bytes_t_f(capacity);

				_offset = _size = 0;
			}

			std::size_t size() const
			{
				return _size;
			}
			
			//���� ���f ����
			std::size_t get_free()
			{
				return _capacity - _offset - _size;
			}
			//���딵�� ���،��H������
			std::size_t write(const char* bytes,std::size_t n)
			{
				std::size_t free = get_free();
				std::size_t need = n;
				if(need > free)
				{
					need = free;
				}
				memcpy(_data.get() + _offset + _size,bytes,need);
				_size += need;

				return need;
			}
			//�xȡ ���� ���،��H�xȡ ��
			//���xȡ�� ���� ���� �Ƴ� ���n�^
			std::size_t read(char* bytes,std::size_t n)
			{
				std::size_t need = n;
				if(need > _size)
				{
					need = _size;
				}

				memcpy(bytes,_data.get() + _offset,need);
				_size -= need;
				_offset += need;

				return need;
			}

			//ֻ copy ���� �� �h�����n�^
			std::size_t copy_to(char* bytes,std::size_t n)
			{
				std::size_t need = n;
				if (n > _size)
				{
					need = _size;
				}
				memcpy(bytes,_data.get() + _offset,need);

				return need;
			}
			//���^n�ֹ� copy
			std::size_t copy_to(std::size_t skip,char* bytes,std::size_t n)
			{
				if(skip >= _size)
				{
					return 0;
				}
				std::size_t offset = _offset + skip;
				std::size_t size = _size - skip;

				std::size_t need = n;
				if (need > size)
				{
					need = size;
				}
				memcpy(bytes,_data.get() + offset,need);

				return need;
			}
		};
		typedef boost::shared_ptr<fragmentation_t> fragmentation_spt; 
		typedef boost::function<fragmentation_spt(int)> create_fragmentation_spt_bf_t;
		//һ����� golang bytes.Buffer �� io ���n�^
		class buffer_t
			: boost::noncopyable
		{
		public:
			static fragmentation_spt create_fragmentation_spt(int capacity)
			{
				return boost::make_shared<fragmentation_t>(capacity);
			}
		protected:
			//��Ƭ ��С
			int _capacity;
			//�Ƿ� ���� ���� ͬ��
			boost::shared_ptr<boost::mutex> _mutex;
			//��Ƭ ����
			std::list<fragmentation_spt> _fragmentations;

			create_fragmentation_spt_bf_t _create_fragmentation_spt_f;
		public:
			buffer_t(int capacity = 1024/*�։K�L��*/,create_fragmentation_spt_bf_t create_fragmentation_spt_f=create_fragmentation_spt,bool sync = false)
			{
				_create_fragmentation_spt_f = create_fragmentation_spt_f;
				_capacity = capacity;
				if(sync)
				{
					_mutex = boost::make_shared<boost::mutex>();
				}
			}

			//��վ��� �h�����д��x����
			inline void reset()
			{
				if(_mutex)
				{
					boost::mutex::scoped_lock lock(*_mutex);
					_fragmentations.clear();
					return;
				}
				_fragmentations.clear();
			}
			//���� ���� ���x�ֹ���
			std::size_t size()
			{
				if(_mutex)
				{
					boost::mutex::scoped_lock lock(*_mutex);
					return unlock_size();
				}
				return unlock_size();
			}
		protected:
			std::size_t unlock_size()
			{
				std::size_t sum = 0;
				BOOST_FOREACH(fragmentation_spt fragmentation,_fragmentations)
				{
					sum += fragmentation->size();
				}
				return sum;
			}

		public:
			//�����n�^ copy ��ָ���ȴ� ���،��H copy�����L
			//��copy�Ĕ��� ������ ���n�^�� �h��
			//��� n > ���n�^ ���� �� ֻcopy ���n�^ ��t copy n �ֹ�����
			std::size_t copy_to(char* bytes,std::size_t n)
			{
				if(_mutex)
				{
					boost::mutex::scoped_lock lock(*_mutex);
					return unlock_copy_to(bytes,n);
				}
				return unlock_copy_to(bytes,n);
			}
			std::size_t copy_to(std::size_t skip,char* bytes,std::size_t n)
			{
				if(_mutex)
				{
					boost::mutex::scoped_lock lock(*_mutex);
					return unlock_copy_to(skip,bytes,n);
				}
				return unlock_copy_to(skip,bytes,n);
			}
		protected:
			std::size_t unlock_copy_to(char* bytes,std::size_t n)
			{
				std::size_t sum = 0;
				BOOST_FOREACH(fragmentation_spt fragmentation,_fragmentations)
				{
					std::size_t count = fragmentation->copy_to(bytes,n);
					n -= count;
					bytes += count;
					sum += count;

					if(n < 1)
					{
						break;
					}
				}
				return sum;
			}
			std::size_t unlock_copy_to(std::size_t skip,char* bytes,std::size_t n)
			{
				std::size_t sum = 0;
				BOOST_FOREACH(fragmentation_spt fragmentation,_fragmentations)
				{
					std::size_t need_skip = skip;
					if(need_skip)
					{
						std::size_t size = fragmentation->size();
						if(need_skip > size)
						{
							need_skip = size;
						}
						skip -= need_skip;
					}

					std::size_t count = fragmentation->copy_to(need_skip,bytes,n);
					n -= count;
					bytes += count;
					sum += count;

					if(n < 1)
					{
						break;
					}
				}
				return sum;
			}

		public:
			std::size_t write(const char* bytes,std::size_t n)
			{
				if(_mutex)
				{
					boost::mutex::scoped_lock lock(*_mutex);
					return unlock_write(bytes,n);
				}
				return unlock_write(bytes,n);
			}
		protected:
			std::size_t unlock_write(const char* bytes,std::size_t n)
			{
				std::size_t sum = 0;
				while (n)
				{
					fragmentation_spt fragmentation = get_write_fragmentation();
					int count = fragmentation->write(bytes,n);
					n -= count;
					bytes += count;
					sum += count;
				}

				return sum;
			}
			//���� �Ɍ��� ��Ƭ
			fragmentation_spt get_write_fragmentation()
			{
				if (!_fragmentations.empty())
				{
					fragmentation_spt fragmentation = _fragmentations.back();
					if(fragmentation->get_free())
					{
						return fragmentation;
					}
				}
				fragmentation_spt fragmentation = _create_fragmentation_spt_f(_capacity);
				_fragmentations.push_back(fragmentation);
				return fragmentation;
			}
		public:
			std::size_t read(char* bytes,std::size_t n)
			{
				if(_mutex)
				{
					boost::mutex::scoped_lock lock(*_mutex);
					return unlock_read(bytes,n);
				}
				return unlock_read(bytes,n);
			}
		protected:
			std::size_t unlock_read(char* bytes,std::size_t n)
			{
				std::size_t sum = 0;

				while(n)
				{
					fragmentation_spt fragmentation = get_read_fragmentation();
					std::size_t count = fragmentation->read(bytes,n);
					n -= count;
					bytes += count;
					sum += count;
				}
				remove_no_read_fragmentation();

				return sum;
			}
			//���� ���x�� ��Ƭ
			fragmentation_spt get_read_fragmentation()
			{
				while(!_fragmentations.empty())
				{
					fragmentation_spt fragmentation = _fragmentations.front();
					if (fragmentation->size() < 1)
					{
						_fragmentations.pop_front();
						continue;
					}
					return fragmentation;
				}

				return fragmentation_spt();
			}
			//�h�� �o�������x�� ��Ƭ
			void remove_no_read_fragmentation()
			{
				while(!_fragmentations.empty())
				{
					fragmentation_spt fragmentation = _fragmentations.front();
					if(fragmentation->size())
					{
						break;
					}
					_fragmentations.pop_front();
				}
			}
		};
		typedef boost::shared_ptr<buffer_t> buffer_spt;
	};
};
#endif	//_DARK_CPP__BYTES_BUFFER__HPP_