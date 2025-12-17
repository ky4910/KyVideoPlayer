#include <QDebug>

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
}

#include "minedecoder.h"

// video path: D:\SW\shareDir\Focal20.04\VideoDir\vDir\276982.mp4

int MineDecoder::startDecoding(const QString &filePath)
{
    int ret = 0;
    AVFormatContext *fmtCtx = nullptr;

    qDebug() << "Start decoding:" << filePath;

    if ( filePath.isEmpty() )
    {
        qDebug() << "file path is NULL";
    }

    fmtCtx = avformat_alloc_context();
    if ( !fmtCtx )
    {
        printf("Could not allocate the context!\n");
        return -1;
    }

    ret = avformat_open_input(&fmtCtx, filePath.toStdString().c_str(), nullptr, nullptr);
    if (ret < 0)
    {
        char errbuf[128];
        av_strerror(ret, errbuf, sizeof(errbuf));
        fprintf(stderr, "Could not open video stream: %s\n", errbuf);
        return -1;
    }

    ret = avformat_find_stream_info(fmtCtx, nullptr);
    if ( ret < 0 )
    {
        printf("Find stream info fail!\n");
        return -1;
    }
    printf("Stream numbers: %u\n", fmtCtx->nb_streams);

    int video_stream_index = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (video_stream_index < 0)
    {
        char errbuf[128];
        av_strerror(video_stream_index, errbuf, sizeof(errbuf));
        fprintf(stderr, "Could not find video stream: %s\n", errbuf);
        return -1;
    }

    printf("Video stream index: %d\n", video_stream_index);
    AVStream *video_stream = fmtCtx->streams[video_stream_index];
    AVCodecParameters *codecpar = video_stream->codecpar;

    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    avcodec_open2(codec_ctx, codec, nullptr);

    AVPacket *pkt = nullptr;
    pkt = av_packet_alloc();
    if ( !pkt )
    {
        printf("Could not allocate the packet!\n");
        return -1;
    }

    int cnt = 0;
    AVFrame *frame = av_frame_alloc();

    while( av_read_frame(fmtCtx, pkt) >= 0)
    {
        if (pkt->stream_index == video_stream_index)
        {
            // Send data to decoder
            int ret = avcodec_send_packet(codec_ctx, pkt);
            if (ret < 0)
            {
                // fprintf(stderr, "Error sending packet: %s\n", av_err2str(ret));
                continue;
            }

            // Send packages to decoder
            while( avcodec_receive_frame(codec_ctx, frame) == 0)
            {
                //
                cnt++;
                printf("No.%d frame\n", cnt);
            }
        }
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);

    if ( fmtCtx )
    {
        avformat_close_input(&fmtCtx);
    }

    return 0;
}

