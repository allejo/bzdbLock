lib_LTLIBRARIES = bzdbLock.la

bzdbLock_la_SOURCES = bzdbLock.cpp
bzdbLock_la_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
bzdbLock_la_LDFLAGS = -module -avoid-version -shared
bzdbLock_la_LIBADD = $(top_builddir)/plugins/plugin_utils/libplugin_utils.la

AM_CPPFLAGS = $(CONF_CPPFLAGS)
AM_CFLAGS = $(CONF_CFLAGS)
AM_CXXFLAGS = $(CONF_CXXFLAGS)

EXTRA_DIST = \
	LICENSE.md \
	README.md \
	bzdbLock.def \
	bzdbLock.ini \
	bzdbLock.sln \
	bzdbLock.vcxproj \
	bzdbLock.vcxproj.filters

MAINTAINERCLEANFILES =	\
	Makefile.in
