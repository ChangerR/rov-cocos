#ifndef __JUST_HTTPSTREAM_
#define __JUST_HTTPSTREAM_
#include <list>
#include <mutex>
typedef unsigned char u8;
class CCHttpStream {
public:
	CCHttpStream(int size_count,int cnt = 16);

	virtual ~CCHttpStream();
public:
	inline void lock();

	inline void unlock();
public:	
	int read(u8* buf,int len);

	int readline(u8* buf,int len);

	int skip(int len);

	int write(const u8* wbuf,int len,bool blocked = false);

	int left();

	void alloc_new_page(int cnt);

	void setFinish(bool v);

	bool isFinish();
private:
	int page_size;
	std::list<u8*>* free_page;
	std::list<u8*>* p_arr;
	int read_pos;
	int write_pos;
	u8* readpage_addr;
	u8* writepage_addr;
	std::mutex m_lock;
	bool b_finished;
};
#endif