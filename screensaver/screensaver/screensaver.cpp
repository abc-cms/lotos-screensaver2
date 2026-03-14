/*!
 * \file screensaver.cpp
 * \brief Main entry point for the lotos screensaver application
 * 
 * This file contains the main function that initializes the logging system
 * and starts the screensaver renderer.
 */

#include "screensaver/screensaver/renderer.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

/*!
 * \brief Main entry point for the lotos screensaver application
 * 
 * This function initializes the logging system with a rotating file logger
 * and starts the screensaver renderer, which manages video playback and UI elements.
 * 
 * \param argc Number of command line arguments
 * \param argv Array of command line argument strings
 * \return int Exit code of the application (0 for success)
 * 
 * \details The function:
 * 1. Creates a rotating logger with maximum log file size of 1MB and keeps 2 files
 * 2. Initializes the renderer
 * 3. Starts the main rendering loop
 * 4. Returns exit code when the loop ends
 */
int main(int argc, char *argv[]) {
    // Initialize logger.
    auto max_size = 1024 * 1024; /*!< Maximum size of log file in bytes (1MB) */
    auto max_files = 2;          /*!< Number of log files to keep in rotation */
    auto logging_path = get_logging_path(); /*!< Path to the log file */
    auto logger = spdlog::rotating_logger_mt(log_name, logging_path, max_size, max_files); /*!< Rotating logger instance */
    logger->flush_on(spdlog::level::err); /*!< Flush logs on error level */

    renderer_t renderer; /*!< Renderer instance to manage video and UI */
    renderer.run(); /*!< Start the main rendering loop */

    return 0;
}
