# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   Ksquirrel - image viewer for KDE
Name:      ksquirrel
Version:   0.5.0
Release:   
Copyright: GPL
Vendor:    CKulT <squirrel-sf@yandex.ru>
Url:       http://ksquirrel.sf.net

Packager:  CKulT <squirrel-sf@yandex.ru>
Group:     Graphic
Source:    ksquirrel-0.5.0.tar.gz
BuildRoot: /usr

%description


%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                 \
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
