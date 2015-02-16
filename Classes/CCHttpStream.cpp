#include "CCHttpStream.h"
#include <thread>
#include "cocos2d.h"
CCHttpStream::CCHttpStream(int size_count,int cnt) :page_size(size_count),b_finished(false){

	free_page = new std::list<u8*>();
	p_arr = new std::list<u8*>();

	p_arr->push_back(new u8[size_count]);
	for(int i = 0;i < cnt - 1;i ++) {
		free_page->push_back(nullptr);
	}

	read_pos = 0;
	write_pos = 0;

	readpage_addr = *(p_arr->begin());
	writepage_addr = readpage_addr;

}

CCHttpStream::~CCHttpStream() {

	lock();

	for(std::list<u8*>::iterator p = free_page->begin();p != free_page->end();p++) {
		if (*p)
			delete [] (*p);
	}

	for(std::list<u8*>::iterator p = p_arr->begin();p != p_arr->end();p++) {
		delete [] (*p);
	}

	delete free_page;
	delete p_arr;

	unlock();
	
}


void CCHttpStream::lock() {
	m_lock.lock();
}

void CCHttpStream::unlock() {
	m_lock.unlock();
}
	
int CCHttpStream::read(u8* buf,int len) {

	int read_len = 0;
	int span = 0;

	lock();
	while(len) {

		span = (readpage_addr == writepage_addr ? write_pos : page_size) - read_pos;
		span = span > len ? len:span;

		if (span <= 0)
		{
			if(readpage_addr == writepage_addr)
				break;

			read_pos = 0;
			memset(readpage_addr,0,page_size);

			free_page->push_back(readpage_addr);
			p_arr->pop_front();

			readpage_addr = *(p_arr->begin());
			continue;
		}

		memcpy(buf,readpage_addr + read_pos,span);
		read_pos += span;
		read_len += span;
		buf += span;
		len -= span;
	}

	unlock();
	return read_len;
}

int CCHttpStream::readline(u8* buf,int len) {

	char* p ;
	char* end ;
	int read_size = 1;	
	lock();

	for(std::list<u8*>::iterator p_node = p_arr->begin();p_node != p_arr->end();p_node++) {

		p = (char*)(*p_node) + (*p_node == readpage_addr ? read_pos : 0);
		end = (char*)(*p_node) + (*p_node == writepage_addr ? write_pos : page_size);

		while(p < end&&*p != '\n')p++;

		read_size += p - ((char*)(*p_node) + (*p_node == readpage_addr ? read_pos : 0));

		if(read_size > len) {
			unlock();
			return -2;
		}

		if(p < end)
			break;
	}

	unlock();

	if (*p != '\n') {
		return -1;
	}

	return read(buf,read_size);
}
	
int CCHttpStream::write(const u8* wbuf,int len,bool blocked) {

	int write_len = 0;
	int span;

	lock();

	while (len > 0)
	{
		span = page_size - write_pos;

		if ( span <= 0 ){	

			if(blocked) {

				while (!free_page->size()) {

					unlock();
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					CCLOG("I am full");
					lock();

				}

			}else if(!free_page->size())
				break;
			
			writepage_addr = *(free_page->begin());

			while (writepage_addr == NULL)
				writepage_addr = new u8[page_size];

			free_page->pop_front();

			p_arr->push_back(writepage_addr);
			write_pos = 0;

		}

		span = span > len ? len:span;
		memcpy(writepage_addr+write_pos,wbuf,span);

		write_pos += span;
		write_len += span;

		wbuf += span;
		len -= span;
	}

	unlock();
	return write_len;
}

int CCHttpStream::left() {

	lock();
	int ret = free_page->size()*page_size + page_size - write_pos;
	unlock();
	return ret;

}

void CCHttpStream::setFinish(bool v)
{
	lock();
	b_finished = v;
	unlock();
}

bool CCHttpStream::isFinish()
{
	return b_finished;
}

void CCHttpStream::alloc_new_page( int cnt )
{
	lock();
	for(int i = 0;i < cnt;i ++) {
		free_page->push_back(nullptr);
	}
	unlock();
}

int CCHttpStream::skip( int len )
{

	int read_len = 0;
	int span = 0;

	lock();

	while(len) {

		span = (readpage_addr == writepage_addr ? write_pos : page_size) - read_pos;
		span = span > len ? len : span; 

		if (span <= 0)
		{
			if(readpage_addr == writepage_addr)
				break;

			read_pos = 0;
			memset(readpage_addr,0,page_size);
			free_page->push_back(readpage_addr);
			p_arr->pop_front();

			readpage_addr = *(p_arr->begin());
			continue;
		}

		read_pos += span;
		read_len += span;
		len -= span;
	}

	unlock();
	return read_len;
}
