#include "FFmpegMuxer.hpp"
#include "ffmpegCodecHeader.hpp"
#include <iostream>
#include <cstring>
extern "C" {
#include <libavutil/dict.h>
}
namespace bsp_container
{

FFmpegMuxer::~FFmpegMuxer()
{
    closeContainerMux();
}

int FFmpegMuxer::openContainerMux(const std::string& path)
{
    AVFormatContext* format_Ctx = nullptr;
    avformat_alloc_output_context2(&format_Ctx, nullptr, nullptr, path.c_str());
    if (format_Ctx == nullptr)
    {
        std::cerr << "Could not allocate output context." << std::endl;
        return -1;
    }

    m_format_Ctx = std::shared_ptr<AVFormatContext>(format_Ctx, [](AVFormatContext* p) { avformat_free_context(p); });
    m_path = path;

}

void FFmpegMuxer::closeContainerMux()
{
    m_format_Ctx.reset();
}

void FFmpegMuxer::setupVideoStreamParams(AVStream* out_stream, StreamInfo& streamInfo)
{
    if (out_stream == nullptr)
    {
        std::cerr << "Output video sstream is null." << std::endl;
        return;
    }

    out_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    out_stream->codecpar->codec_id = ffmpegCodecHeader::getInstance().strToFFmpegCoding(streamInfo.codec_params.codec_name);
    out_stream->codecpar->bit_rate = streamInfo.codec_params.bit_rate;
    out_stream->codecpar->sample_aspect_ratio = av_d2q(streamInfo.codec_params.sample_aspect_ratio, 255);
    out_stream->codecpar->width = streamInfo.codec_params.width;
    out_stream->codecpar->height = streamInfo.codec_params.height;
    out_stream->time_base = {1, streamInfo.codec_params.frame_rate};
}

void FFmpegMuxer::setupAudioStreamParams(AVStream* out_stream, StreamInfo& streamInfo)
{
    if (out_stream == nullptr)
    {
        std::cerr << "Output audio stream is null." << std::endl;
        return;
    }

    out_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    out_stream->codecpar->codec_id = ffmpegCodecHeader::getInstance().strToFFmpegCoding(streamInfo.codec_params.codec_name);
    out_stream->codecpar->bit_rate = streamInfo.codec_params.bit_rate;
    out_stream->codecpar->sample_rate = streamInfo.codec_params.sample_rate;
    out_stream->time_base = {1, streamInfo.codec_params.sample_rate};

}

int FFmpegMuxer::addStream(StreamInfo& streamInfo)
{
    AVStream* out_stream = avformat_new_stream(m_format_Ctx.get(), nullptr);
    if (out_stream == nullptr)
    {
        std::cerr << "Could not allocate output stream." << std::endl;
        return -1;
    }

    out_stream->id = m_format_Ctx->nb_streams - 1;

    if (streamInfo.codec_params.codec_type == "video")
    {
        setupVideoStreamParams(out_stream, streamInfo);
    }
    else if (streamInfo.codec_params.codec_type == "audio")
    {
        setupAudioStreamParams(out_stream, streamInfo);
    }
    else
    {
        std::cerr << "Unsupported codec type." << std::endl;
        return -1;
    }

    return 0;

}

int FFmpegMuxer::writeStreamPacket(StreamPacket& streamPacket)
{
    if (m_header_written == false)
    {
        if (m_format_Ctx == nullptr)
        {
            std::cerr << "Output format context is null." << std::endl;
            return -1;
        }

        if(!(m_format_Ctx->oformat->flags & AVFMT_NOFILE))
        {
            if (avio_open(&m_format_Ctx->pb, m_path.c_str(), AVIO_FLAG_WRITE) < 0)
            {
                std::cerr << "Could not open output file." << std::endl;
                return -1;
            }
        }

        if (avformat_write_header(m_format_Ctx.get(), nullptr) < 0)
        {
            std::cerr << "Could not write header." << std::endl;
            return -1;
        }
        m_header_written = true;
    }

    AVPacket pkt;

    pkt.stream_index = streamPacket.stream_index;
    pkt.pts = streamPacket.pts;
    pkt.dts = streamPacket.dts;
    pkt.data = streamPacket.pkt_data.data();
    pkt.size = streamPacket.pkt_data.size();
    pkt.pos = streamPacket.pos;

    if (av_interleaved_write_frame(m_format_Ctx.get(), &pkt) < 0)
    {
        std::cerr << "Could not write frame." << std::endl;
        return -1;
    }

    return 0;
}

int FFmpegMuxer::endStreamMux()
{
    if (m_format_Ctx == nullptr)
    {
        std::cerr << "Output format context is null." << std::endl;
        return -1;
    }

    av_write_trailer(m_format_Ctx.get());

    if (!(m_format_Ctx->oformat->flags & AVFMT_NOFILE))
    {
        avio_closep(&m_format_Ctx->pb);
    }
}

} // namespace bsp_container