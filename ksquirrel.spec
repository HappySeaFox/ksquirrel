# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   OpenGL image viewer
Name:      ksquirrel
Version:   0.1.3
Release:   0.1.3
Copyright: GPL
Vendor:    ckult <squirrel-sf@yandex.ru>
Url:       http://ksquirrel.sf.net

Packager:  ckult <squirrel-sf@yandex.ru>
Group:     image viewer
Source:    ksquirrel-0.1.3.tar.gz
BuildRoot: /usr

%description


%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                 --build=i386-linux --host=i386-linux --target=i386-linux  \
                $LOCALFLAGS
%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.ksquirrel
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.ksquirrel
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.ksquirrel

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/ksquirrel
rm -rf ../file.list.ksquirrel


%files -f ../file.list.ksquirrel
