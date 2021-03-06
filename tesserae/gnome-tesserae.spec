# Note that this is NOT a relocatable package
#
# (It isn't?  Okay...  This needs further investigation.)
#   - ndt@wagonfixers.com
#
%define ver      	1.4.0
%define RELEASE 	1
%define rel     	%{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define prefix   	/usr

Summary: Gnome Tesserae.
Name: 		gnome-tesserae
Version: 	%ver
Release: 	%rel
Copyright: 	GPL
Group: 		Amusements/Games
Source:		http://prdownloads.sourceforge.net/gtess/gnome-tesserae-%{PACKAGE_VERSION}.tar.gz
BuildRoot: 	/var/tmp/gnome-tesserae-%{PACKAGE_VERSION}-root
URL: 		http://gtess.sourceforge.net/
Docdir: 	%{prefix}/doc
Requires:	gnome-libs >= 1.0.60

%description
Gnome Tesserae is a tile game for GNOME.

%changelog
* Fri Mar 16 2001 Nathan Thompson-Amato <ndt@wagonfixers.com>

- Initial work turning iagno into gnome-tesserae.

%prep
%setup -q

%build
%ifarch alpha
  MYARCH_FLAGS="--host=alpha-redhat-linux"
%endif

CFLAGS="$RPM_OPT_FLAGS" ./configure $MYARCH_FLAGS --prefix=%prefix \
	 --localstatedir=/var/lib --sysconfdir=/etc

if [ "$SMP" != "" ]; then
  make -j$SMP MAKE="make -j$SMP"
else
  make
fi

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{prefix} scoredir=$RPM_BUILD_ROOT/var/lib/games sysconfdir=$RPM_BUILD_ROOT/etc install >install.log 2>&1

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%doc AUTHORS COPYING ChangeLog NEWS README
%attr(2111, root, games) %{prefix}/bin/gnome-tesserae
%{prefix}/share/gnome/apps/Games/*
%{prefix}/share/gnome/help/*
#%{prefix}/share/locale/*/*/*
%{prefix}/share/pixmaps/*
%{prefix}/share/sounds/*

#%{prefix}/lib/lib*.so.*
%defattr(-, games, games)
/var/lib/games/*
