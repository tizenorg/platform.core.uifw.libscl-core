%define _optdir /opt
%define _appdir %{_optdir}/apps

Name:       libscl-core
Summary:    A library for developing software keyboards
Version:    0.0.1
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
BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(libscl-common)


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
export CFLAGS+=" -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS+=" -DTIZEN_DEBUG_ENABLE"
export FFLAGS+=" -DTIZEN_DEBUG_ENABLE"

rm -rf CMakeFiles
rm -rf CMakeCache.txt
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%make_install



%post

%postun

%files
%defattr(-,root,root,-)
%{_libdir}/libscl-core.so
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/libscl-core.pc
