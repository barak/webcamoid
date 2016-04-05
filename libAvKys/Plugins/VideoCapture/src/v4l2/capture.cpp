/* Webcamoid, webcam capture application.
 * Copyright (C) 2011-2016  Gonzalo Exequiel Pedone
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
 * Email   : hipersayan DOT x AT gmail DOT com
 * Web-Site: http://github.com/hipersayanX/webcamoid
 */

#include "capture.h"

typedef QMap<quint32, QString> V4l2PixFmtMap;

inline V4l2PixFmtMap initV4l2PixFmtMap()
{
    V4l2PixFmtMap rawToFF;

    // RGB formats
    //rawToFF[V4L2_PIX_FMT_RGB332] = "";
    rawToFF[V4L2_PIX_FMT_RGB444] = "rgb444le";
    rawToFF[V4L2_PIX_FMT_RGB555] = "rgb555le";
    rawToFF[V4L2_PIX_FMT_RGB565] = "rgb565le";
    rawToFF[V4L2_PIX_FMT_RGB555X] = "rgb555be";
    rawToFF[V4L2_PIX_FMT_RGB565X] = "rgb565be";
    //rawToFF[V4L2_PIX_FMT_BGR666] = "";
    rawToFF[V4L2_PIX_FMT_BGR24] = "bgr24";
    rawToFF[V4L2_PIX_FMT_RGB24] = "rgb24";
    rawToFF[V4L2_PIX_FMT_BGR32] = "bgr0";
    rawToFF[V4L2_PIX_FMT_RGB32] = "0rgb";

    // Grey formats
    rawToFF[V4L2_PIX_FMT_GREY] = "gray";
    //rawToFF[V4L2_PIX_FMT_Y4] = "";
    //rawToFF[V4L2_PIX_FMT_Y6] = "";
    //rawToFF[V4L2_PIX_FMT_Y10] = "";
    //rawToFF[V4L2_PIX_FMT_Y12] = "";
    rawToFF[V4L2_PIX_FMT_Y16] = "gray16le";

    // Grey bit-packed formats
    //rawToFF[V4L2_PIX_FMT_Y10BPACK] = "";

    // Palette formats
    //rawToFF[V4L2_PIX_FMT_PAL8] = "";

    // Chrominance formats
    //rawToFF[V4L2_PIX_FMT_UV8] = "";

    // Luminance+Chrominance formats
    rawToFF[V4L2_PIX_FMT_YVU410] = "yuv410p";
    rawToFF[V4L2_PIX_FMT_YVU420] = "yuv420p";
    rawToFF[V4L2_PIX_FMT_YUYV] = "yuyv422";
    rawToFF[V4L2_PIX_FMT_YYUV] = "yuv422p";
    //rawToFF[V4L2_PIX_FMT_YVYU] = "";
    rawToFF[V4L2_PIX_FMT_UYVY] = "uyvy422";
    rawToFF[V4L2_PIX_FMT_VYUY] = "yuv422p";
    rawToFF[V4L2_PIX_FMT_YUV422P] = "yuv422p";
    rawToFF[V4L2_PIX_FMT_YUV411P] = "yuv411p";
    rawToFF[V4L2_PIX_FMT_Y41P] = "yuv411p";
    //rawToFF[V4L2_PIX_FMT_YUV444] = "";
    //rawToFF[V4L2_PIX_FMT_YUV555] = "";
    //rawToFF[V4L2_PIX_FMT_YUV565] = "";
    //rawToFF[V4L2_PIX_FMT_YUV32] = "";
    rawToFF[V4L2_PIX_FMT_YUV410] = "yuv410p";
    rawToFF[V4L2_PIX_FMT_YUV420] = "yuv420p";
    //rawToFF[V4L2_PIX_FMT_HI240] = "";
    //rawToFF[V4L2_PIX_FMT_HM12] = "";
    //rawToFF[V4L2_PIX_FMT_M420] = "";

    // two planes -- one Y, one Cr + Cb interleaved
    rawToFF[V4L2_PIX_FMT_NV12] = "nv12";
    rawToFF[V4L2_PIX_FMT_NV21] = "nv21";
    rawToFF[V4L2_PIX_FMT_NV16] = "nv16";
    //rawToFF[V4L2_PIX_FMT_NV61] = "";
    //rawToFF[V4L2_PIX_FMT_NV24] = "";
    //rawToFF[V4L2_PIX_FMT_NV42] = "";

    // Bayer formats
    rawToFF[V4L2_PIX_FMT_SBGGR8] = "bayer_bggr8";
    rawToFF[V4L2_PIX_FMT_SGBRG8] = "bayer_gbrg8";
    rawToFF[V4L2_PIX_FMT_SGRBG8] = "bayer_grbg8";
    rawToFF[V4L2_PIX_FMT_SRGGB8] = "bayer_rggb8";

    // 10bit raw bayer, expanded to 16 bits
    rawToFF[V4L2_PIX_FMT_SBGGR16] = "bayer_bggr16le";

    return rawToFF;
}

Q_GLOBAL_STATIC_WITH_ARGS(V4l2PixFmtMap, rawToFF, (initV4l2PixFmtMap()))

inline V4l2PixFmtMap initCompressedFmtMap()
{
    V4l2PixFmtMap compressedToFF;

    // compressed formats
    compressedToFF[V4L2_PIX_FMT_MJPEG] = "mjpeg";
    compressedToFF[V4L2_PIX_FMT_JPEG] = "mjpeg";
    //compressedToFF[V4L2_PIX_FMT_DV] = "";
    //compressedToFF[V4L2_PIX_FMT_MPEG] = "";
    compressedToFF[V4L2_PIX_FMT_H264] = "h264";
    compressedToFF[V4L2_PIX_FMT_H264_NO_SC] = "h264";

#ifdef V4L2_PIX_FMT_H264_MVC
    compressedToFF[V4L2_PIX_FMT_H264_MVC] = "h264";
#endif

    compressedToFF[V4L2_PIX_FMT_H263] = "h263";
    compressedToFF[V4L2_PIX_FMT_MPEG1] = "mpeg1video";
    compressedToFF[V4L2_PIX_FMT_MPEG2] = "mpeg2video";
    compressedToFF[V4L2_PIX_FMT_MPEG4] = "mpeg4";
    //compressedToFF[V4L2_PIX_FMT_XVID] = "";
    compressedToFF[V4L2_PIX_FMT_VC1_ANNEX_G] = "vc1";
    compressedToFF[V4L2_PIX_FMT_VC1_ANNEX_L] = "vc1";

#ifdef V4L2_PIX_FMT_VP8
    compressedToFF[V4L2_PIX_FMT_VP8] = "vp8";
#endif

    //  Vendor-specific formats
    compressedToFF[V4L2_PIX_FMT_CPIA1] = "cpia";

    return compressedToFF;
}

Q_GLOBAL_STATIC_WITH_ARGS(V4l2PixFmtMap, compressedToFF, (initCompressedFmtMap()))

typedef QMap<v4l2_ctrl_type, QString> V4l2CtrlTypeMap;

inline V4l2CtrlTypeMap initV4l2CtrlTypeMap()
{
    V4l2CtrlTypeMap ctrlTypeToStr;

    // V4L2 controls
    ctrlTypeToStr[V4L2_CTRL_TYPE_INTEGER] = "integer";
    ctrlTypeToStr[V4L2_CTRL_TYPE_BOOLEAN] = "boolean";
    ctrlTypeToStr[V4L2_CTRL_TYPE_MENU] = "menu";
    ctrlTypeToStr[V4L2_CTRL_TYPE_BUTTON] = "button";
    ctrlTypeToStr[V4L2_CTRL_TYPE_INTEGER64] = "integer64";
    ctrlTypeToStr[V4L2_CTRL_TYPE_CTRL_CLASS] = "ctrlClass";
    ctrlTypeToStr[V4L2_CTRL_TYPE_STRING] = "string";
    ctrlTypeToStr[V4L2_CTRL_TYPE_BITMASK] = "bitmask";
    ctrlTypeToStr[V4L2_CTRL_TYPE_INTEGER_MENU] = "integerMenu";

    return ctrlTypeToStr;
}

Q_GLOBAL_STATIC_WITH_ARGS(V4l2CtrlTypeMap, ctrlTypeToStr, (initV4l2CtrlTypeMap()))

typedef QMap<Capture::IoMethod, QString> IoMethodMap;

inline IoMethodMap initIoMethodMap()
{
    IoMethodMap ioMethodToStr;
    ioMethodToStr[Capture::IoMethodReadWrite] = "readWrite";
    ioMethodToStr[Capture::IoMethodMemoryMap] = "memoryMap";
    ioMethodToStr[Capture::IoMethodUserPointer] = "userPointer";

    return ioMethodToStr;
}

Q_GLOBAL_STATIC_WITH_ARGS(IoMethodMap, ioMethodToStr, (initIoMethodMap()))

Capture::Capture(): QObject()
{
    this->m_fd = -1;
    this->m_id = -1;
    this->m_ioMethod = IoMethodUnknown;
    this->m_nBuffers = 32;

    this->m_webcams = this->webcams();
    this->m_fsWatcher = new QFileSystemWatcher(QStringList() << "/dev");
    this->m_fsWatcher->setParent(this);

    QObject::connect(this->m_fsWatcher,
                     &QFileSystemWatcher::directoryChanged,
                     this,
                     &Capture::onDirectoryChanged);
}

Capture::~Capture()
{
    delete this->m_fsWatcher;
}

QStringList Capture::webcams() const
{
    QDir devicesDir("/dev");

    QStringList devices = devicesDir.entryList(QStringList() << "video*",
                                               QDir::System
                                               | QDir::Readable
                                               | QDir::Writable
                                               | QDir::NoSymLinks
                                               | QDir::NoDotAndDotDot
                                               | QDir::CaseSensitive,
                                               QDir::Name);

    QStringList webcams;
    QFile device;
    v4l2_capability capability;
    memset(&capability, 0, sizeof(v4l2_capability));

    foreach (QString devicePath, devices) {
        device.setFileName(devicesDir.absoluteFilePath(devicePath));

        if (device.open(QIODevice::ReadWrite)) {
            this->xioctl(device.handle(), VIDIOC_QUERYCAP, &capability);

            if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)
                webcams << device.fileName();

            device.close();
        }
    }

    return webcams;
}

QString Capture::device() const
{
    return this->m_device;
}

QString Capture::ioMethod() const
{
    return ioMethodToStr->value(this->m_ioMethod, "any");
}

int Capture::nBuffers() const
{
    return this->m_nBuffers;
}

bool Capture::isCompressed() const
{
    return false;
}

AkCaps Capture::caps(v4l2_format *format, bool *changePxFmt) const
{
    if (this->m_caps)
        return this->m_caps;

    bool closeFd = false;
    int fd = this->m_fd;

    if (fd < 0) {
        fd = open(this->m_device.toStdString().c_str(), O_RDWR);

        v4l2_capability capabilities;
        memset(&capabilities, 0, sizeof(v4l2_capability));

        if (this->xioctl(fd, VIDIOC_QUERYCAP, &capabilities) < 0) {
            qDebug() << "VideoCapture: Can't query capabilities.";

            if (closeFd)
                close(fd);

            return AkCaps();
        }

        closeFd = true;
    }

    v4l2_format fmt;
    memset(&fmt, 0, sizeof(v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (this->xioctl(fd, VIDIOC_G_FMT, &fmt) < 0) {
        qDebug() << "VideoCapture: Can't get default input format.";

        if (closeFd)
            close(fd);

        return AkCaps();
    }

    if (!rawToFF->contains(fmt.fmt.pix.pixelformat)) {
        quint32 pixelFormat = this->defaultFormat(fd, false);

        if (pixelFormat) {
            fmt.fmt.pix.pixelformat = pixelFormat;

            if (changePxFmt)
                *changePxFmt = true;
        }
        else {
            qDebug() << "VideoCapture: Doesn't support format:" << this->fourccToStr(fmt.fmt.pix.pixelformat);

            if (closeFd)
                close(fd);

            return AkCaps();
        }
    }

    if (format)
        memcpy(format, &fmt, sizeof(v4l2_format));

    AkVideoCaps caps;
    caps.isValid() = true;
    caps.format() = AkVideoCaps::pixelFormatFromString(this->v4l2ToFF(fmt.fmt.pix.pixelformat));
    caps.bpp() = AkVideoCaps::bitsPerPixel(caps.format());
    caps.width() = fmt.fmt.pix.width;
    caps.height() = fmt.fmt.pix.height;
    caps.fps() = this->fps(fd);

    if (closeFd)
        close(fd);

    return caps.toCaps();
}

QString Capture::description(const QString &webcam) const
{
    if (webcam.isEmpty())
        return QString();

    QFile device;
    v4l2_capability capability;
    memset(&capability, 0, sizeof(v4l2_capability));

    device.setFileName(webcam);

    if (device.open(QIODevice::ReadWrite)) {
        this->xioctl(device.handle(), VIDIOC_QUERYCAP, &capability);

        if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)
            return QString((const char *) capability.card);

        device.close();
    }

    return QString();
}

QVariantList Capture::availableSizes(const QString &webcam) const
{
    QFile device(webcam);
    QVariantList sizeList;

    if (!device.open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        return sizeList;

    QList<v4l2_buf_type> bufType;

    bufType << V4L2_BUF_TYPE_VIDEO_CAPTURE
            << V4L2_BUF_TYPE_VIDEO_OUTPUT
            << V4L2_BUF_TYPE_VIDEO_OVERLAY;

    foreach (v4l2_buf_type type, bufType) {
        v4l2_fmtdesc fmt;
        memset(&fmt, 0, sizeof(v4l2_fmtdesc));
        fmt.index = 0;
        fmt.type = type;

        while (this->xioctl(device.handle(), VIDIOC_ENUM_FMT, &fmt) >= 0) {
            v4l2_frmsizeenum frmsize;
            memset(&frmsize, 0, sizeof(v4l2_frmsizeenum));
            frmsize.pixel_format = fmt.pixelformat;
            frmsize.index = 0;

            while (this->xioctl(device.handle(),
                         VIDIOC_ENUM_FRAMESIZES,
                         &frmsize) >= 0) {
                if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                    QSize size(frmsize.discrete.width,
                               frmsize.discrete.height);

                    if (!sizeList.contains(size))
                        sizeList << size;
                }

                frmsize.index++;
            }

            fmt.index++;
        }
    }

    device.close();

    return sizeList;
}

QSize Capture::size(const QString &webcam) const
{
    QFile deviceFile(webcam);
    QSize size;

    if (!deviceFile.open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        return size;

    v4l2_format fmt;
    memset(&fmt, 0, sizeof(v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (this->xioctl(deviceFile.handle(), VIDIOC_G_FMT, &fmt) >= 0)
        size = QSize(fmt.fmt.pix.width,
                     fmt.fmt.pix.height);

    deviceFile.close();

    return size;
}

bool Capture::setSize(const QString &webcam, const QSize &size)
{
    QFile deviceFile(webcam);

    if (!deviceFile.open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        return false;

    v4l2_format fmt;
    memset(&fmt, 0, sizeof(v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (this->xioctl(deviceFile.handle(), VIDIOC_G_FMT, &fmt) == 0) {
        fmt.fmt.pix.width = size.width();
        fmt.fmt.pix.height = size.height();
        fmt.fmt.pix.pixelformat = this->format(webcam, size);

        this->xioctl(deviceFile.handle(), VIDIOC_S_FMT, &fmt);
    }

    deviceFile.close();

    emit this->sizeChanged(webcam, size);

    return true;
}

bool Capture::resetSize(const QString &webcam)
{
    return this->setSize(webcam, this->availableSizes(webcam)[0].toSize());
}

QVariantList Capture::imageControls(const QString &webcam) const
{
    return this->controls(webcam, V4L2_CTRL_CLASS_USER);
}

bool Capture::setImageControls(const QString &webcam, const QVariantMap &controls) const
{
    QFile device(webcam);

    if (!device.open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        return false;

    QMap<QString, uint> ctrl2id = this->findImageControls(device.handle());
    QVector<v4l2_ext_control> mpegCtrls;
    QVector<v4l2_ext_control> userCtrls;

    foreach (QString control, controls.keys()) {
        v4l2_ext_control ctrl;
        ctrl.id = ctrl2id[control];
        ctrl.value = controls[control].toInt();

        if (V4L2_CTRL_ID2CLASS(ctrl.id) == V4L2_CTRL_CLASS_MPEG)
            mpegCtrls << ctrl;
        else
            userCtrls << ctrl;
    }

    foreach (v4l2_ext_control user_ctrl, userCtrls) {
        v4l2_control ctrl;
        memset(&ctrl, 0, sizeof(v4l2_control));
        ctrl.id = user_ctrl.id;
        ctrl.value = user_ctrl.value;
        this->xioctl(device.handle(), VIDIOC_S_CTRL, &ctrl);
    }

    if (!mpegCtrls.isEmpty()) {
        v4l2_ext_controls ctrls;
        memset(&ctrls, 0, sizeof(v4l2_ext_control));
        ctrls.ctrl_class = V4L2_CTRL_CLASS_MPEG;
        ctrls.count = mpegCtrls.size();
        ctrls.controls = &mpegCtrls[0];
        this->xioctl(device.handle(), VIDIOC_S_EXT_CTRLS, &ctrls);
    }

    device.close();

    emit this->imageControlsChanged(webcam, controls);

    return true;
}

bool Capture::resetImageControls(const QString &webcam) const
{
    QVariantMap controls;

    foreach (QVariant control, this->imageControls(webcam)) {
        QVariantList params = control.toList();

        controls[params[0].toString()] = params[5].toInt();
    }

    return this->setImageControls(webcam, controls);
}

QVariantList Capture::cameraControls(const QString &webcam) const
{
    return this->controls(webcam, V4L2_CTRL_CLASS_CAMERA);
}

bool Capture::setCameraControls(const QString &webcam, const QVariantMap &cameraControls) const
{
    return this->setImageControls(webcam, cameraControls);
}

bool Capture::resetCameraControls(const QString &webcam) const
{
    return this->resetImageControls(webcam);
}

AkPacket Capture::readFrame()
{
    if (this->m_buffers.isEmpty())
        return AkPacket();

    int fd = this->m_fd;

    if (fd < 0)
        return AkPacket();

    if (this->m_ioMethod == IoMethodReadWrite) {
        if (read(fd, this->m_buffers[0].start, this->m_buffers[0].length) < 0)
            return AkPacket();

        timeval timestamp;
        gettimeofday(&timestamp, NULL);

        qint64 pts = (timestamp.tv_sec
                      + 1e-6 * timestamp.tv_usec)
                       * this->m_fps.value();

        return this->processFrame(this->m_buffers[0].start,
                                  this->m_buffers[0].length,
                                  pts);
    }
    else if (this->m_ioMethod == IoMethodMemoryMap
             || this->m_ioMethod == IoMethodUserPointer) {
        v4l2_buffer buffer;
        memset(&buffer, 0, sizeof(buffer));

        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        buffer.memory = (this->m_ioMethod == IoMethodMemoryMap)?
                            V4L2_MEMORY_MMAP:
                            V4L2_MEMORY_USERPTR;

        if (this->xioctl(fd, VIDIOC_DQBUF, &buffer) < 0)
            return AkPacket();

        if (buffer.index >= (quint32) this->m_buffers.size())
            return AkPacket();

        qint64 pts = (buffer.timestamp.tv_sec
                       + 1e-6 * buffer.timestamp.tv_usec)
                        * this->m_fps.value();

        AkPacket packet = this->processFrame(this->m_buffers[buffer.index].start,
                                             buffer.bytesused,
                                             pts);

        if (this->xioctl(fd, VIDIOC_QBUF, &buffer) < 0)
            return AkPacket();

        return packet;
    }

    return AkPacket();
}

quint32 Capture::defaultFormat(int fd, bool compressed) const
{
    v4l2_fmtdesc fmtdesc;

    for (int i = 0; ; i++) {
        memset(&fmtdesc, 0, sizeof(fmtdesc));
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmtdesc.index = i;

        if (this->xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) < 0)
            break;

        bool isCompressed = (fmtdesc.flags & V4L2_FMT_FLAG_COMPRESSED)? true: false;

        if ((isCompressed && compressed)
            || (!isCompressed && !compressed))
            if ((!compressed && rawToFF->contains(fmtdesc.pixelformat))
                || (compressed && compressedToFF->contains(fmtdesc.pixelformat)))
                return fmtdesc.pixelformat;
    }

    return 0;
}

QString Capture::v4l2ToFF(quint32 fmt) const
{
    if (rawToFF->contains(fmt))
        return rawToFF->value(fmt);

    return compressedToFF->value(fmt, QString());
}

AkFrac Capture::fps(int fd) const
{
    AkFrac fps;
    v4l2_std_id stdId;

    if (this->xioctl(fd, VIDIOC_G_STD, &stdId) >= 0) {
        v4l2_standard standard;
        memset(&standard, 0, sizeof(standard));

        standard.index = 0;

        while (this->xioctl(fd, VIDIOC_ENUMSTD, &standard) == 0) {
            if (standard.id & stdId) {
                fps = AkFrac(standard.frameperiod.denominator,
                             standard.frameperiod.numerator);

                break;
            }

            standard.index++;
        }
    }

    v4l2_streamparm streamparm;
    memset(&streamparm, 0, sizeof(streamparm));

    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (this->xioctl(fd, VIDIOC_G_PARM, &streamparm) >= 0) {
        if (streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
            fps = AkFrac(streamparm.parm.capture.timeperframe.denominator,
                         streamparm.parm.capture.timeperframe.numerator);
    }

    return fps;
}

quint32 Capture::format(const QString &webcam, const QSize &size) const
{
    QFile device(webcam);

    if (!device.open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        return 0;

    QList<v4l2_buf_type> bufType;

    bufType << V4L2_BUF_TYPE_VIDEO_CAPTURE
            << V4L2_BUF_TYPE_VIDEO_OUTPUT
            << V4L2_BUF_TYPE_VIDEO_OVERLAY;

    foreach (v4l2_buf_type type, bufType)
    {
        v4l2_fmtdesc fmt;
        memset(&fmt, 0, sizeof(v4l2_fmtdesc));
        fmt.index = 0;
        fmt.type = type;

        while (this->xioctl(device.handle(), VIDIOC_ENUM_FMT, &fmt) >= 0) {
            v4l2_frmsizeenum frmsize;
            memset(&frmsize, 0, sizeof(v4l2_frmsizeenum));
            frmsize.pixel_format = fmt.pixelformat;
            frmsize.index = 0;

            while (this->xioctl(device.handle(),
                         VIDIOC_ENUM_FRAMESIZES,
                         &frmsize) >= 0) {
                if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
                    if (QSize(frmsize.discrete.width,
                              frmsize.discrete.height) == size) {
                        device.close();

                        return fmt.pixelformat;
                    }

                frmsize.index++;
            }

            fmt.index++;
        }
    }

    device.close();

    return 0;
}

QVariantList Capture::controls(const QString &webcam, quint32 controlClass) const
{
    QVariantList controls;

    QFile device(webcam);

    if (!device.open(QIODevice::ReadWrite | QIODevice::Unbuffered))
        return controls;

    v4l2_queryctrl queryctrl;
    memset(&queryctrl, 0, sizeof(v4l2_queryctrl));
    queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;

    while (this->xioctl(device.handle(), VIDIOC_QUERYCTRL, &queryctrl) == 0) {
        QVariantList control = this->queryControl(device.handle(), controlClass, &queryctrl);

        if (!control.isEmpty())
            controls << QVariant(control);

        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }

    if (queryctrl.id != V4L2_CTRL_FLAG_NEXT_CTRL) {
        device.close();

        return controls;
    }

    for (int id = V4L2_CID_USER_BASE; id < V4L2_CID_LASTP1; id++) {
        queryctrl.id = id;

        if (this->xioctl(device.handle(), VIDIOC_QUERYCTRL, &queryctrl) == 0) {
            QVariantList control = this->queryControl(device.handle(), controlClass, &queryctrl);

            if (!control.isEmpty())
                controls << QVariant(control);
        }
    }

    for (queryctrl.id = V4L2_CID_PRIVATE_BASE;
         this->xioctl(device.handle(), VIDIOC_QUERYCTRL, &queryctrl) == 0;
         queryctrl.id++) {
        QVariantList control = this->queryControl(device.handle(), controlClass, &queryctrl);

        if (!control.isEmpty())
            controls << QVariant(control);
    }

    device.close();

    return controls;
}

QVariantList Capture::queryControl(int handle, quint32 controlClass, v4l2_queryctrl *queryctrl) const
{
    if (queryctrl->flags & V4L2_CTRL_FLAG_DISABLED)
        return QVariantList();

    if (V4L2_CTRL_ID2CLASS(queryctrl->id) != controlClass)
        return QVariantList();

    v4l2_ext_control ext_ctrl;
    memset(&ext_ctrl, 0, sizeof(v4l2_ext_control));
    ext_ctrl.id = queryctrl->id;

    v4l2_ext_controls ctrls;
    memset(&ctrls, 0, sizeof(v4l2_ext_controls));
    ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(queryctrl->id);
    ctrls.count = 1;
    ctrls.controls = &ext_ctrl;

    if (V4L2_CTRL_ID2CLASS(queryctrl->id) != V4L2_CTRL_CLASS_USER &&
        queryctrl->id < V4L2_CID_PRIVATE_BASE) {
        if (this->xioctl(handle, VIDIOC_G_EXT_CTRLS, &ctrls))
            return QVariantList();
    }
    else {
        v4l2_control ctrl;
        memset(&ctrl, 0, sizeof(v4l2_control));
        ctrl.id = queryctrl->id;

        if (this->xioctl(handle, VIDIOC_G_CTRL, &ctrl))
            return QVariantList();

        ext_ctrl.value = ctrl.value;
    }

    v4l2_querymenu qmenu;
    memset(&qmenu, 0, sizeof(v4l2_querymenu));
    qmenu.id = queryctrl->id;
    QStringList menu;

    if (queryctrl->type == V4L2_CTRL_TYPE_MENU)
        for (int i = 0; i < queryctrl->maximum + 1; i++) {
            qmenu.index = i;

            if (this->xioctl(handle, VIDIOC_QUERYMENU, &qmenu))
                continue;

            menu << QString((const char *) qmenu.name);
        }

    v4l2_ctrl_type type = static_cast<v4l2_ctrl_type>(queryctrl->type);

    return QVariantList() << QString((const char *) queryctrl->name)
                          << ctrlTypeToStr->value(type)
                          << queryctrl->minimum
                          << queryctrl->maximum
                          << queryctrl->step
                          << queryctrl->default_value
                          << ext_ctrl.value
                          << menu;
}

QMap<QString, quint32> Capture::findImageControls(int handle) const
{
    v4l2_queryctrl qctrl;
    memset(&qctrl, 0, sizeof(v4l2_queryctrl));
    qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    QMap<QString, quint32> controls;

    while (this->xioctl(handle, VIDIOC_QUERYCTRL, &qctrl) == 0) {
        if (!(qctrl.flags & V4L2_CTRL_FLAG_DISABLED))
            controls[QString((const char *) qctrl.name)] = qctrl.id;

        qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }

    if (qctrl.id != V4L2_CTRL_FLAG_NEXT_CTRL)
        return controls;

    for (int id = V4L2_CID_USER_BASE; id < V4L2_CID_LASTP1; id++) {
        qctrl.id = id;

        if (this->xioctl(handle, VIDIOC_QUERYCTRL, &qctrl) == 0 &&
           !(qctrl.flags & V4L2_CTRL_FLAG_DISABLED))
            controls[QString((const char *) qctrl.name)] = qctrl.id;
    }

    qctrl.id = V4L2_CID_PRIVATE_BASE;

    while (this->xioctl(handle, VIDIOC_QUERYCTRL, &qctrl) == 0) {
        if (!(qctrl.flags & V4L2_CTRL_FLAG_DISABLED))
            controls[QString((const char *) qctrl.name)] = qctrl.id;

        qctrl.id++;
    }

    return controls;
}

bool Capture::initReadWrite(quint32 bufferSize)
{
    this->m_buffers.resize(1);

    this->m_buffers[0].length = bufferSize;
    this->m_buffers[0].start = new char[bufferSize];

    if (!this->m_buffers[0].start) {
        this->m_buffers.clear();

        return false;
    }

    return true;
}

bool Capture::initMemoryMap()
{
    v4l2_requestbuffers requestBuffers;
    memset(&requestBuffers, 0, sizeof(requestBuffers));

    requestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestBuffers.memory = V4L2_MEMORY_MMAP;
    requestBuffers.count = this->m_nBuffers;

    if (this->xioctl(this->m_fd, VIDIOC_REQBUFS, &requestBuffers) < 0)
        return false;

    if (requestBuffers.count < 1)
        return false;

    this->m_buffers.resize(requestBuffers.count);
    bool error = false;

    for (quint32 i = 0; i < requestBuffers.count; i++) {
        v4l2_buffer buffer;
        memset(&buffer, 0, sizeof(buffer));

        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        if (this->xioctl(this->m_fd, VIDIOC_QUERYBUF, &buffer) < 0) {
            error = true;

            break;
        }

        this->m_buffers[i].length = buffer.length;

        this->m_buffers[i].start = (char *) mmap(NULL,
                                                 buffer.length,
                                                 PROT_READ | PROT_WRITE,
                                                 MAP_SHARED,
                                                 this->m_fd,
                                                 buffer.m.offset);

        if (this->m_buffers[i].start == MAP_FAILED) {
            error = true;

            break;
        }
    }

    if (error) {
        for (qint32 i = 0; i < this->m_buffers.size(); i++)
            munmap(this->m_buffers[i].start, this->m_buffers[i].length);

        this->m_buffers.clear();

        return false;
    }

    return true;
}

bool Capture::initUserPointer(quint32 bufferSize)
{
    v4l2_requestbuffers requestBuffers;
    memset(&requestBuffers, 0, sizeof(requestBuffers));

    requestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestBuffers.memory = V4L2_MEMORY_USERPTR;
    requestBuffers.count = this->m_nBuffers;

    if (this->xioctl(this->m_fd, VIDIOC_REQBUFS, &requestBuffers) < 0)
        return false;

    this->m_buffers.resize(requestBuffers.count);
    bool error = false;

    for (quint32 i = 0; i < requestBuffers.count; i++) {
        this->m_buffers[i].length = bufferSize;
        this->m_buffers[i].start = new char[bufferSize];

        if (!this->m_buffers[i].start) {
            error = true;

            break;
        }
    }

    if (error) {
        for (qint32 i = 0; i < this->m_buffers.size(); i++)
            delete this->m_buffers[i].start;

        this->m_buffers.clear();

        return false;
    }

    return true;
}

bool Capture::startCapture()
{
    bool error = false;

    if (this->m_ioMethod == IoMethodMemoryMap) {
        for (qint32 i = 0; i < this->m_buffers.size(); i++) {
            v4l2_buffer buffer;
            memset(&buffer, 0, sizeof(buffer));

            buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buffer.memory = V4L2_MEMORY_MMAP;
            buffer.index = i;

            if (this->xioctl(this->m_fd, VIDIOC_QBUF, &buffer) < 0)
                error = true;
        }

        v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (this->xioctl(this->m_fd, VIDIOC_STREAMON, &type) < 0)
            error = true;
    }
    else if (this->m_ioMethod == IoMethodUserPointer) {
        for (qint32 i = 0; i < this->m_buffers.size(); i++) {
            v4l2_buffer buffer;
            memset(&buffer, 0, sizeof(buffer));

            buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buffer.memory = V4L2_MEMORY_USERPTR;
            buffer.index = i;
            buffer.m.userptr = (unsigned long) this->m_buffers[i].start;
            buffer.length = this->m_buffers[i].length;

            if (this->xioctl(this->m_fd, VIDIOC_QBUF, &buffer) < 0)
                error = true;
        }

        v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (this->xioctl(this->m_fd, VIDIOC_STREAMON, &type) < 0)
            error = true;
    }

    if (error)
        this->uninit();

    this->m_id = Ak::id();

    return !error;
}

void Capture::stopCapture()
{
    if (this->m_ioMethod == IoMethodMemoryMap
        || this->m_ioMethod == IoMethodUserPointer) {
        v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        this->xioctl(this->m_fd, VIDIOC_STREAMOFF, &type);
    }
}

bool Capture::isCompressedFormat(quint32 format)
{
    v4l2_fmtdesc fmtdesc;

    for (int i = 0; ; i++) {
        memset(&fmtdesc, 0, sizeof(fmtdesc));
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmtdesc.index = i;

        if (this->xioctl(this->m_fd, VIDIOC_ENUM_FMT, &fmtdesc) < 0)
            break;

        if (fmtdesc.pixelformat == format)
            return (fmtdesc.flags & V4L2_FMT_FLAG_COMPRESSED)? true: false;
    }

    return false;
}

bool Capture::init()
{
    this->m_fd = open(this->m_device.toStdString().c_str(), O_RDWR);

    v4l2_capability capabilities;
    memset(&capabilities, 0, sizeof(v4l2_capability));

    if (this->xioctl(this->m_fd, VIDIOC_QUERYCAP, &capabilities) < 0) {
        qDebug() << "VideoCapture: Can't query capabilities.";
        close(this->m_fd);

        return false;
    }

    v4l2_format fmt;
    memset(&fmt, 0, sizeof(v4l2_format));
    bool changePxFmt = false;

    AkCaps caps = this->caps(&fmt, &changePxFmt);

    if (changePxFmt && this->xioctl(this->m_fd, VIDIOC_S_FMT, &fmt) < 0) {
        qDebug() << "VideoCapture: Can't set format:" << this->fourccToStr(fmt.fmt.pix.pixelformat);
        close(this->m_fd);

        return false;
    }

    this->m_caps = caps;
    this->m_fps = caps.property("fps").toString();
    this->m_timeBase = this->m_fps.invert();

    if (this->m_ioMethod == IoMethodReadWrite
        && capabilities.capabilities & V4L2_CAP_READWRITE
        && this->initReadWrite(fmt.fmt.pix.sizeimage)) {
    }
    else if (this->m_ioMethod == IoMethodMemoryMap
             && capabilities.capabilities & V4L2_CAP_STREAMING
             && this->initMemoryMap()) {
    }
    else if (this->m_ioMethod == IoMethodUserPointer
             && capabilities.capabilities & V4L2_CAP_STREAMING
             && this->initUserPointer(fmt.fmt.pix.sizeimage)) {
    }
    else
        this->m_ioMethod = IoMethodUnknown;

    if (this->m_ioMethod != IoMethodUnknown)
        return this->startCapture();

    if (capabilities.capabilities & V4L2_CAP_READWRITE && this->initReadWrite(fmt.fmt.pix.sizeimage))
        this->m_ioMethod = IoMethodReadWrite;
    else if (capabilities.capabilities & V4L2_CAP_STREAMING) {
        if (this->initMemoryMap())
            this->m_ioMethod = IoMethodMemoryMap;
        else if (this->initUserPointer(fmt.fmt.pix.sizeimage))
            this->m_ioMethod = IoMethodUserPointer;
        else {
            this->m_ioMethod = IoMethodUnknown;

            return false;
        }
    }
    else
        return false;

    return this->startCapture();
}

void Capture::uninit()
{
    this->stopCapture();

    if (!this->m_buffers.isEmpty()) {
        if (this->m_ioMethod == IoMethodReadWrite)
            delete this->m_buffers[0].start;
        else if (this->m_ioMethod == IoMethodMemoryMap)
            for (qint32 i = 0; i < this->m_buffers.size(); i++)
                munmap(this->m_buffers[i].start, this->m_buffers[i].length);
        else if (this->m_ioMethod == IoMethodUserPointer)
            for (qint32 i = 0; i < this->m_buffers.size(); i++)
                delete this->m_buffers[i].start;
    }

    close(this->m_fd);
    this->m_caps.clear();
    this->m_fps = AkFrac();
    this->m_timeBase = AkFrac();
    this->m_fd = -1;
    this->m_buffers.clear();
}

void Capture::setDevice(const QString &device)
{
    if (this->m_device == device)
        return;

    this->m_device = device;
    emit this->deviceChanged(device);
}

void Capture::setIoMethod(const QString &ioMethod)
{
    if (this->m_fd >= 0)
        return;

    IoMethod ioMethodEnum = ioMethodToStr->key(ioMethod, IoMethodUnknown);

    if (this->m_ioMethod == ioMethodEnum)
        return;

    this->m_ioMethod = ioMethodEnum;
    emit this->ioMethodChanged(ioMethod);
}

void Capture::setNBuffers(int nBuffers)
{
    if (this->m_nBuffers == nBuffers)
        return;

    this->m_nBuffers = nBuffers;
    emit this->nBuffersChanged(nBuffers);
}

void Capture::resetDevice()
{
    this->setDevice("");
}

void Capture::resetIoMethod()
{
    this->setIoMethod("any");
}

void Capture::resetNBuffers()
{
    this->setNBuffers(32);
}

void Capture::reset(const QString &webcam)
{
    QStringList webcams;

    if (webcam.isEmpty())
        webcams = this->webcams();
    else
        webcams << webcam;

    foreach (QString webcam, webcams) {
        this->resetSize(webcam);
        this->resetImageControls(webcam);
        this->resetCameraControls(webcam);
    }
}

void Capture::onDirectoryChanged(const QString &path)
{
    Q_UNUSED(path)

    QStringList webcams = this->webcams();

    if (webcams != this->m_webcams) {
        emit this->webcamsChanged(webcams);

        this->m_webcams = webcams;
    }
}
