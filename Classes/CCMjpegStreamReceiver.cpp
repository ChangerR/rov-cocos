#include "CCMjpegStreamReceiver.h"

unsigned int receiver(CCMjpegStreamReceiver* data) {

	while(data->getRunningState()) {
		data->push();
	}

	CCLOG("receiver return\n");
	return 1;
}

CCMjpegStreamReceiver::CCMjpegStreamReceiver(int max_):max_size(max_)
{
	p_conn = new CCAsyncHttpClient();
	m_buf = new u8[max_];
	receiver_handle = NULL;
	isRunning = false;
}

CCMjpegStreamReceiver::~CCMjpegStreamReceiver( void )
{
	closeReceiver();
	delete[] m_buf;
	delete p_conn;
}

const stringc& CCMjpegStreamReceiver::getResponse()
{
	return p_conn->getResponse();
}

static char buffer[256] = {0};

bool CCMjpegStreamReceiver::getRecvData(int* len)
{
	int state = 0;
	int recv_len;
	stringc tmp;
	u8* buf = m_buf;

	CCHttpStream* outs = p_conn->getHttpOutputStream();		

	do 
	{
		memset(buffer,0,256);

		recv_len = outs->readline((u8*)buffer,256);

		if (recv_len == -2) {
			CCLOG("error read\n");
			return false;
		}else if (recv_len == -1)
		{
			std::this_thread::yield();
		}else
			tmp.append(buffer);
	} while (!outs->isFinish()
		&&(recv_len > 2||recv_len <= 0||tmp.size()<=2));

	//printf("->(\n%s,%d)\n",tmp.c_str(),tmp.size());
	if (outs->isFinish())
		return false;

	int pos = tmp.find("Content-Length:",0);
	if (pos == std::string::npos) {
		CCLOG("cannot find length\n");
		return false;
	}

	int pos2 = tmp.find("\n",pos);
	if (pos2 == std::string::npos)
		return false;

	pos += 15;
	stringc digital = tmp.substr(pos,pos2 - pos);
	trim(digital);
	if (!isDigit(digital)) {
		CCLOG("(is not digital:%s)",digital.c_str());
		return false;
	}

	recv_len = toDigit(digital);

	int write_len = 0;
	if (recv_len > *len)
	{
		CCLOG("In memory not enough");
		*len = -1;		
		while(recv_len) {
			write_len = outs->skip(recv_len);
			recv_len -= write_len;
		}
		return true;
	}
	
	*len = recv_len;
	while(recv_len) {

		write_len = outs->read(buf,recv_len);
		recv_len -= write_len;
		buf += write_len;

	}

	if (capture_pic)
	{
		CCLOG("save file\n");
		/*stringc tmp;
		tmp.formatFilename();
		tmp.append(".jpeg");
		FILE* pic = fopen(tmp.c_str(),"wb");
		fwrite(m_buf,*len,1,pic);
		fflush(pic);
		fclose(pic);
		capture_pic = false;
		printf("save file %s ok\n",tmp.c_str());*/
	}
	return true;
}

bool CCMjpegStreamReceiver::openReceiver(const char* path)
{
	if (receiver_handle)
		return false;
	isRunning = true;
	p_conn->openConnection(path);
	receiver_handle =new std::thread(receiver,this);

	return receiver_handle != nullptr;
}

void CCMjpegStreamReceiver::closeReceiver()
{
	isRunning = false;
	if (receiver_handle)
	{
		receiver_handle->join();
		delete receiver_handle;
		receiver_handle = NULL;
		p_conn->closeConnection();
	}	

	if (!images.empty())
	{
		m_lock.lock();
		for (auto img = images.begin(); img != images.end();img++)
		{
			CC_SAFE_DELETE(*img);
		}
		m_lock.unlock();
	}
}

void CCMjpegStreamReceiver::push()
{
	int len = max_size;
	cocos2d::Image* img = new (std::nothrow)cocos2d::Image;
	if (!getRecvData(&len)||!img->initWithImageData(m_buf,len)) {
		CCLOG("get image data false");
		isRunning = false;
		return;
	}

	m_lock.lock();

	if (img)
		images.push_back(img);

	if (images.size() > 4)
	{
		CCLOG("I am delete it");
		img = *(images.begin());
		images.pop_front();
		CC_SAFE_DELETE(img);
	}

	m_lock.unlock();
}

cocos2d::Image* CCMjpegStreamReceiver::pop()
{
	cocos2d::Image* img;

	m_lock.lock();

	if (images.empty())
		img = nullptr;
	else {
		img = *(images.begin());
		images.pop_front();
	}
		
	m_lock.unlock();
	return img;
}

bool CCMjpegStreamReceiver::capture_pic = false;

