%bcond_with x
%bcond_with wayland

%define _optdir /opt
%define _appdir %{_optdir}/apps

Name:       libscl-core
Summary:    A library for developing software keyboards
Version:    0.4.14
Release:    1
Group:      Graphics & UI Framework/Input
License:    Apache-2.0
Source0:    libscl-core-%{version}.tar.gz
BuildRequires:  gettext-tools
BuildRequires:  cmake
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(isf)
%if %{with wayland}
BuildRequires:  pkgconfig(ecore-wayland)
BuildRequires:  pkgconfig(wayland-client)
BuildRequires:  pkgconfig(input-method-client)
%else
BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(x11)
%endif
BuildRequires:  pkgconfig(libscl-common)
BuildRequires:  pkgconfig(capi-appfw-application)


%description
A library that helps developing S/W Keyboard

%package devel
Summary:    SCL-Core header file
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
A devel package of libscl-core library that helps developing S/W Keyboard

%prep
%setup -q


%build
export CFLAGS+=" -DTIZEN_DEBUG_ENABLE -Werror"
export CXXFLAGS+=" -DTIZEN_DEBUG_ENABLE -Werror"
export FFLAGS+=" -DTIZEN_DEBUG_ENABLE"

rm -rf CMakeFiles
rm -rf CMakeCache.txt

%if %{with wayland}
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DLIB_INSTALL_DIR:PATH=%{_libdir} -Dwith_wayland=TRUE
%else
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DLIB_INSTALL_DIR:PATH=%{_libdir}
%endif
make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%make_install



%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libscl-core.so
%license LICENSE

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/libscl-core.pc
