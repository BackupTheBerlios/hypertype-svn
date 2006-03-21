# This spec file was generated by KDevelop
# Please report any problem to KDevelop Team <kdevelop-devel@kdevelop.org>
# Thanks to Matthias Saou for his explanations on http://freshrpms.net/docs/fight.html

Name: hypertype
Version: 0.2
Release: 2
Vendor: None
Copyright: GPL
Summary: HyperType is a typing tutor
Group: None
Packager: Josh Nisly
BuildRoot:  %{_tmppath}/%{name}-root
Source: %{name}-%{version}.tar.gz

%description


%prep
%setup -n %name-%version
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure

%build
%configure --enable-aggregator
make

%install
rm -rf %{buildroot}
%makeinstall

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog NEWS README
%{_bindir}/*
%{_datadir}/%{name}
%{_datadir}/applications/hypertype.desktop
%changelog

