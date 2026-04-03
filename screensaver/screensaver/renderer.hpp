/*!
 * \file renderer.hpp
 * \brief Header file containing the main renderer class for the lotos screensaver
 *
 * This file defines the renderer_t class that manages the complete screensaver
 * functionality including video playback, button rendering, and event handling.
 */

#pragma once

#include <cstdint>

#include "button_renderer.hpp"
#include "configuration.hpp"
#include "positioner.hpp"
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <mpv/render.h>
/*!
 * \class renderer_t
 * \brief Main class that manages the lotos screensaver functionality
 *
 * This class is responsible for initializing the video playback system,
 * managing the rendering loop, handling user input, and coordinating
 * between various components like media playback, button positioning,
 * and configuration management.
 */
class renderer_t {
public:
    /*!
     * \brief Constructor for the renderer
     *
     * Initializes the renderer by setting up the MPV video player library,
     * SDL video subsystem, creating window and renderer instances, and
     * preparing the button renderer for drawing operations.
     */
    renderer_t() {
        SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
        SDL_Init(SDL_INIT_VIDEO);

        SDL_Rect bounds;
        SDL_GetDisplayBounds(0, &bounds);
        SDL_CreateWindowAndRenderer(bounds.w, bounds.h, SDL_WINDOW_FULLSCREEN_DESKTOP, &m_window, &m_renderer);

        m_button_renderer.set_renderer(m_renderer);
    }

    /*!
     * \brief Destructor for the renderer
     *
     * Cleans up resources by freeing the MPV render context and destroying
     * the MPV handle.
     */
    ~renderer_t() {
        destroy_player();
    }

    /*!
     * \brief Start the main rendering loop
     * \return int Exit code of the application (0 for success)
     *
     * This function initializes the configuration, sets up MPV render context,
     * registers event callbacks, and starts the main event loop that handles
     * video playback, button rendering, and user interactions.
     */
    int run() {
        m_configuration = configuration_t::load(get_configuration_path());

        m_wakeup_on_mpv_render_update = SDL_RegisterEvents(1);
        m_wakeup_on_mpv_events = SDL_RegisterEvents(1);
        m_timer_event = SDL_RegisterEvents(1);
        m_update_settings_timer_event = SDL_RegisterEvents(1);

        m_timer_id = SDL_AddTimer(33, &renderer_t::timer_callback, &m_timer_event);
        m_update_settings_timer_id = SDL_AddTimer(10000, &renderer_t::timer_callback, &m_update_settings_timer_event);
        int result = loop();

        cleanup();

        return result;
    }

private:
    /*!
     * \brief Check if current time is within active periods
     * \return bool True if current time is within configured activity periods
     *
     * This function determines whether the screensaver should be active based
     * on the configured time periods in the settings.
     */
    bool is_active_period() const {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm local = *std::localtime(&now);

        uint32_t hours = static_cast<uint32_t>(local.tm_hour);
        uint32_t minutes = static_cast<uint32_t>(local.tm_min);

        for (const auto &interval : m_configuration.activity_frames()) {
            const auto start_hours = interval.start.hours;
            const auto start_minutes = interval.start.minutes;
            const auto end_hours = interval.end.hours;
            const auto end_minutes = interval.end.minutes;

            if ((hours > start_hours || (hours == start_hours && minutes >= start_minutes))
                && (hours < end_hours || (hours == end_hours && minutes < end_minutes))) {
                return true;
            }
        }
        return false;
    }

    /*!
     * \brief Clean up resources
     *
     * This function cleans up the texture resource when the application shuts down.
     */
    void cleanup() {
        if (m_texture) {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
    }

    /*!
     * \brief Create and initialize the MPV player
     *
     * This function initializes the MPV video player with the required
     * settings for screensaver functionality. It creates the MPV handle,
     * sets up the video output to libmpv, disables audio output, and
     * configures playlist looping.
     */
    void create_player() {
        if (m_mpv) {
            destroy_player();
        }

        m_mpv = mpv_create();

        mpv_set_option_string(m_mpv, "vo", "libmpv");
        mpv_set_option_string(m_mpv, "ao", "null");
        mpv_set_option_string(m_mpv, "loop-playlist", "inf");

        mpv_initialize(m_mpv);
        mpv_request_log_messages(m_mpv, "debug");

        char *api_type = MPV_RENDER_API_TYPE_SW;
        int advanced_control = 1;
        mpv_render_param params[] = {{MPV_RENDER_PARAM_API_TYPE, api_type},
                                     {MPV_RENDER_PARAM_ADVANCED_CONTROL, &advanced_control},
                                     {MPV_RENDER_PARAM_INVALID, nullptr}};

        mpv_render_context_create(&m_render_context, m_mpv, params);

        mpv_set_wakeup_callback(m_mpv, &renderer_t::on_mpv_events, nullptr);
        mpv_render_context_set_update_callback(m_render_context, &renderer_t::on_mpv_render_update, nullptr);
    }

    /*!
     * \brief Destroy and cleanup the MPV player
     *
     * This function cleans up the MPV player resources by freeing the
     * render context and destroying the MPV handle. It should be called
     * when the screensaver is being shut down or when switching between
     * active and inactive states.
     */
    void destroy_player() {
        if (m_render_context) {
            mpv_render_context_set_update_callback(m_render_context, nullptr, nullptr);
            mpv_render_context_free(m_render_context);
            m_render_context = nullptr;
        }
        if (m_mpv) {
            mpv_set_wakeup_callback(m_mpv, nullptr, nullptr);
            mpv_destroy(m_mpv);
            m_mpv = nullptr;
        }
    }

    /*!
     * \brief Get the current media index in the playlist
     * \return int64_t The index of the currently playing media, or -1 if no media is playing
     *
     * This function retrieves the current position in the MPV playlist.
     * It returns the index of the current media file or -1 if no media is playing.
     */
    int64_t media_index() const {
        int64_t position = -1;
        mpv_get_property(m_mpv, "playlist-pos", MPV_FORMAT_INT64, &position);
        return position;
    }

    /*!
     * \brief Check if the player is in idle state
     * \return bool True if no media is currently playing, false otherwise
     *
     * This function determines if the media player is currently idle,
     * which occurs when no media is playing (media_index() returns -1).
     */
    bool is_idle() const {
        return media_index() < 0;
    }

    /*!
     * \brief Main event loop
     * \return int Exit code of the application
     *
     * This is the main loop that handles all events, updates screen content,
     * manages video playback, and handles user interactions.
     */
    int loop() {
        SDL_Event event;
        bool loop_active = true;

        int window_height;
        int window_width;
        SDL_GetWindowSize(m_window, &window_width, &window_height);
        m_positioner.set_configuration(m_configuration.button(), window_width, window_height, 75);

        while (loop_active) {
            if (SDL_WaitEvent(&event) != 1) {
                char *const args[] = {(char *)"lotos-screensaver", nullptr};
                execv("/proc/self/exe", args);
            }

            bool is_active = is_active_period();

            if (m_was_active && !is_active) {
                destroy_player();
            } else if (is_active && !m_was_active) {
                create_player();
                load_list();
            }

            m_was_active = is_active;

            bool redraw_frame = false;
            bool redraw_button = false;

            switch (event.type) {
            case SDL_QUIT: {
                loop_active = false;
                break;
            }

            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
                    if (is_active && is_idle()) {
                        load_list();
                        redraw_frame = true;
                        redraw_button = true;
                    }
                }
                break;
            }

            case SDL_KEYDOWN:
                break;

            default: {
                if (event.type == m_wakeup_on_mpv_render_update) {
                    if (m_render_context) {
                        const uint64_t flags = mpv_render_context_update(m_render_context);
                        if (flags & MPV_RENDER_UPDATE_FRAME) {
                            redraw_frame = true;
                            redraw_button = true;
                        }
                    }
                }

                if (event.type == m_wakeup_on_mpv_events) {
                    while (true) {
                        if (!m_mpv) {
                            break;
                        }

                        const mpv_event *event = mpv_wait_event(m_mpv, 0);

                        if (event->event_id == MPV_EVENT_START_FILE) {
                            double duration = 10;

                            int64_t index = media_index();
                            if (index >= 0 && index < m_configuration.media().size()) {
                                const auto &media = m_configuration.media()[index];
                                if (media.media_type() == media_type_e::image) {
                                    duration = media.duration();
                                }
                            }
                            mpv_set_property(m_mpv, "image-display-duration", MPV_FORMAT_DOUBLE, &duration);

                        } else if (event->event_id == MPV_EVENT_NONE) {
                            break;
                        }
                    }
                }

                if (event.type == m_timer_event) {
                    redraw_button = true;
                }

                if (event.type == m_update_settings_timer_event) {
                    auto configuration = configuration_t::load(get_configuration_path());
                    if (m_configuration != configuration) {
                        m_configuration = configuration;
                        load_list();
                    }
                }

                break;
            }
            }

            if (!is_active) {
                SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
                SDL_RenderClear(m_renderer);
            } else if (redraw_frame) {
                redraw_window_frame(true);
                redraw_button_frame();
            } else if (redraw_button) {
                redraw_window_frame(false);
                redraw_button_frame();
            }

            if (redraw_frame || redraw_button || !is_active) {
                SDL_RenderPresent(m_renderer);
            }
        }

        return 0;
    }

    /*!
     * \brief Redraw the video window frame
     * \param update_video Whether to update the video content
     *
     * This function renders the video content to the screen, managing texture
     * creation and updating based on current window dimensions and video frames.
     */
    void redraw_window_frame(const bool update_video) {
        if (update_video || !m_texture) {
            int window_width, window_height;
            SDL_GetWindowSize(m_window, &window_width, &window_height);

            if (!m_texture || window_width != m_screen_width || window_height != m_screen_height) {
                SDL_DestroyTexture(m_texture);
                m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING,
                                              window_width, window_height);

                m_screen_width = window_width;
                m_screen_height = window_height;
            }

            void *pixels;
            int pitch;
            SDL_LockTexture(m_texture, nullptr, &pixels, &pitch);

            size_t cpitch = pitch;
            int window_rect[2] = {window_width, window_height};
            mpv_render_param params[] = {{MPV_RENDER_PARAM_SW_SIZE, window_rect},
                                         {MPV_RENDER_PARAM_SW_FORMAT, const_cast<char *>("0bgr")},
                                         {MPV_RENDER_PARAM_SW_STRIDE, &cpitch},
                                         {MPV_RENDER_PARAM_SW_POINTER, pixels},
                                         {MPV_RENDER_PARAM_INVALID, nullptr}};
            mpv_render_context_render(m_render_context, params);

            SDL_UnlockTexture(m_texture);
        }

        SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
    }

    /*!
     * \brief Redraw the button frame
     *
     * This function renders the button with current position and styling,
     * using the positioner to determine location and the button renderer
     * for actual drawing.
     */
    void redraw_button_frame() {
        const uint32_t ticks = SDL_GetTicks();
        const button_configuration_t &button_configuration = m_configuration.button();

        m_positioner.update(static_cast<float>(ticks) / 1000.f);
        auto [left, right, top, bottom] = m_positioner.button_rectangle();

        auto bg_color_raw = button_configuration.background_color();
        SDL_Color bg_color{static_cast<uint8_t>(bg_color_raw & 0xff), static_cast<uint8_t>((bg_color_raw >> 8) & 0xff),
                           static_cast<uint8_t>((bg_color_raw >> 16) & 0xff),
                           static_cast<uint8_t>((bg_color_raw >> 24) & 0xff)};
        auto fg_color_raw = button_configuration.text_color();
        SDL_Color fg_color{static_cast<uint8_t>(fg_color_raw & 0xff), static_cast<uint8_t>((fg_color_raw >> 8) & 0xff),
                           static_cast<uint8_t>((fg_color_raw >> 16) & 0xff),
                           static_cast<uint8_t>((fg_color_raw >> 24) & 0xff)};
        m_button_renderer.render(button_configuration.text(), button_configuration.text_size(), right - left,
                                 bottom - top, bg_color, fg_color, button_configuration.corner_radius(), m_screen_width,
                                 bottom);
    }

    void load_list() {
        if (!m_mpv) {
            return;
        }

        const char *playlist_clear_cmd[] = {"playlist-clear", nullptr};
        mpv_command(m_mpv, playlist_clear_cmd);

        for (size_t index = 0; index < m_configuration.media().size(); ++index) {
            const auto media = m_configuration.media()[index];
            const std::string path = absolute_path(media.path());
            const char *load_file_cmd[]
                = {"loadfile", path.c_str(), (index == 0) ? "append-play" : "append", nullptr};
            mpv_command(m_mpv, load_file_cmd);
        }
    }

    /*!
     * \brief Timer callback function
     * \param interval Timer interval in milliseconds
     * \param param Pointer to timer event parameter
     * \return uint32_t The interval to maintain
     *
     * This static function handles timer events and posts corresponding
     * SDL events to the event queue.
     */
    static uint32_t timer_callback(const uint32_t interval, void *param) {
        SDL_Event event;
        SDL_zero(event);
        event.type = *static_cast<uint32_t *>(param);
        SDL_PushEvent(&event);
        return interval;
    }

    /*!
     * \brief MPV events callback function
     * \param ctx Context pointer (unused)
     *
     * This static function handles MPV events by posting an SDL event
     * to the main event queue.
     */
    static void on_mpv_events(void *ctx) {
        SDL_Event event = {.type = m_wakeup_on_mpv_events};
        SDL_PushEvent(&event);
    }

    /*!
     * \brief MPV render update callback function
     * \param ctx Context pointer (unused)
     *
     * This static function handles MPV render update events by posting
     * an SDL event to the main event queue.
     */
    static void on_mpv_render_update(void *ctx) {
        SDL_Event event = {.type = m_wakeup_on_mpv_render_update};
        SDL_PushEvent(&event);
    }

private:
    configuration_t m_configuration;     //!< Configuration settings for the screensaver
    positioner_t m_positioner;           //!< Positioner for managing button location and animation
    button_renderer_t m_button_renderer; //!< Renderer for drawing buttons with text

    inline static uint32_t m_wakeup_on_mpv_render_update; //!< SDL event ID for MPV render updates
    inline static uint32_t m_wakeup_on_mpv_events;        //!< SDL event ID for MPV events
    inline static uint32_t m_timer_event;                 //!< SDL event ID for timer events
    inline static uint32_t m_update_settings_timer_event; //!< SDL event ID for settings update timer

    mpv_handle *m_mpv = nullptr;                    //!< MPV handle for video playback
    SDL_Window *m_window = nullptr;                 //!< SDL window instance
    SDL_Renderer *m_renderer = nullptr;             //!< SDL renderer instance
    mpv_render_context *m_render_context = nullptr; //!< MPV render context

    int m_screen_width = -1;          //!< Current screen width
    int m_screen_height = -1;         //!< Current screen height
    SDL_Texture *m_texture = nullptr; //!< Texture for video rendering

    bool m_was_active = false; //!< Flag tracking previous active state

    SDL_TimerID m_timer_id;                 //!< Timer ID for regular updates
    SDL_TimerID m_update_settings_timer_id; //!< Timer ID for configuration updates
};
