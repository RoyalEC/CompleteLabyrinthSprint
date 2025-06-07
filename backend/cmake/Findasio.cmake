# Find standalone ASIO (header-only)
find_path(asio_INCLUDE_DIR NAMES asio.hpp
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES asio)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(asio DEFAULT_MSG asio_INCLUDE_DIR)

if (asio_FOUND)
    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE ${asio_INCLUDE_DIR})
endif()
