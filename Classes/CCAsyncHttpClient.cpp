#include "CCAsyncHttpClient.h"

#include "CCHttpStream.h"

#if defined(_MSC_VER)
#pragma comment(lib,"libcurl_imp.lib")
#endif

long data_writer_sync(void *data, int size, int nmemb, CCHttpStream* stream) { 

	return stream->write((u8*)data,size*nmemb,false);

}

long data_writer(void *data, int size, int nmemb, CCHttpStream* stream) { 

	return stream->write((u8*)data,size*nmemb,true);

}

long response_writer(void *data, int size, int nmemb, stringc* response) {

	long sizes = size * nmemb;
	response->append((char*)data,sizes);
	return sizes;

}

CCAsyncHttpClient::CCAsyncHttpClient(int page_size,int page_count)
	:p_thread(NULL){	

	handler = curl_easy_init();
	m_outs = new CCHttpStream(page_size,page_size);
	m_isClosed = false;
	
}

CCAsyncHttpClient::~CCAsyncHttpClient() {

	closeConnection();
	delete m_outs;
	curl_easy_cleanup(handler);
}

CCHttpStream* CCAsyncHttpClient::getHttpOutputStream() {

	return m_outs;

}

void CCAsyncHttpClient::closeConnection() {

	m_isClosed = true;
	if (p_thread)
	{
		m_outs->alloc_new_page(8);
		p_thread->join();
		delete p_thread;
		p_thread = NULL;
	}
}

stringc& CCAsyncHttpClient::getResponse() {

	return m_Response;

}


bool CCAsyncHttpClient::openConnection(const char* url)
{
	if (p_thread)
		return false;
	m_url = url;
	p_thread = new std::thread(urlConnAsync,this);

	return p_thread == nullptr ? false : true;
}

int progress_callback(void *clientp,double dltotal,double dlnow,double ultotal,double ulnow) {

	CCAsyncHttpClient* conn = (CCAsyncHttpClient*)clientp;
	if (conn->isCloseed() == true) {
		printf("progress_callback call for close\n");
		return CURLE_ABORTED_BY_CALLBACK;
	}
	return 0;
}

unsigned int urlConnAsync(CCAsyncHttpClient* data)  
{  

	curl_easy_setopt(data->handler, CURLOPT_URL, data->m_url.c_str());
	curl_easy_setopt(data->handler, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(data->handler, CURLOPT_FOLLOWLOCATION, 1);
	/// 保存body文件
	curl_easy_setopt(data->handler, CURLOPT_WRITEFUNCTION, data_writer);
	curl_easy_setopt(data->handler, CURLOPT_WRITEDATA, data->m_outs);
	/// 保存服务器返回的响应消息
	curl_easy_setopt(data->handler, CURLOPT_HEADERFUNCTION, response_writer);
	curl_easy_setopt(data->handler, CURLOPT_WRITEHEADER, &(data->getResponse()));
	curl_easy_setopt(data->handler, CURLOPT_PROGRESSFUNCTION, progress_callback);
	curl_easy_setopt(data->handler, CURLOPT_PROGRESSDATA, data);
	curl_easy_setopt(data->handler, CURLOPT_NOPROGRESS, 0);
	CURLcode res = CURLE_OK;

	int retcode = 0;
	res = curl_easy_perform(data->handler);
	res = curl_easy_getinfo(data->handler, CURLINFO_RESPONSE_CODE, &retcode);
	data->m_outs->setFinish(true);

	return 1;
}
