#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "string.h"
#include "sys/ioctl.h"
#include "sys/mman.h"
#include "iostream"

using namespace std;
/* ffmpeg */
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
}


/* DS */
//AVFormatContext *avcontext;
//AVInputFormat   fmt;
//AVDictionary    *options;
//AVCodec         *codec;
//AVCodecID       code_id;
//AVCodecContext  *codecontext;
//AVPacket        *packet; 
/* demo */
int main(int argc, char* argv[])  
{  
    AVFormatContext *pFormatCtx;  
    int             i, videoindex;  
    AVCodecContext  *pCodecCtx;  
    AVCodec         *pCodec;  
    AVFrame *pFrame,*pFrameYUV;  
    uint8_t *out_buffer;  
    AVPacket *packet;  
    int y_size;  
    int ret, got_picture;  
    struct SwsContext *img_convert_ctx;  
    //输入文件路径  
    char filepath[]="./1.mp4";  
  
    int frame_cnt;  
  
    av_register_all();//注册所有组件  
    avformat_network_init();  
    pFormatCtx = avformat_alloc_context();  
  
    //打开输入视频文件  
    if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){  
        printf("Couldn't open input stream.\n");  
        return -1;  
    }  
    //获取视频文件信息  
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){  
        printf("Couldn't find stream information.\n");  
        return -1;  
    }  
    videoindex=-1;  
    for(i=0; i<pFormatCtx->nb_streams; i++)   
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){  
            videoindex=i;//找到视频的数组位置  
            break;  
        }  
    if(videoindex==-1){  
        printf("Didn't find a video stream.\n");  
        return -1;  
    }  
  
    pCodecCtx=pFormatCtx->streams[videoindex]->codec;  
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);//查找解码器  
    if(pCodec==NULL){  
        printf("Codec not found.\n");  
        return -1;  
    }  
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){//打开解码器  
        printf("Could not open codec.\n");  
        return -1;  
    }  
    /* 
     * 在此处添加输出视频信息的代码 
     * 取自于pFormatCtx，使用fprintf() 
     */  
    printf("视频的时长:%dμs\n", pFormatCtx->duration);//输入视频的时长（以微秒为单位）  
  
  
  
    pFrame=av_frame_alloc();  
    pFrameYUV=av_frame_alloc();  
    out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));  
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);  
    packet=(AVPacket *)av_malloc(sizeof(AVPacket));  
    //Output Info-----------------------------  
    printf("--------------- File Information ----------------\n");  
    av_dump_format(pFormatCtx,0,filepath,0);  
    printf("-------------------------------------------------\n");  
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,   
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);   
  
    frame_cnt=0;  
    //从输入文件读取一帧压缩数据  
    while(av_read_frame(pFormatCtx, packet)>=0){  
        if(packet->stream_index==videoindex){  
                /* 
                 * 在此处添加输出H264码流的代码 
                 * 取自于packet，使用fwrite() 
                 */  
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);//解码一帧压缩数据  
            if(ret < 0){  
                printf("Decode Error.\n");  
                return -1;  
            }  
            if(got_picture){  
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,   
                    pFrameYUV->data, pFrameYUV->linesize);  
                printf("Decoded frame index: %d\n",frame_cnt);  
  
                /* 
                 * 在此处添加输出YUV的代码 
                 * 取自于pFrameYUV，使用fwrite() 
                 */  
  
                frame_cnt++;  
  
            }  
        }  
        av_free_packet(packet);  
    }  
  
    sws_freeContext(img_convert_ctx);    
  
    av_frame_free(&pFrameYUV);  
    av_frame_free(&pFrame);  
    avcodec_close(pCodecCtx);//关闭解码器  
    avformat_close_input(&pFormatCtx);//关闭输入视频文件。  
  
    return 0;  
}