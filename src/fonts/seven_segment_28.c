/*******************************************************************************
 * Size: 28 px
 * Bpp: 1
 * Opts: --bpp 1 --size 28 --no-compress --stride 1 --align 1 --font 7segment.ttf --symbols 0123456789:- --format lvgl -o seven_segment_28.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



#ifndef SEVEN_SEGMENT_28
#define SEVEN_SEGMENT_28 1
#endif

#if SEVEN_SEGMENT_28

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+002D "-" */
    0x7f, 0x7e,

    /* U+0030 "0" */
    0x3f, 0xed, 0xfb, 0xe0, 0x7e, 0x7, 0xe0, 0x7e,
    0x7, 0xe0, 0x7e, 0x7, 0xe0, 0x7c, 0x3, 0x0,
    0xc, 0x6, 0xe0, 0xee, 0xe, 0xe0, 0xee, 0xe,
    0xe0, 0xee, 0xe, 0xe0, 0xeb, 0xfe, 0xff, 0x40,

    /* U+0031 "1" */
    0x2f, 0xff, 0xff, 0xec, 0x3f, 0xff, 0xff, 0xf8,

    /* U+0032 "2" */
    0x3f, 0xe1, 0xfb, 0x0, 0x70, 0x7, 0x0, 0x70,
    0x7, 0x0, 0x70, 0x7, 0x0, 0x71, 0xfa, 0x7f,
    0xce, 0x0, 0xe0, 0xe, 0x0, 0xe0, 0xe, 0x0,
    0xe0, 0xe, 0x0, 0xe0, 0xf, 0xf0, 0x7f, 0x0,

    /* U+0033 "3" */
    0x3f, 0x83, 0xec, 0x3, 0x80, 0x70, 0xe, 0x1,
    0xc0, 0x38, 0x7, 0x0, 0xe7, 0xf4, 0xfe, 0x0,
    0x70, 0xe, 0x1, 0xc0, 0x38, 0x7, 0x0, 0xe0,
    0x1c, 0x3, 0x9f, 0xf7, 0xf8,

    /* U+0034 "4" */
    0x0, 0x58, 0xf, 0x83, 0xf0, 0x7e, 0xf, 0xc1,
    0xf8, 0x3f, 0x7, 0xe0, 0xf7, 0xe8, 0xff, 0x0,
    0xe0, 0x1c, 0x3, 0x80, 0x70, 0xe, 0x1, 0xc0,
    0x38, 0x7, 0x0, 0x40,

    /* U+0035 "5" */
    0x7f, 0x37, 0xce, 0x3, 0x80, 0xe0, 0x38, 0xe,
    0x3, 0x80, 0xe0, 0x2f, 0xc3, 0xf4, 0x7, 0x1,
    0xc0, 0x70, 0x1c, 0x7, 0x1, 0xc0, 0x70, 0x1c,
    0xff, 0xfe, 0x80,

    /* U+0036 "6" */
    0x3f, 0x9b, 0xf3, 0x80, 0x70, 0xe, 0x1, 0xc0,
    0x38, 0x7, 0x0, 0xe0, 0x17, 0xf2, 0xfe, 0xf0,
    0x7e, 0xf, 0xc1, 0xf8, 0x3f, 0x7, 0xe0, 0xfc,
    0x1f, 0x83, 0xdf, 0xff, 0xf4,

    /* U+0037 "7" */
    0xfe, 0x3e, 0xc0, 0xe0, 0x70, 0x38, 0x1c, 0xe,
    0x7, 0x3, 0x80, 0x0, 0x0, 0x70, 0x38, 0x1c,
    0xe, 0x7, 0x3, 0x81, 0xc0, 0xe0, 0x60,

    /* U+0038 "8" */
    0x3f, 0xed, 0xfb, 0xe0, 0x7e, 0x7, 0xe0, 0x7e,
    0x7, 0xe0, 0x7e, 0x7, 0xe0, 0x7b, 0xfa, 0xbf,
    0xee, 0xe, 0xe0, 0xee, 0xe, 0xe0, 0xee, 0xe,
    0xe0, 0xee, 0xe, 0xe0, 0xeb, 0xfe, 0xff, 0x40,

    /* U+0039 "9" */
    0x7f, 0xdb, 0xef, 0x83, 0xf0, 0x7e, 0xf, 0xc1,
    0xf8, 0x3f, 0x7, 0xe0, 0xf7, 0xe8, 0xff, 0x0,
    0xe0, 0x1c, 0x3, 0x80, 0x70, 0xe, 0x1, 0xc0,
    0x38, 0x7, 0x1f, 0xef, 0xe8,

    /* U+003A ":" */
    0xfc, 0x0, 0x0, 0x3, 0xf0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 210, .box_w = 8, .box_h = 2, .ofs_x = 3, .ofs_y = 9},
    {.bitmap_index = 2, .adv_w = 210, .box_w = 12, .box_h = 21, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 34, .adv_w = 210, .box_w = 3, .box_h = 21, .ofs_x = 10, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 210, .box_w = 12, .box_h = 21, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 74, .adv_w = 210, .box_w = 11, .box_h = 21, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 103, .adv_w = 210, .box_w = 11, .box_h = 20, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 131, .adv_w = 210, .box_w = 10, .box_h = 21, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 210, .box_w = 11, .box_h = 21, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 210, .box_w = 9, .box_h = 20, .ofs_x = 4, .ofs_y = 1},
    {.bitmap_index = 210, .adv_w = 210, .box_w = 12, .box_h = 21, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 210, .box_w = 11, .box_h = 21, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 79, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = 5}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint8_t glyph_id_ofs_list_0[] = {
    0, 0, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 45, .range_length = 14, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = glyph_id_ofs_list_0, .list_length = 14, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif

};

extern const lv_font_t lv_font_montserrat_28;


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t seven_segment_28 = {
#else
lv_font_t seven_segment_28 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 21,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &lv_font_montserrat_28,
#endif
    .user_data = NULL,
};



#endif /*#if SEVEN_SEGMENT_28*/
