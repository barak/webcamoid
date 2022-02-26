/* Webcamoid, webcam capture application.
 * Copyright (C) 2017  Gonzalo Exequiel Pedone
 *
 * Webcamoid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Webcamoid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

#include <QDebug>
#include <QMutex>
#include <QSharedPointer>
#include <QWaitCondition>
#include <akaudiocaps.h>
#include <akaudioconverter.h>
#include <akaudiopacket.h>
#include <akcaps.h>
#include <akelement.h>
#include <akpacket.h>
#include <akpluginmanager.h>

extern "C"
{
    #include <libavutil/channel_layout.h>
}

#include "audiostream.h"
#include "mediawriterffmpeg.h"

using SampleFormatsMap = QMap<AkAudioCaps::SampleFormat, AVSampleFormat>;
using ChannelLayoutsMap = QMap<AkAudioCaps::ChannelLayout, uint64_t>;

class AudioStreamPrivate
{
    public:
        AkAudioConverter m_audioConvert;
        AVFrame *m_frame {nullptr};
        QMutex m_frameMutex;
        int64_t m_pts {0};
        QWaitCondition m_frameReady;

        inline static const SampleFormatsMap &sampleFormats(bool planar)
        {
            static const SampleFormatsMap formats {
                {AkAudioCaps::SampleFormat_u8 , AV_SAMPLE_FMT_U8 },
                {AkAudioCaps::SampleFormat_s16, AV_SAMPLE_FMT_S16},
                {AkAudioCaps::SampleFormat_s32, AV_SAMPLE_FMT_S32},
                {AkAudioCaps::SampleFormat_s64, AV_SAMPLE_FMT_S64 },
                {AkAudioCaps::SampleFormat_flt, AV_SAMPLE_FMT_FLT},
                {AkAudioCaps::SampleFormat_dbl, AV_SAMPLE_FMT_DBL},
            };
            static const SampleFormatsMap planarFormats {
                {AkAudioCaps::SampleFormat_u8 , AV_SAMPLE_FMT_U8P },
                {AkAudioCaps::SampleFormat_s16, AV_SAMPLE_FMT_S16P},
                {AkAudioCaps::SampleFormat_s32, AV_SAMPLE_FMT_S32P},
                {AkAudioCaps::SampleFormat_s64, AV_SAMPLE_FMT_S64P},
                {AkAudioCaps::SampleFormat_flt, AV_SAMPLE_FMT_FLTP},
                {AkAudioCaps::SampleFormat_dbl, AV_SAMPLE_FMT_DBLP},
            };

            return planar? planarFormats: formats;
        }

        inline static const QVector<AVSampleFormat> &planarFormats()
        {
            static const QVector<AVSampleFormat> formats {
                AV_SAMPLE_FMT_U8P ,
                AV_SAMPLE_FMT_S16P,
                AV_SAMPLE_FMT_S32P,
                AV_SAMPLE_FMT_FLTP,
                AV_SAMPLE_FMT_DBLP,
                AV_SAMPLE_FMT_S64P,
            };

            return formats;
        }

        inline static const ChannelLayoutsMap &channelLayouts()
        {
            static const ChannelLayoutsMap channelLayouts = {
                {AkAudioCaps::Layout_mono         , AV_CH_LAYOUT_MONO             },
                {AkAudioCaps::Layout_stereo       , AV_CH_LAYOUT_STEREO           },
                {AkAudioCaps::Layout_2p1          , AV_CH_LAYOUT_2POINT1          },
                {AkAudioCaps::Layout_3p0          , AV_CH_LAYOUT_SURROUND         },
                {AkAudioCaps::Layout_3p0_back     , AV_CH_LAYOUT_2_1              },
                {AkAudioCaps::Layout_3p1          , AV_CH_LAYOUT_3POINT1          },
                {AkAudioCaps::Layout_4p0          , AV_CH_LAYOUT_4POINT0          },
                {AkAudioCaps::Layout_quad         , AV_CH_LAYOUT_QUAD             },
                {AkAudioCaps::Layout_quad_side    , AV_CH_LAYOUT_2_2              },
                {AkAudioCaps::Layout_4p1          , AV_CH_LAYOUT_4POINT1          },
                {AkAudioCaps::Layout_5p0          , AV_CH_LAYOUT_5POINT0_BACK     },
                {AkAudioCaps::Layout_5p0_side     , AV_CH_LAYOUT_5POINT0          },
                {AkAudioCaps::Layout_5p1          , AV_CH_LAYOUT_5POINT1_BACK     },
                {AkAudioCaps::Layout_5p1_side     , AV_CH_LAYOUT_5POINT1          },
                {AkAudioCaps::Layout_6p0          , AV_CH_LAYOUT_6POINT0          },
                {AkAudioCaps::Layout_6p0_front    , AV_CH_LAYOUT_6POINT0_FRONT    },
                {AkAudioCaps::Layout_hexagonal    , AV_CH_LAYOUT_HEXAGONAL        },
                {AkAudioCaps::Layout_6p1          , AV_CH_LAYOUT_6POINT1          },
                {AkAudioCaps::Layout_6p1_back     , AV_CH_LAYOUT_6POINT1_BACK     },
                {AkAudioCaps::Layout_6p1_front    , AV_CH_LAYOUT_6POINT1_FRONT    },
                {AkAudioCaps::Layout_7p0          , AV_CH_LAYOUT_7POINT0          },
                {AkAudioCaps::Layout_7p0_front    , AV_CH_LAYOUT_7POINT0_FRONT    },
                {AkAudioCaps::Layout_7p1          , AV_CH_LAYOUT_7POINT1          },
                {AkAudioCaps::Layout_7p1_wide     , AV_CH_LAYOUT_7POINT1_WIDE     },
                {AkAudioCaps::Layout_7p1_wide_back, AV_CH_LAYOUT_7POINT1_WIDE_BACK},
                {AkAudioCaps::Layout_octagonal    , AV_CH_LAYOUT_OCTAGONAL        },
                {AkAudioCaps::Layout_hexadecagonal, AV_CH_LAYOUT_HEXADECAGONAL    },
                {AkAudioCaps::Layout_downmix      , AV_CH_LAYOUT_STEREO_DOWNMIX   },
            };

            return channelLayouts;
        }
};

AudioStream::AudioStream(const AVFormatContext *formatContext,
                         uint index,
                         int streamIndex,
                         const QVariantMap &configs,
                         const QMap<QString, QVariantMap> &codecOptions,
                         MediaWriterFFmpeg *mediaWriter,
                         QObject *parent):
    AbstractStream(formatContext,
                   index, streamIndex,
                   configs,
                   codecOptions,
                   mediaWriter,
                   parent)
{
    this->d = new AudioStreamPrivate;
    auto codecContext = this->codecContext();
    auto codec = codecContext->codec;
    auto defaultCodecParams = mediaWriter->defaultCodecParams(codec->name);
    codecContext->bit_rate = configs["bitrate"].toInt();

    if (codecContext->bit_rate < 1)
        codecContext->bit_rate = defaultCodecParams["defaultBitRate"].toInt();

    switch (codec->id) {
    case AV_CODEC_ID_G723_1:
        codecContext->bit_rate = 6300;
        break;
    case AV_CODEC_ID_GSM_MS:
        codecContext->bit_rate = 13000;
        break;
    default:
        break;
    }

    AkAudioCaps audioCaps(configs["caps"].value<AkCaps>());
    auto ffFormat =
            AudioStreamPrivate::sampleFormats(audioCaps.planar())
            .value(audioCaps.format(), AV_SAMPLE_FMT_NONE);
    auto sampleFormat = QString(av_get_sample_fmt_name(ffFormat));
    auto supportedSampleFormats =
            defaultCodecParams["supportedSampleFormats"].toStringList();

    if (!supportedSampleFormats.isEmpty()
        && !supportedSampleFormats.contains(sampleFormat)) {
        auto defaultSampleFormat = defaultCodecParams["defaultSampleFormat"].toString();
        ffFormat = av_get_sample_fmt(defaultSampleFormat.toStdString().c_str());
        auto planar = AudioStreamPrivate::planarFormats().contains(ffFormat);
        auto format =
                AudioStreamPrivate::sampleFormats(planar)
                .key(ffFormat, AkAudioCaps::SampleFormat_none);
        audioCaps.setFormat(format);
        audioCaps.updatePlaneSize(planar);
    }

    auto supportedSampleRates =
            defaultCodecParams["supportedSampleRates"].toList();

    if (!supportedSampleRates.isEmpty()) {
        int sampleRate = 0;
        int maxDiff = std::numeric_limits<int>::max();

        for (auto &rate: supportedSampleRates) {
            int diff = qAbs(audioCaps.rate() - rate.toInt());

            if (diff < maxDiff) {
                sampleRate = rate.toInt();

                if (!diff)
                    break;

                maxDiff = diff;
            }
        }

        audioCaps.rate() = sampleRate;
    }

    auto channelLayout = AkAudioCaps::channelLayoutToString(audioCaps.layout());
    auto supportedChannelLayouts =
            defaultCodecParams["supportedChannelLayouts"].toStringList();

    if (!supportedChannelLayouts.isEmpty() && !supportedChannelLayouts.contains(channelLayout)) {
        auto defaultChannelLayout = defaultCodecParams["defaultChannelLayout"].toString();
        auto layout = AkAudioCaps::channelLayoutFromString(defaultChannelLayout);
        audioCaps.setLayout(layout);
    }

    if (!strcmp(formatContext->oformat->name, "gxf")) {
        audioCaps.rate() = 48000;
        audioCaps.setLayout(AkAudioCaps::Layout_mono);
    } else if (!strcmp(formatContext->oformat->name, "mxf")) {
        audioCaps.rate() = 48000;
    } else if (!strcmp(formatContext->oformat->name, "swf")) {
        audioCaps = mediaWriter->nearestSWFCaps(audioCaps);
    }

    codecContext->sample_fmt =
            AudioStreamPrivate::sampleFormats(audioCaps.planar())
            .value(audioCaps.format(), AV_SAMPLE_FMT_NONE);
    codecContext->sample_rate = audioCaps.rate();
    auto layout = AkAudioCaps::channelLayoutToString(audioCaps.layout());
    codecContext->channel_layout = av_get_channel_layout(layout.toStdString().c_str());
    codecContext->channels = audioCaps.channels();

    auto stream = this->stream();
    stream->time_base.num = 1;
    stream->time_base.den = audioCaps.rate();
    codecContext->time_base = stream->time_base;
    this->d->m_audioConvert.setOutputCaps(audioCaps);
}

AudioStream::~AudioStream()
{
    this->uninit();
    delete this->d;
}

void AudioStream::convertPacket(const AkPacket &packet)
{
    if (!packet)
        return;

    auto codecContext = this->codecContext();
    auto iPacket = AkAudioPacket(this->d->m_audioConvert.convert(packet));

    if (!iPacket)
        return;

    AVFrame iFrame;
    memset(&iFrame, 0, sizeof(AVFrame));
    iFrame.format = codecContext->sample_fmt;
    iFrame.channel_layout = codecContext->channel_layout;
    iFrame.sample_rate = codecContext->sample_rate;
    iFrame.nb_samples = iPacket.caps().samples();
    iFrame.pts = iPacket.pts();
    int channels = av_get_channel_layout_nb_channels(iFrame.channel_layout);

    if (av_samples_fill_arrays(iFrame.data,
                               iFrame.linesize,
                               reinterpret_cast<const uint8_t *>(iPacket.buffer().constData()),
                               channels,
                               iPacket.caps().samples(),
                               AVSampleFormat(iFrame.format),
                               1) < 0) {
        return;
    }

    this->d->m_frameMutex.lock();

    // Create new buffer.
    auto oFrame = av_frame_alloc();
    oFrame->format = codecContext->sample_fmt;
    oFrame->channel_layout = codecContext->channel_layout;
    oFrame->sample_rate = codecContext->sample_rate;
    oFrame->nb_samples = (this->d->m_frame? this->d->m_frame->nb_samples: 0)
                         + iFrame.nb_samples;
    oFrame->pts = this->d->m_frame? this->d->m_frame->pts: 0;

    if (av_frame_get_buffer(oFrame, 0) < 0) {
        this->deleteFrame(&oFrame);
        this->d->m_frameMutex.unlock();

        return;
    }

    // Copy old samples to new buffer.
    if (this->d->m_frame)
        if (av_samples_copy(oFrame->data,
                            this->d->m_frame->data,
                            0,
                            0,
                            this->d->m_frame->nb_samples,
                            channels,
                            AVSampleFormat(iFrame.format)) < 0) {
            this->deleteFrame(&oFrame);
            this->d->m_frameMutex.unlock();

            return;
        }

    // Copy converted samples to the new buffer.
    if (av_samples_copy(oFrame->data,
                        iFrame.data,
                        this->d->m_frame? this->d->m_frame->nb_samples: 0,
                        0,
                        iFrame.nb_samples,
                        channels,
                        AVSampleFormat(iFrame.format)) < 0) {
        this->deleteFrame(&oFrame);
        this->d->m_frameMutex.unlock();

        return;
    }

    this->deleteFrame(&this->d->m_frame);
    this->d->m_frame = oFrame;

    if (codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE
        || oFrame->nb_samples >= codecContext->frame_size) {
        this->d->m_frameReady.wakeAll();
    }

    this->d->m_frameMutex.unlock();
}

int AudioStream::encodeData(AVFrame *frame)
{
    auto codecContext = this->codecContext();

    if (!frame
        && codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        return AVERROR_EOF;

    if (frame) {
        frame->pts = this->d->m_pts;
        this->d->m_pts += frame->nb_samples;
    } else {
        this->d->m_pts++;
    }

    auto stream = this->stream();

    // Compress audio packet.
    int result = avcodec_send_frame(codecContext, frame);

    if (result < 0) {
        char error[1024];
        av_strerror(result, error, 1024);
        qDebug() << "Error: " << error;

        return result;
    }

    forever {
        // Initialize audio packet.
        auto pkt = av_packet_alloc();
        result = avcodec_receive_packet(codecContext, pkt);

        if (result < 0) {
            av_packet_free(&pkt);

            break;
        }

        pkt->stream_index = this->streamIndex();
        this->rescaleTS(pkt, codecContext->time_base, stream->time_base);

        // Write the compressed frame to the media file.
        emit this->packetReady(pkt);
        av_packet_free(&pkt);
    }

    return result;
}

AVFrame *AudioStream::dequeueFrame()
{
    /* FIXME: Allocating and copying frames when enqueuing/dequeuing is pretty
     * slow, it can improved creating a fixed size frame, and then playing with
     * read write pointers.
     */

    auto codecContext = this->codecContext();
    this->d->m_frameMutex.lock();

    if (!this->d->m_frame
        || (!(codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
            && this->d->m_frame->nb_samples < codecContext->frame_size)) {
        if (!this->d->m_frameReady.wait(&this->d->m_frameMutex, THREAD_WAIT_LIMIT)) {
            this->d->m_frameMutex.unlock();

            return nullptr;
        }
    }

    if (!this->d->m_frame) {
        this->d->m_frameMutex.unlock();

        return nullptr;
    }

    AVFrame *oFrame = nullptr;

    if (codecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE
        || this->d->m_frame->nb_samples == codecContext->frame_size) {
        oFrame = this->d->m_frame;
        this->d->m_frame = nullptr;
    } else {
        // Create output buffer.
        oFrame = av_frame_alloc();
        oFrame->format = codecContext->sample_fmt;
        oFrame->channel_layout = codecContext->channel_layout;
        oFrame->sample_rate = codecContext->sample_rate;
        oFrame->nb_samples = codecContext->frame_size;
        oFrame->pts = this->d->m_frame->pts;
        int channels = av_get_channel_layout_nb_channels(oFrame->channel_layout);

        if (av_frame_get_buffer(oFrame, 0) < 0) {
            this->deleteFrame(&oFrame);
            this->d->m_frameMutex.unlock();

            return nullptr;
        }

        // Copy samples to the output buffer.
        if (av_samples_copy(oFrame->data,
                            this->d->m_frame->data,
                            0,
                            0,
                            codecContext->frame_size,
                            channels,
                            AVSampleFormat(oFrame->format)) < 0) {
            this->deleteFrame(&oFrame);
            this->d->m_frameMutex.unlock();

            return nullptr;
        }

        // Create new buffer.
        auto frame = av_frame_alloc();
        frame->format = codecContext->sample_fmt;
        frame->channel_layout = codecContext->channel_layout;
        frame->sample_rate = codecContext->sample_rate;
        frame->nb_samples = this->d->m_frame->nb_samples - codecContext->frame_size;
        frame->pts = this->d->m_frame->pts + codecContext->frame_size;

        if (av_frame_get_buffer(frame, 0) < 0) {
            this->deleteFrame(&oFrame);
            this->deleteFrame(&frame);
            this->d->m_frameMutex.unlock();

            return nullptr;
        }

        // Copy samples to the output buffer.
        if (av_samples_copy(frame->data,
                            this->d->m_frame->data,
                            0,
                            codecContext->frame_size,
                            frame->nb_samples,
                            channels,
                            AVSampleFormat(frame->format)) < 0) {
            this->deleteFrame(&oFrame);
            this->deleteFrame(&frame);
            this->d->m_frameMutex.unlock();

            return nullptr;
        }

        this->deleteFrame(&this->d->m_frame);
        this->d->m_frame = frame;
    }

    this->d->m_frameMutex.unlock();

    return oFrame;
}

bool AudioStream::init()
{
    auto result = AbstractStream::init();
    this->d->m_audioConvert.reset();

    return result;
}

void AudioStream::uninit()
{
    AbstractStream::uninit();

    this->d->m_frameMutex.lock();
    this->deleteFrame(&this->d->m_frame);
    this->d->m_frameMutex.unlock();
}

#include "moc_audiostream.cpp"
