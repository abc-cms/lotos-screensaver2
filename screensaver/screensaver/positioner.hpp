/*!
 * \file positioner.hpp
 * \brief Header file containing the button positioner class for the lotos screensaver
 *
 * This file defines the positioner_t class that manages the positioning and animation
 * of the button in the screensaver, handling smooth transitions between different positions.
 */

#pragma once

#include <cmath>
#include <random>
#include <tuple>

#include "configuration.hpp"

/*!
 * \class positioner_t
 * \brief Class for managing button positioning and animation in the screensaver
 *
 * This class handles the positioning of the button within the screensaver window,
 * including smooth animations between different positions. It calculates button
 * coordinates based on configuration settings and time-based interpolation.
 */
class positioner_t {
public:
    /*!
     * \brief Set the configuration for the positioner
     * \param configuration Button configuration containing positioning settings
     * \param window_width Width of the window in pixels
     * \param window_height Height of the window in pixels
     * \param button_height Height of the button in pixels
     *
     * This function initializes the positioner with configuration settings and
     * window dimensions. It sets up all necessary parameters for position calculations.
     */
    void set_configuration(const button_configuration_t &configuration, const int window_width, const int window_height,
                           const int button_height) {
        m_animation_duration = configuration.animation_duration();
        m_switch_duration = configuration.switch_duration();
        m_window_width = window_width;
        m_window_height = window_height;
        m_button_height = button_height;
        m_bottom_margin = configuration.bottom_margin();
        m_bottom_margin_random = configuration.bottom_margin_random();
        m_side_margin = configuration.side_margin();
        m_side_margin_random = configuration.side_margin_random();
        m_full_duration = m_animation_duration + m_switch_duration;

        update_offsets();
        update(0);
    }

    /*!
     * \brief Update the positioner state based on current time
     * \param time Current time in seconds
     *
     * This function updates the internal state of the positioner to reflect
     * the current position based on the elapsed time. It handles animation
     * interpolation between different button positions.
     */
    void update(const float time) {
        const int current_period = static_cast<int>(std::floor(time / m_full_duration));
        const float rest_period_time = std::fmod(time, m_full_duration);

        if (current_period != previous_period) {
            update_offsets();
            previous_period = current_period;
        }

        m_interpolation_factor
            = std::max(0.f, (rest_period_time - m_switch_duration)) / static_cast<float>(m_animation_duration);
    }

    /*!
     * \brief Get the button rectangle coordinates
     * \return Tuple of (left, right, top, bottom) coordinates
     *
     * This function returns the current coordinates of the button rectangle.
     * The coordinates represent the button's position within the window.
     */
    std::tuple<int, int, int, int> button_rectangle() const {
        const int side_offset = static_cast<int>(
            round(m_previous_side_offset * (1 - m_interpolation_factor) + m_side_offset * m_interpolation_factor));
        const int bottom_offset = static_cast<int>(
            round(m_previous_bottom_offset * (1 - m_interpolation_factor) + m_bottom_offset * m_interpolation_factor));
        return {side_offset, m_window_width - side_offset, m_window_height - bottom_offset - m_button_height,
                m_window_height - bottom_offset};
    }

private:
    /*!
     * \brief Update the offset values for animation
     *
     * This private function generates new random offsets for the button position,
     * preparing for the next animation cycle. It stores both current and previous
     * offset values to enable smooth interpolation.
     */
    void update_offsets() {
        m_previous_side_offset = m_side_offset;
        m_previous_bottom_offset = m_bottom_offset;
        m_side_offset = m_side_margin
                        + static_cast<int>(round(static_cast<float>(m_side_margin_random) * distribution(generator)));
        m_bottom_offset
            = m_bottom_margin
              + static_cast<int>(round(static_cast<float>(m_bottom_margin_random) * distribution(generator)));
    }

private:
    int m_animation_duration; //!< Duration of the animation in milliseconds
    int m_switch_duration;    //!< Duration of the switching phase between positions

    int m_window_width;  //!< Width of the window in pixels
    int m_window_height; //!< Height of the window in pixels
    int m_button_height; //!< Height of the button in pixels

    int m_bottom_margin;        //!< Base bottom margin from window edge
    int m_bottom_margin_random; //!< Random variation in bottom margin

    int m_side_margin;        //!< Base side margin from window edges
    int m_side_margin_random; //!< Random variation in side margin

    float m_full_duration; //!< Total duration of one animation cycle (animation + switch)

    int previous_period = -1;           //!< Previous animation period for tracking changes
    int m_side_offset = 0;              //!< Current horizontal offset from window sides
    int m_bottom_offset = 0;            //!< Current vertical offset from bottom of window
    int m_previous_side_offset = 0;     //!< Previous horizontal offset for interpolation
    int m_previous_bottom_offset = 0;   //!< Previous vertical offset for interpolation
    float m_interpolation_factor = 0.f; //!< Interpolation factor for smooth animation

    std::mt19937 generator{23012012};                               //!< Random number generator for position variation
    std::uniform_real_distribution<float> distribution{0.0f, 1.0f}; //!< Distribution for random values
};
