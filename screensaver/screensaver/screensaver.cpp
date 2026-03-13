// Build with: gcc -o main_sw main_sw.c `pkg-config --libs --cflags mpv sdl2` -std=c99

#include "screensaver/screensaver/renderer.hpp"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>

#include "positioner.hpp"
#include <charconv>
#include <string>
#include <iostream>
#include "configuration.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>




int main(int argc, char* argv[])
{
    // Initialize logger.
    auto max_size = 1024 * 1024; // Set max log size to 1MB.
    auto max_files = 2;
    auto logging_path = get_logging_path();
    auto logger = spdlog::rotating_logger_mt(log_name, logging_path, max_size, max_files);
    logger->flush_on(spdlog::level::err);

    renderer_t renderer;
    renderer.run();

    return 0;
}
