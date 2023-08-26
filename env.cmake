if(WIN32)
    set(VCPKG_ROOT C:/source/vcpkg)
    set(QT_ROOT C:/Qt/Qt6.4/6.4.0/msvc2019_64)

#    set(VCPKG_ROOT G:/vcpkg)
#    set(QT_ROOT F:/qt6.4/6.4.0/msvc2019_64)

    set(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)
elseif (APPLE)

    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

    set(QT_ROOT /Users/shark/Qt/6.5.0/macos)
    set(VCPKG_ROOT /Users/shark/source/vcpkg)

elseif (UNIX)

endif ()