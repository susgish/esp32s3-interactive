cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Build only main and its dependencies. This keeps unrelated components
# (unity, protocomm, spiffs, bt, wifi, ...) out of the build, whose
# third-party submodules cannot be fetched in this offline environment.
set(COMPONENTS main)

project(esp32s3-interactive)
