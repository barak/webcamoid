Name: webcamoid
Version: 8.0.0
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

Url: https://webcamoid.github.io/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-build
# AutoReqProv: no

%if %{defined fedora}
Requires: v4l2loopback

BuildRequires: fdupes
BuildRequires: make
BuildRequires: gcc-c++
BuildRequires: qt5-qttools-devel
BuildRequires: qt5-qtdeclarative-devel
BuildRequires: qt5-qtmultimedia-devel
BuildRequires: qt5-qtsvg-devel
BuildRequires: ffmpeg-devel
BuildRequires: gstreamer1-plugins-base-devel
BuildRequires: libv4l-devel
BuildRequires: alsa-lib-devel
BuildRequires: pulseaudio-libs-devel
BuildRequires: jack-audio-connection-kit-devel
%endif

%if %{defined suse_version}
Requires: v4l2loopback-utils

BuildRequires: fdupes
BuildRequires: update-desktop-files
BuildRequires: libqt5-linguist
BuildRequires: libqt5-qtbase-devel
BuildRequires: libqt5-qtdeclarative-devel
BuildRequires: libqt5-qtmultimedia-devel
BuildRequires: libqt5-qtsvg-devel
BuildRequires: ffmpeg-devel
BuildRequires: gstreamer-plugins-base-devel
BuildRequires: libv4l-devel
BuildRequires: alsa-devel
BuildRequires: libpulse-devel
BuildRequires: libjack-devel
%endif

Conflicts: plasmoid-webcamoid

%description
Webcamoid is a full featured webcam capture application.

Features:

    * Cross-platform (GNU/Linux, Windows)
    * Take pictures and record videos with the webcam.
    * Manages multiple webcams.
    * Written in C++/Qt.
    * Custom controls for each webcam.
    * Add funny effects to the webcam.
    * +60 effects available.
    * Translated to many languages.
    * Use custom network and local files as capture devices.
    * Capture from desktop.
    * Many recording formats.
    * Virtual webcam support for feeding other programs.

%package -n libavkys7
Version: 8.0.0
Summary: Modular audio and video processing library

%description -n libavkys7
LibAvKys is a Qt library that provides a wide range of plugins for audio and video playing, recording, capture, and processing.

%package -n akqml
Version: 8.0.0
Summary: Modular audio and video processing library (qml files)

%description -n akqml
LibAvKys is a Qt library that provides a wide range of plugins for audio and video playing, recording, capture, and processing.

%package -n libavkys-devel
Version: 8.0.0
Summary: Modular audio and video processing library (development files)

%if %{defined fedora}
Group: Development/Libraries
%endif

%if %{defined suse_version}
Group: Development/Libraries/Other
%endif

%description -n libavkys-devel
LibAvKys is a Qt library that provides a wide range of plugins for audio and video playing, recording, capture, and processing.

%package -n avkys-plugins
Version: 8.0.0
Summary: Modular audio and video processing library (plugins)

%description -n avkys-plugins
LibAvKys is a Qt library that provides a wide range of plugins for audio and video playing, recording, capture, and processing.

%prep
%setup -q -n %{name}-%{version}
# delete not needed files
find . -name ".gitignore" -exec rm {} \;

%build
%if %{defined fedora}
  qmake-qt5 Webcamoid.pro \
      LIBDIR=%{_libdir} \
      LICENSEDIR=%{_defaultdocdir}/webcamoid \
      INSTALLDEVHEADERS=1
%endif
%if %{defined suse_version}
  qmake-qt5 Webcamoid.pro \
      LIBDIR=%{_libdir} \
      LICENSEDIR=%{_defaultdocdir}/webcamoid \
      INSTALLDEVHEADERS=1
%endif
%if !%{defined fedora} && !%{defined suse_version}}
  qmake -qt=5 Webcamoid.pro \
      ROOT_METHOD=sudo \
      INSTALLDEVHEADERS=1
%endif

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make INSTALL_ROOT="%{buildroot}" install

%fdupes %{buildroot}

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%post -n libavkys7 -p /sbin/ldconfig
%postun -n libavkys7 -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_bindir}/webcamoid
%{_datadir}/applications/webcamoid.desktop
%{_datadir}/icons/hicolor/8x8/
%{_datadir}/icons/hicolor/8x8/apps/
%{_datadir}/icons/hicolor/8x8/apps/webcamoid.png
%{_datadir}/icons/hicolor/16x16/
%{_datadir}/icons/hicolor/16x16/apps/
%{_datadir}/icons/hicolor/16x16/apps/webcamoid.png
%{_datadir}/icons/hicolor/22x22/
%{_datadir}/icons/hicolor/22x22/apps/
%{_datadir}/icons/hicolor/22x22/apps/webcamoid.png
%{_datadir}/icons/hicolor/32x32/
%{_datadir}/icons/hicolor/32x32/apps/
%{_datadir}/icons/hicolor/32x32/apps/webcamoid.png
%{_datadir}/icons/hicolor/48x48/
%{_datadir}/icons/hicolor/48x48/apps/
%{_datadir}/icons/hicolor/48x48/apps/webcamoid.png
%{_datadir}/icons/hicolor/64x64/
%{_datadir}/icons/hicolor/64x64/apps/
%{_datadir}/icons/hicolor/64x64/apps/webcamoid.png
%{_datadir}/icons/hicolor/128x128/
%{_datadir}/icons/hicolor/128x128/apps/
%{_datadir}/icons/hicolor/128x128/apps/webcamoid.png
%{_datadir}/icons/hicolor/256x256/
%{_datadir}/icons/hicolor/256x256/apps/
%{_datadir}/icons/hicolor/256x256/apps/webcamoid.png
%{_datadir}/icons/hicolor/scalable/
%{_datadir}/icons/hicolor/scalable/apps/
%{_datadir}/icons/hicolor/scalable/apps/webcamoid.svg
%{_defaultdocdir}/webcamoid/
%{_defaultdocdir}/webcamoid/COPYING
%{_mandir}/man1/webcamoid.1.gz

%files -n libavkys7
%defattr(-,root,root,-)
%{_libdir}/libavkys.so.8.0.0
%{_defaultdocdir}/avkys/COPYING

%files -n akqml
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/AkQml/
%{_libdir}/qt5/qml/AkQml/libAkQml.so
%{_libdir}/qt5/qml/AkQml/qmldir

%files -n libavkys-devel
%defattr(-,root,root,-)
%{_includedir}/avkys
%{_includedir}/avkys/*.h
%{_libdir}/libavkys.so
%{_libdir}/libavkys.so.8
%{_libdir}/libavkys.so.8.0

%files -n avkys-plugins
%defattr(-,root,root,-)
%{_libdir}/avkys
%{_libdir}/avkys/lib*.so

%changelog
* Tue May 16 2017 Gonzalo Exequiel Pedone <hipersayan.x@gmail.com> 8.0.0-1
- New release
