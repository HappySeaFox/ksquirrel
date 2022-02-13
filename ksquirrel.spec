Name: ksquirrel 
Summary: KSquirrel - image viewer for KDE 
Group: User Interface/Desktops 
Version: 0.6.0
Release: pre3
Copyright: GPL 
Source: %{name}-%{version}-%{release}.tar.bz2 
URL: http://ksquirrel.sf.net 
Packager: - 
Vendor: Baryshev Dmitry aka Krasu <ksquirrel@tut.by> 
BuildRoot: %{_tmppath}/%{name}-%{version}-root 
BuildRequires: gcc, gcc-c++, gettext 
BuildRequires: kdelibs-devel
Requires: kdelibs, libpng, libjpeg 

%description 
Ksquirrel is an image viewer for KDE implemented using OpenGL. 
You should have your videocard specific drivers been installed. 

%prep 

%setup -q

CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure --prefix="`kde-config --prefix`"

%build 
%__make %{?_smp_mflags} 

%install 
[ "$RPM_BUILD_ROOT" != "" ] && rm -rf $RPM_BUILD_ROOT 
%makeinstall 

%clean 
[ "$RPM_BUILD_ROOT" != "" ] && rm -rf $RPM_BUILD_ROOT 

%files 
%defattr(-,root,root) 
%doc README AUTHORS COPYING INSTALL
%{_bindir}/* 
%{_datadir}/applnk/Applications/* 
%lang(ru) %{_datadir}/locale/ru/LC_MESSAGES/* 
%lang(ru) %{_datadir}/locale/nl/LC_MESSAGES/* 
%{_datadir}/icons/hicolor/16x16/apps/* 
%{_datadir}/icons/hicolor/32x32/apps/* 
%dir %{_datadir}/apps/ksquirrel 
%{_datadir}/apps/ksquirrel 

%changelog 
