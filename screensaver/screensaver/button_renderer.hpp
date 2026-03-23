/*!
 * \file button_renderer.hpp
 * \brief Header file containing the button renderer class for the lotos screensaver
 *
 * This file defines the button_renderer_t class that handles rendering of buttons
 * with text, colors, and rounded corners in the screensaver application.
 */

#pragma once

#include <string>

#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_render.h>

/*!
 * \class button_renderer_t
 * \brief Class for rendering buttons with text, colors, and rounded corners
 *
 * This class manages the rendering of buttons in the screensaver, including
 * text rendering, color management, and creating visually appealing rounded
 * corner buttons with smooth animations.
 */
class button_renderer_t {
public:
    /*!
     * \brief Constructor for button renderer
     *
     * Initializes the button renderer by setting up the SDL TTF library and
     * opening the default font. This constructor ensures that the TTF system
     * is properly initialized once during application startup.
     */
    button_renderer_t() {
        if (!m_initialized) {
            TTF_Init();
            m_initialized = true;
        }

        m_font = TTF_OpenFont(m_font_path, m_font_height);
    }

    /*!
     * \brief Set the SDL renderer for button rendering
     * \param renderer Pointer to the SDL_Renderer to use for drawing operations
     *
     * This function sets the renderer that will be used for all subsequent
     * button rendering operations. The renderer must be initialized before calling this.
     */
    void set_renderer(SDL_Renderer *renderer) {
        m_renderer = renderer;
    }

    /*!
     * \brief Render a button with text and styling
     * \param text Text to display on the button
     * \param font_height Height of the font in pixels
     * \param width Width of the button in pixels
     * \param height Height of the button in pixels
     * \param bg_color Background color of the button (RGBA format)
     * \param fg_color Foreground color of the text (RGBA format)
     * \param radius Corner radius for rounded corners
     * \param window_width Width of the window in pixels
     * \param bottom Bottom coordinate where button should be positioned
     *
     * This function renders a button with the specified properties, including
     * text content, colors, and rounded corners. It handles caching to avoid
     * unnecessary re-creation of textures when parameters haven't changed.
     */
    void render(const std::string &text, const uint32_t font_height, const int width, const int height,
                const SDL_Color &bg_color, const SDL_Color &fg_color, const int radius, const int window_width,
                const int bottom) {
        const bool text_equal = text == m_text;
        const bool font_height_equal = font_height == m_font_height;
        const bool width_equal = width == m_width;
        const int adjusted_height = std::max(height, m_text_height + 2 * (m_radius + 1));
        const bool height_equal = adjusted_height == m_height;
        const bool bg_color_equal = compare_color(bg_color, m_bg_color);
        const bool fg_color_equal = compare_color(fg_color, m_fg_color);
        const bool radius_equal = radius == m_radius;
        if (!text_equal || !font_height_equal || !width_equal || !height_equal || !bg_color_equal || !fg_color_equal
            || !radius_equal) {
            m_text = text;
            m_font_height = font_height;
            m_width = width;
            m_height = height;
            m_bg_color = bg_color;
            m_fg_color = fg_color;
            m_radius = radius;

            if (!font_height_equal) {
                rebuild_font();
            }

            rebuild(!(text_equal && font_height_equal && width_equal && bg_color_equal && radius_equal),
                    !(bg_color_equal && radius_equal),
                    !(text_equal && font_height_equal && width_equal && fg_color_equal && radius_equal));
        }

        SDL_Rect destination_rect{(window_width - width) / 2, bottom - m_height, m_width, m_height};
        SDL_RenderCopy(m_renderer, m_button_texture, nullptr, &destination_rect);
    }

private:
    /*!
     * \brief Rebuild the font with a new height
     *
     * This private function closes the current font and opens a new one
     * with the specified font height. Used when font size changes.
     */
    void rebuild_font() {
        if (m_font) {
            TTF_CloseFont(m_font);
        }

        m_font = TTF_OpenFont(m_font_path, m_font_height);
    }

    /*!
     * \brief Rebuild button components based on changed parameters
     * \param rebuild_button Whether to rebuild the entire button texture
     * \param rebuild_corner Whether to rebuild the corner textures
     * \param rebuild_text Whether to rebuild the text texture
     *
     * This private function handles the rebuilding of different components
     * of the button based on which parameters have changed.
     */
    void rebuild(const bool rebuild_button, const bool rebuild_corner, const bool rebuild_text) {
        if (rebuild_corner) {
            draw_circle_texture(m_radius, m_bg_color);
        }
        if (rebuild_text) {
            create_text_texture(m_text, m_fg_color, m_width - 2 * m_radius);
        }
        if (rebuild_button) {
            m_height = std::max(m_height, m_text_height + 2 * (m_radius + 1));
            create_button_texture(m_text.c_str(), m_width, m_height, m_bg_color, m_fg_color, m_radius);
        }
    }

    /*!
     * \brief Compare two SDL_Color structures for equality
     * \param left First color to compare
     * \param right Second color to compare
     * \return true if colors are equal, false otherwise
     *
     * This static helper function compares two SDL_Color structures by
     * checking all their RGBA components.
     */
    static bool compare_color(const SDL_Color &left, const SDL_Color &right) {
        return left.a == right.a && left.b == right.b && left.g == right.g && left.r == right.r;
    }

    /*!
     * \brief Draw a circular texture for rounded corners
     * \param radius Radius of the circle
     * \param color Color to use for the circle
     *
     * This function creates a circular texture with the specified radius and color
     * that will be used as corner pieces for the button.
     */
    void draw_circle_texture(const int radius, const SDL_Color &color) {
        if (m_circle_texture != nullptr) {
            SDL_DestroyTexture(m_circle_texture);
        }

        const int size = 2 * radius + 1;
        m_circle_texture
            = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size, size);

        SDL_SetTextureBlendMode(m_circle_texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(m_renderer, m_circle_texture);
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

        for (int dy = -radius; dy <= radius; ++dy) {
            const float dx = sqrt(radius * radius - dy * dy);
            SDL_RenderDrawLineF(m_renderer, radius - dx, radius + dy, radius + dx, radius + dy);
        }

        SDL_SetRenderTarget(m_renderer, nullptr);
    }

    /*!
     * \brief Create a texture for the button text
     * \param text Text content to render
     * \param color Color of the text
     * \param max_width Maximum width for wrapped text
     *
     * This function renders the button text into a texture that can be
     * copied onto the button surface.
     */
    void create_text_texture(const std::string &text, const SDL_Color &color, const int max_width) {
        if (m_text_texture != nullptr) {
            SDL_DestroyTexture(m_text_texture);
        }

        TTF_SetFontWrappedAlign(m_font, TTF_WRAPPED_ALIGN_CENTER);
        SDL_Surface *text_surface = TTF_RenderUTF8_Blended_Wrapped(m_font, text.c_str(), color, max_width);
        m_text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
        m_text_width = text_surface->w;
        m_text_height = text_surface->h;
        SDL_FreeSurface(text_surface);
    }

    /*!
     * \brief Create the complete button texture
     * \param text Text to display on the button
     * \param width Width of the button
     * \param height Height of the button
     * \param bg_color Background color of the button
     * \param fg_color Foreground color of the text
     * \param radius Corner radius for rounded corners
     *
     * This function creates the complete button texture including text,
     * background, and corner pieces.
     */
    void create_button_texture(const char *text, const int width, const int height, const SDL_Color &bg_color,
                               const SDL_Color &fg_color, const int radius) {
        if (m_button_texture != nullptr) {
            SDL_DestroyTexture(m_button_texture);
        }

        m_button_texture
            = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

        SDL_SetTextureBlendMode(m_button_texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(m_renderer, m_button_texture);
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderDrawColor(m_renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);

        SDL_Rect middle = {radius, 0, width - 2 * radius, height};
        SDL_Rect left = {0, radius, radius, height - 2 * radius};
        SDL_Rect right = {width - radius, radius, radius, height - 2 * radius};

        SDL_RenderFillRect(m_renderer, &middle);
        SDL_RenderFillRect(m_renderer, &left);
        SDL_RenderFillRect(m_renderer, &right);

        const int circle_size = 2 * radius + 1;
        SDL_Rect lt_rect = {0, 0, circle_size, circle_size};
        SDL_RenderCopy(m_renderer, m_circle_texture, nullptr, &lt_rect);
        SDL_Rect lb_rect = {0, height - circle_size, circle_size, circle_size};
        SDL_RenderCopy(m_renderer, m_circle_texture, nullptr, &lb_rect);
        SDL_Rect rb_rect = {width - circle_size, height - circle_size, circle_size, circle_size};
        SDL_RenderCopy(m_renderer, m_circle_texture, nullptr, &rb_rect);
        SDL_Rect rt_rect = {width - circle_size, 0, circle_size, circle_size};
        SDL_RenderCopy(m_renderer, m_circle_texture, nullptr, &rt_rect);

        SDL_Rect text_rect = {(width - m_text_width) / 2, (height - m_text_height) / 2, m_text_width, m_text_height};
        SDL_RenderCopy(m_renderer, m_text_texture, nullptr, &text_rect);
        SDL_SetRenderTarget(m_renderer, nullptr);
    }

private:
    TTF_Font *m_font = nullptr;              //!< Font used for rendering text
    SDL_Renderer *m_renderer = nullptr;      //!< SDL Renderer for drawing operations
    SDL_Texture *m_circle_texture = nullptr; //!< Texture for rounded corners
    SDL_Texture *m_text_texture = nullptr;   //!< Texture containing rendered text
    SDL_Texture *m_button_texture = nullptr; //!< Complete button texture

    int m_text_width = 0;  //!< Width of the text texture
    int m_text_height = 0; //!< Height of the text texture

    inline static bool m_initialized = false; //!< Flag indicating if TTF system is initialized

    std::string m_text;               //!< Current text content
    int m_width = -1;                 //!< Width of the button
    int m_height = -1;                //!< Height of the button
    SDL_Color m_bg_color{0, 0, 0, 0}; //!< Background color of the button
    SDL_Color m_fg_color{0, 0, 0, 0}; //!< Foreground color (text) of the button
    int m_radius = -1;                //!< Radius for rounded corners

    uint32_t m_font_height = 14; //!< Current font height
    constexpr static const char *m_font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"; //!< Font file path
};
