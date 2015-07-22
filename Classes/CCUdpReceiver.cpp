#include "CCUdpReceiver.h"

#ifdef __ANDROID__
#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif
#endif

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
};

#if defined(_MSC_VER)
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#endif

unsigned int CCUdpReceiver::udp_receiver(CCUdpReceiver* data) {
	
	AVFormatContext* pFormatCtx;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	int videoindex;
	int i;
	AVFrame	*pFrame, *pFrameRGB;
	uint8_t *out_buffer;		
	int ret, got_picture;
	AVPacket *packet;
	AVInputFormat *ifmt;
	struct SwsContext *img_convert_ctx;
	
	ifmt = av_find_input_format("mpegts");
	pFormatCtx = avformat_alloc_context();
	
	if (avformat_open_input(&pFormatCtx, data->m_url,ifmt, NULL) != 0) {
		CCLOG("***ERROR*** Couldn't open input stream.\n");
		return 1;
	}
	
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		CCLOG("Couldn't find stream information.（无法获取流信息）\n");
		return -1;
	}
	
	videoindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}
	if (videoindex == -1)
	{
		CCLOG("Couldn't find a video stream.（没有找到视频流）\n");
		return -1;
	}
	
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		CCLOG("Codec not found.（没有找到解码器）\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		CCLOG("Could not open codec.（无法打开解码器）\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameRGB, out_buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	//Output Information-----------------------------
	CCLOG("File Information（文件信息）---------------------\n");
	av_dump_format(pFormatCtx, 0, NULL, 0);
	CCLOG("-------------------------------------------------\n");
	
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, 
		pCodecCtx->height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
		
	while(data->getRunningState()) {
		
		if (av_read_frame(pFormatCtx, packet) >= 0)
		{
			if (packet->stream_index == videoindex)
			{
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
				if (ret < 0)
				{
					printf("Decode Error.（解码错误）\n");
					return -1;
				}
				if (got_picture)
				{
					sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
						pFrameRGB->data, pFrameRGB->linesize);
					data->m_buf = pFrameRGB->data[0];
					data->_width = pCodecCtx->width;
					data->_height = pCodecCtx->height;
					data->push();
					//printf("%d %d %d\n", img.width, img.height, pFrameRGB->linesize[0]);
				}
			}	
		} else 
			std::this_thread::yield();
		
	}
	
	av_free_packet(packet);
	av_free(out_buffer);
	av_free(pFrame);
	av_free(pFrameRGB);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	
	CCLOG("receiver return\n");
	return 1;
}

CCUdpReceiver::CCUdpReceiver()
{
	m_buf = NULL;
	receiver_handle = NULL;
	isRunning = false;

	if (ffmpeg_init <= 0)
	{
		av_register_all();
		avformat_network_init();
		ffmpeg_init ++ ;
	}
}

CCUdpReceiver::~CCUdpReceiver( void )
{
	closeReceiver();
}

bool CCUdpReceiver::openReceiver(const char* path)
{
	if (receiver_handle)
		return false;
	strcpy(m_url,path);
	isRunning = true;

	receiver_handle =new std::thread(CCUdpReceiver::udp_receiver,this);

	return receiver_handle != nullptr;
}

void CCUdpReceiver::closeReceiver()
{
	isRunning = false;
	if (receiver_handle)
	{
		receiver_handle->join();
		delete receiver_handle;
		receiver_handle = NULL;
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

void CCUdpReceiver::push()
{
	cocos2d::Image* img = new (std::nothrow)cocos2d::Image;
	if (!img->initWithRawDataRGB(m_buf,_width,_height)) {
		CCLOG("get image data false");
		isRunning = false;
		return;
	}

	m_lock.lock();

	if (img)
		images.push_back(img);

	if (images.size() > 8)
	{
		img = *(images.begin());
		images.pop_front();
		CC_SAFE_DELETE(img);
	}

	m_lock.unlock();
}

cocos2d::Image* CCUdpReceiver::pop()
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

bool CCUdpReceiver::getRunningState()
{
	return isRunning;
}

int CCUdpReceiver::ffmpeg_init = 0;

bool CCUdpReceiver::capture_pic = false;

