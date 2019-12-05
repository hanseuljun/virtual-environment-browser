How to create the files for v8

Finding the version to build:
	Use the one that chromium currently uses. At the time this file was written, it was 7.8.

To generate ninja files using gn:
	For Debug:
		call
			gn gen out.gn\x64.debug.msvc (from the top directory of v8 git repo and out.gn\x64.release.msvc is merely a path)
		set args.gn as below
			enable_iterator_debugging = true
			is_clang = false
			is_component_build = false
			is_debug = true
			symbol_level = 2 (without this, _ITERATOR_DEBUG_LEVEL of the lib files gets set to 0 and will not work this Debug runtime library of msvc)
			target_cpu = "x64"
		then call
			gn args out.gn\x64.debug.msvc (this updates ninja files based on the gn arguments)
	For Release:
		call
			gn gen out.gn\x64.release.msvc
		set args.gn as below
			is_clang = false
			is_component_build = false
			is_debug = false
			target_cpu = "x64"
		then call
			gn args out.gn\x64.release.msvc

	For the details of the gn arguments, consider using gn args --list <path>

Run ninja to build v8:
	call
		ninja -C <path>

Unfortunately, MSVC will not be able to find std::string from v8.h during compiling version 7.8 of v8.
This can be fixed by adding #include <string> to v8.h. 

MSVC prints the below warning, however it is okay.
	cl : Command line warning D9002 : ignoring unknown option '/Zc:twoPhase'
This option, which is set for clang, is applied to MSVC by default, and MSVC even requires explicitly setting /Zc:twoPhase- to turn this option off.
Therefore, nonetheless this warning exists, MSVC will be building v8 with two-phase name lookup for template name resolution. 
