#ifndef __JUST_HTTP_CONNECTION_
#define __JUST_HTTP_CONNECTION_
#include "CCHttpStream.h"
#include "curl/curl.h"
#include <string>
#include <thread>
#include "stringc.h"
class CCAsyncHttpClient;

unsigned int urlConnAsync(CCAsyncHttpClient* data);

class CCAsyncHttpClient {

public:

	CCAsyncHttpClient(int page_size = 256 * 1024,int page_count = 8);

	virtual ~CCAsyncHttpClient();

	bool openConnection(const char* url);

	CCHttpStream* getHttpOutputStream();

	void closeConnection();
	
	stringc& getResponse();
	
	bool isCloseed() {
		return m_isClosed;
	}

private:
	std::thread * p_thread;
	CCHttpStream* m_outs;
	stringc m_Response;
	stringc m_url;
	bool m_isClosed;
	CURL* handler;
	friend unsigned int urlConnAsync(CCAsyncHttpClient* data);
};
#endif