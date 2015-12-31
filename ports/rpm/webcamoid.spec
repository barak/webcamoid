Name: webcamoid
Version: 6.2.0
Release: 1%{?dist}
Summary: The full webcam and multimedia suite

%if %{defined fedora}
Group: Applications/Multimedia
License: GPL
%endif

%if %{defined suse_version}
Group: Productivity/Multimedia/Video/Players
License: GPL-3.0+
%endif

%if %{defined mgaversion}
Group: Video/Utilities
License: GPLv3+
%endif

URL: https://github.com/hipersayanX/webcamoid
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-build
AutoReqProv: no

%if %{defined fedora}
BuildRequires: fdupes
BuildRequires: gcc-c++
BuildRequires: qt5-qtmultimedia-devel
BuildRequires: qt5-qttools-devel
BuildRequires: ffmpeg-devel
BuildRequires: libv4l-devel

Requires: qt5-qtmultimedia
Requires: ffmpeg-libs
Requires: libv4l
%endif

%if %{defined suse_version}
BuildRequires: fdupes
BuildRequires: libQt5Multimedia-devel
BuildRequires: libqt5-qttools
BuildRequires: libffmpeg-devel
BuildRequires: libv4l-devel

Requires: libQt5Multimedia5
Requires: libavcodec55
Requires: libavdevice55
Requires: libavfilter3
Requires: libavformat55
Requires: libavutil52
Requires: libpostproc52
Requires: libswresample0
Requires: libswscale2
Requires: libv4l1-0
Requires: libv4l2-0
Requires: libv4lconvert0
%endif

%if %{defined mgaversion}
BuildRequires: fdupes
BuildRequires: qttools5
BuildRequires: libv4l-devel

%ifarch i586
BuildRequires: libqt5widgets-devel
BuildRequires: libqt5multimedia-devel
BuildRequires: libqt5concurrent-devel
BuildRequires: libqt5xml-devel
BuildRequires: libffmpeg-devel
%else
BuildRequires: lib64qt5widgets-devel
BuildRequires: lib64qt5multimedia-devel
BuildRequires: lib64qt5concurrent-devel
BuildRequires: lib64qt5xml-devel
BuildRequires: lib64ffmpeg-devel
%endif

Requires: libv4l0

%ifarch i586
Requires: libqt5widgets5
Requires: libqt5multimedia5
Requires: libqt5concurrent5
Requires: libqt5xml5
Requires: libavcodec55
Requires: libavformat55
Requires: libavutil52
Requires: libpostproc52
Requires: libswscaler2
Requires: libavfilter3
Requires: libswresample0
%else
Requires: lib64qt5widgets5
Requires: lib64qt5multimedia5
Requires: lib64qt5concurrent5
Requires: lib64qt5xml5
Requires: lib64avcodec55
Requires: lib64avformat55
Requires: lib64avutil52
Requires: lib64postproc52
Requires: lib64swscaler2
Requires: lib64avfilter3
Requires: lib64swresample0
%endif
%endif

%description
Webcamoid is a full featured webcam capture application.

Features:

    * Take pictures and record videos with the webcam.
    * Manages multiple webcams.
    * Written in C++/Qt.
    * Custom controls for each webcam.
    * Add funny effects to the webcam.
    * +60 effects available.
    * Effects with live previews.
    * Translated to many languages.
    * Use custom network and local files as capture devices.
    * Capture from desktop.

%prep
%setup -q -n %{name}-%{version}

%build
%if %{defined fedora}
qmake-qt5 Webcamoid.pro \
    LIBDIR=%{_libdir} \
    LICENSEDIR=%{_defaultdocdir}/webcamoid
%endif

%if %{defined suse_version}
qmake-qt5 Webcamoid.pro \
    LIBDIR=%{_libdir} \
    LICENSEDIR=%{_defaultdocdir}/webcamoid
%endif

%if %{defined mgaversion}
%{_libdir}/qt5/bin/qmake Webcamoid.pro \
    LIBDIR=%{_libdir} \
    LICENSEDIR=%{_defaultdocdir}/webcamoid
%endif

make

%install
rm -rf %{buildroot}
make INSTALL_ROOT="%{buildroot}" install

%fdupes %{buildroot}

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_bindir}/webcamoid
%{_datadir}/applications/webcamoid.desktop
%{_datadir}/webcamoid/
%{_defaultdocdir}/webcamoid/
%{_includedir}/Qb/
%{_libdir}/Qb/
%{_libdir}/lib*Qb.so*
%{_libdir}/libWebcamoid.so*

%changelog
* Wed Feb 23 2015 Gonzalo Exequiel Pedone <hipersayan DOT x AT gmail DOT com> 6.2.0-1
- Final Release.
