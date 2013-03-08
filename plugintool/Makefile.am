
bin_PROGRAMS = plugintool

plugintool_SOURCES = \
	PluginTool.c++ PluginTool.h++ Main.c++

plugintool_CPPFLAGS = -DDEBUG -I../lib

plugintool_LDADD = -L. \
	-L../lib/.libs -lcommonc++

man_MANS = plugintool.1
