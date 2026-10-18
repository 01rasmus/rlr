#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../types/res.h"
#include "../math/rect.h"
#include "texture.h"

/**
 * @brief Loads an MTSDF font from a csv and texture from file
 * 
 *        The fallbacks will be used if a label tries to render a glyph that
 *        the original font doesn't have.
 * 
 *        The fonts are generated with [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen)
 * 
 * @param file_csv The file location of the generated csv file
 * @param file_tex The file location of teh generated texture file
 * @param px_range The pixel range used to generate the font
 * 
 * @return Returns a pointer to the loaded font. Returns `NULL` on failure
 * 
 * @warning The font needs to be freed with `rlr_font_free`
 * @warning The font does not take ownership of the fallback fonts. If a fallback font is freed whilst its being used from a root font, use after free may happen
 */
rlr_font_t* rlr_font_load(const char* file_csv, const char* file_tex, float px_range);

/**
 * @brief Loads an MTSDF font from a csv and texture from file with fallback fonts
 * 
 *        The fallbacks will be used if a label tries to render a glyph that
 *        the original font doesn't have.
 * 
 *        The fonts are generated with [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen)
 * 
 * @param file_csv The file location of the csv file
 * @param file_tex The file location of teh texture file
 * @param px_range The pixel range used to generate the font
 * @param fallbacks An array of fonts that will be used as fallbacks
 * @param fallback_count The length of the fallback font array
 * 
 * @return Returns a pointer to the loaded font. Returns `NULL` on failure
 *  
 * @warning The font needs to be freed with `rlr_font_free`
 * @warning The font does not take ownership of the fallback fonts. If a fallback font is freed whilst its being used from a root font, use after free may happen
 */
rlr_font_t* rlr_font_load_flbks(const char* file_csv, const char* file_tex, float px_range, rlr_font_t** fallbacks, size_t fallback_len);

/**
 * @brief Sets an offset for the generated text on a label vertically.
 *        It is a ratio of the height of a character. A value of `0` means no change.
 *        Typical values are between 0.0 and 1.0
 * 
 * @param font The font to set the adjustment to
 * @param fraction The amount to adjust with
 */
void rlr_font_set_vertical_offset(rlr_font_t* font, float fraction);

/**
 * @param font The font to set the adjustment to
 * @param scale The scale to change the font's glyph size
 */
void rlr_font_set_size_scale(rlr_font_t* font, float scale);

/**
 * @param font The font to measure with
 * @param rectangle The bounds of the measurement. The calculated size can never be larger than the rectangles size
 * @param text_size The size of the text to measure with
 * @param format A normal `printf` style format string. Every paramater after this will be used for the string formatter. It uses `snprintf` internally
 * 
 * @return A 2-dimensional vector of the area used
 */
rlr_vec2_t rlr_font_measure(rlr_font_t* font, rlr_rect_t rectangle, float text_size, const char* format, ...);

/**
 * @warning If the font does not have the unicode character,
 *          it will go through all of the fallback fonts and
 *          return one that has it
 *
 * @param font The font to get the glyph from
 * @param unicode The character to get
 * @param `out_font_index` Will be set to the index of the font
 *        that had the glyph
 *
 *        Non-zero means that it was a fallback font that had it,
 *        a zero value means that the main font had it (it will set it to 0
 *        if the function returns NULL too, since then the unknown glyph
 *        is usually in the main font)
 * 
 * @return The glyph. If the the font or any of it's fallback fonts does not have the character, `NULL` will be returned
*/
const rlr_font_glyph_t* rlr_font_get_glyph(rlr_font_t* font, uint32_t unicode, uint32_t* out_font_index);

/**
 * @param font The font to get the count from
 * 
 * @return The amount of glyphs in the root font. Does not include the fallback fonts' glyphs
 */
int32_t rlr_font_get_glyph_count(const rlr_font_t* font);

/**
 * @param ubo The pointer to the font to free. If `NULL` is provided, nothing will be done
 * 
 * @warning Be careful of freeing fonts used as a fallback from other fonts.
 *          They might still be used after freed.
 */
void rlr_font_free(rlr_font_t* font);