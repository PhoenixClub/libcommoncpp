
ACLOCAL_AMFLAGS = -I m4

SUBDIRS=

if MINGW
SUBDIRS += cbits
endif

SUBDIRS += libatomic libstacktrace libiconv-1.13.1 pcre-8.12 sqlite-3.7.5 \
	lib doc plugintool

if USE_CPPUNIT
SUBDIRS += tests
endif

EXTRA_DIST = *.sln *.vcproj *.vsprops config*.opts m4/*.m4 \
	commonc++.rc Resource.h deploy.bat \
	libiconv-1.13.1/configure.gnu libiconv-1.13.1/*.win32 \
	libiconv-1.13.1/*.vcproj libiconv-1.13.1/filelist.txt \
	pcre-8.12/configure.gnu pcre-8.12/*.win32 pcre-8.12/*.vcproj \
	plugintool/*.vcproj \
	sqlite-3.7.5/configure.gnu sqlite-3.7.5/*.vcproj \
	expat.vc8/examples/* expat.vc8/lib/* expat.vc8/xmlwf/*

dox:
	make -C ./lib dox

# TODO: inside each <File> tag for a '.c++' file, add this crap:
# <FileConfiguration Name="Debug|Win32">
#   <Tool Name="VCCLCompilerTool"/>
# </FileConfiguration>
# <FileConfiguration Name="Release|Win32">
#   <Tool Name="VCCLCompilerTool"/>
# </FileConfiguration>

