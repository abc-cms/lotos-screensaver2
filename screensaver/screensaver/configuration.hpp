/*!
 * \file configuration.hpp
 * \brief Header file containing configuration classes and structures for the lotos screensaver
 *
 * This file defines various data structures and classes used to represent
 * the configuration of the lotos screensaver application, including media,
 * button, and activity settings.
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <list>
#include <regex>
#include <string>

#include <json/json.h>
#include <parameters.hpp>
#include <spdlog/spdlog.h>

/*!
 * \brief Enumeration for media types
 *
 * This enumeration defines the different types of media that can be used
 * in the screensaver.
 */
enum class media_type_e : uint8_t { image, video };

/*!
 * \class media_configuration_t
 * \brief Class representing configuration for a single media item
 *
 * This class stores configuration information for individual media files
 * including type, path, and duration settings.
 */
class media_configuration_t {
public:
    /*!
     * \brief Equality operator
     * \param other Other media configuration to compare with
     * \return true if configurations are equal, false otherwise
     */
    bool operator==(const media_configuration_t &) const = default;

    /*!
     * \brief Inequality operator
     * \param other Other media configuration to compare with
     * \return true if configurations are not equal, false otherwise
     */
    bool operator!=(const media_configuration_t &) const = default;

    /*!
     * \brief Constructor for media configuration
     * \param media_type Type of media (image or video)
     * \param path Path to the media file
     * \param duration Duration for image media (in seconds, 0 for videos)
     */
    media_configuration_t(media_type_e media_type, const std::filesystem::path &path, float duration = 0.0)
        : m_media_type(media_type), m_path(path), m_duration(duration) {
    }

    /*!
     * \brief Get the media type
     * \return The media type (image or video)
     */
    media_type_e media_type() const {
        return m_media_type;
    }

    /*!
     * \brief Get the media path
     * \return Path to the media file
     */
    const std::filesystem::path &path() const {
        return m_path;
    }

    /*!
     * \brief Get the media duration
     * \return Duration of the media (in seconds)
     */
    float duration() const {
        return m_duration;
    }

private:
    media_type_e m_media_type;    //!< Type of media (image or video)
    std::filesystem::path m_path; //!< Path to the media file
    float m_duration;             //!< Duration in seconds for image media
};

/*!
 * \class button_configuration_t
 * \brief Class representing configuration for the screensaver button
 *
 * This class stores configuration information for the button appearance and behavior
 * including text, colors, sizes, and animation properties.
 */
class button_configuration_t {
public:
    /*!
     * \brief Equality operator
     * \param other Other button configuration to compare with
     * \return true if configurations are equal, false otherwise
     */
    bool operator==(const button_configuration_t &) const = default;

    /*!
     * \brief Inequality operator
     * \param other Other button configuration to compare with
     * \return true if configurations are not equal, false otherwise
     */
    bool operator!=(const button_configuration_t &) const = default;

    /*!
     * \brief Get the button text
     * \return The text displayed on the button
     */
    const std::string &text() const {
        return m_text;
    }

    /*!
     * \brief Get the text color
     * \return The color of the button text (RGBA format)
     */
    uint32_t text_color() const {
        return m_text_color;
    }

    /*!
     * \brief Get the text size
     * \return The size of the button text
     */
    uint32_t text_size() const {
        return m_text_size;
    }

    /*!
     * \brief Get the background color
     * \return The background color of the button (RGBA format)
     */
    uint32_t background_color() const {
        return m_background_color;
    }

    /*!
     * \brief Get the height of the button
     * \return The height of the button
     */
    uint32_t height() const {
        return m_height;
    }

    /*!
     * \brief Get the corner radius
     * \return The corner radius for rounded corners
     */
    uint32_t corner_radius() const {
        return m_corner_radius;
    }

    /*!
     * \brief Get side margin
     * \return The horizontal margin from the sides
     */
    uint32_t side_margin() const {
        return m_side_margin;
    }

    /*!
     * \brief Get random side margin
     * \return Random variation in side margin
     */
    uint32_t side_margin_random() const {
        return m_side_margin_random;
    }

    /*!
     * \brief Get bottom margin
     * \return The vertical margin from the bottom
     */
    uint32_t bottom_margin() const {
        return m_bottom_margin;
    }

    /*!
     * \brief Get random bottom margin
     * \return Random variation in bottom margin
     */
    uint32_t bottom_margin_random() const {
        return m_bottom_margin_random;
    }

    /*!
     * \brief Get switch duration
     * \return Duration for switching between states
     */
    float switch_duration() const {
        return m_switch_duration;
    }

    /*!
     * \brief Get number of animation steps
     * \return Number of steps in the animation
     */
    uint32_t animation_steps() const {
        return m_animation_steps;
    }

    /*!
     * \brief Get animation duration
     * \return Duration of the animation
     */
    float animation_duration() const {
        return m_animation_duration;
    }

private:
    std::string m_text;              //!< Text displayed on the button
    uint32_t m_text_color;           //!< Color of text (RGBA format)
    uint32_t m_text_size;            //!< Size of the text
    uint32_t m_background_color;     //!< Background color of the button (RGBA format)
    uint32_t m_height;               //!< Height of the button
    uint32_t m_corner_radius;        //!< Corner radius for rounded corners
    uint32_t m_side_margin;          //!< Horizontal margin from window sides
    uint32_t m_side_margin_random;   //!< Random variation in side margin
    uint32_t m_bottom_margin;        //!< Vertical margin from bottom of window
    uint32_t m_bottom_margin_random; //!< Random variation in bottom margin
    float m_animation_duration;      //!< Duration of the animation
    uint32_t m_animation_steps;      //!< Number of steps in the animation
    float m_switch_duration;         //!< Duration for switching between states

    friend class configuration_t; //!< Friend class to allow direct access to private members
};

/*!
 * \struct int_time_t
 * \brief Structure representing a time interval
 *
 * This structure represents a specific point in time using hours and minutes.
 */
struct int_time_t {
    /*!
     * \brief Equality operator
     * \param other Other time to compare with
     * \return true if times are equal, false otherwise
     */
    bool operator==(const int_time_t &) const = default;

    /*!
     * \brief Inequality operator
     * \param other Other time to compare with
     * \return true if times are not equal, false otherwise
     */
    bool operator!=(const int_time_t &) const = default;

    uint32_t hours = 0;   //!< Hours component of the time
    uint32_t minutes = 0; //!< Minutes component of the time
};

/*!
 * \struct int_interval_t
 * \brief Structure representing a time interval
 *
 * This structure represents a time interval with start and end points.
 */
struct int_interval_t {
    /*!
     * \brief Equality operator
     * \param other Other interval to compare with
     * \return true if intervals are equal, false otherwise
     */
    bool operator==(const int_interval_t &) const = default;

    /*!
     * \brief Inequality operator
     * \param other Other interval to compare with
     * \return true if intervals are not equal, false otherwise
     */
    bool operator!=(const int_interval_t &) const = default;

    int_time_t start; //!< Start time of the interval
    int_time_t end;   //!< End time of the interval
};

/*!
 * \class configuration_t
 * \brief Class representing the complete screensaver configuration
 *
 * This class stores all configuration settings for the screensaver including
 * media files, button appearance, and activity periods.
 */
class configuration_t {
public:
    /*!
     * \brief Equality operator
     * \param other Other configuration to compare with
     * \return true if configurations are equal, false otherwise
     */
    bool operator==(const configuration_t &) const = default;

    /*!
     * \brief Inequality operator
     * \param other Other configuration to compare with
     * \return true if configurations are not equal, false otherwise
     */
    bool operator!=(const configuration_t &) const = default;

    /*!
     * \brief Get the inactivity timeout
     * \return Timeout value in seconds
     */
    int timeout() const {
        return m_timeout;
    }

    /*!
     * \brief Get button configuration
     * \return Reference to the button configuration object
     */
    const auto &button() const {
        return m_button;
    }

    /*!
     * \brief Get media configuration
     * \return Vector of media configurations
     */
    const auto media() const {
        return std::vector<media_configuration_t>(m_media.begin(), m_media.end());
    }

    /*!
     * \brief Get activity frames
     * \return Reference to the list of activity intervals
     */
    const auto &activity_frames() const {
        return m_activity_frames;
    }

    /*!
     * \brief Load configuration from file
     * \param path Path to the configuration file
     * \return Configuration object loaded from file
     *
     * This static function loads a configuration object from a JSON file.
     * It reads all settings including media files, button configuration,
     * and activity time periods.
     */
    static configuration_t load(const std::filesystem::path &path) {
        configuration_t configuration;

        auto log = spdlog::get(log_name);
        log->info("Loading configuration from {}", static_cast<std::string>(path));

        Json::Value root;
        std::ifstream file(path);

        if (file.bad()) {
            log->error("Unable to open configuration file: {}", static_cast<std::string>(path));
            throw std::runtime_error("Read configuration error");
        }

        JSONCPP_STRING errors;
        Json::CharReaderBuilder builder;

        if (!parseFromStream(builder, file, &root, &errors)) {
            log->error("Unable to load configuration from file {} due to errors: {}", static_cast<std::string>(path),
                       errors);
            throw std::runtime_error("Read configuration error");
        }

        // Read list of media.
        std::list<media_configuration_t> media;
        auto media_list = root["media_files"];
        for (auto media_node : media_list) {
            media_type_e media_type
                = (std::string(media_node["type"].asString()) == "image") ? media_type_e::image : media_type_e::video;
            std::filesystem::path path(media_node["path"].asString());
            float duration = (media_type == media_type_e::image) ? media_node["time"].asFloat() : 0.0;
            media.emplace_back(media_type, path, duration);
        }

        configuration.m_media = media;

        // Load button configuration.
        auto button = root["button"];
        button_configuration_t &button_configuration = configuration.m_button;
        button_configuration.m_text = button["text"].asString();
        button_configuration.m_text_color = std::stoul(button["text_color"].asString(), nullptr, 16);
        button_configuration.m_text_size = button["text_size"].asInt();
        button_configuration.m_background_color = std::stoul(button["background_color"].asString(), nullptr, 16);
        button_configuration.m_height = button["height"].asInt();
        button_configuration.m_corner_radius = button["corner_radius"].asInt();
        button_configuration.m_side_margin = button["side_margin"].asInt();
        button_configuration.m_side_margin_random = button["side_margin_random"].asInt();
        button_configuration.m_bottom_margin = button["bottom_margin"].asInt();
        button_configuration.m_bottom_margin_random = button["bottom_margin_random"].asInt();
        button_configuration.m_animation_duration = button["animation_duration"].asFloat();
        button_configuration.m_animation_steps = button["animation_steps"].asInt();
        button_configuration.m_switch_duration = button["switch_duration"].asFloat();

        // Screensaver configuration.
        int_interval_t interval;
        std::smatch matches;
        std::regex time("([0-9]|0[0-9]|1[0-9]|2[0-3]):([0-5][0-9])");
        auto screensaver_settings = root["screensaver_settings"];
        configuration.m_timeout = screensaver_settings["inactivity_timeout"].asInt();
        std::string start_time = screensaver_settings["start_time"].asString();
        std::regex_search(start_time, matches, time);
        interval.start.hours = std::atoi(matches.str(1).c_str());
        interval.start.minutes = std::atoi(matches.str(2).c_str());
        std::string end_time = screensaver_settings["end_time"].asString();
        std::regex_search(end_time, matches, time);
        interval.end.hours = std::atoi(matches.str(1).c_str());
        interval.end.minutes = std::atoi(matches.str(2).c_str());

        configuration.m_activity_frames.push_back(interval);

        log->info("Configuration loaded");

        return configuration;
    }

private:
    int m_timeout;                               //!< Inactivity timeout in seconds
    button_configuration_t m_button;             //!< Button configuration
    std::list<media_configuration_t> m_media;    //!< List of media files to display
    std::list<int_interval_t> m_activity_frames; //!< Activity time intervals when screensaver is active
};
