# virtual-environment-browser

How to Build

Step 1:
	Build and copy v8 into third_party/v8. See README.txt in third_party/v8. v8 is not added to this repository since it is too large.

Step 2:
	Install the static version of spdlog through vcpkg. For example, call
	.\vcpkg.exe install spdlog:x64-windows-static

Step 3:
	Create directory build, which is included in .gitignore,
	and inside build, run cmake with -DCMAKE_TOOLCHAIN_FILE, such as
	cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/path/to/this/repository/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static
	Unfortunately, setting -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake does not work and it has to be an absolute path.