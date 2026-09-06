/*******************************************************************************
 * Size: 28 px
 * Bpp: 1
 * Opts: --bpp 1 --size 28 --no-compress --stride 1 --align 1 --font TiltNeon-Regular-VariableFont_XROT,YROT.ttf --symbols 0123456789:- --format lvgl -o tilt_neon_28.c
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



#ifndef TILT_NEON_28
#define TILT_NEON_28 1
#endif

#if TILT_NEON_28

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+002D "-" */
    0xff, 0xff,

    /* U+0030 "0" */
    0x7, 0xc0, 0x3f, 0xe0, 0xf1, 0xe3, 0xc1, 0xe7,
    0x1, 0xde, 0x3, 0xf8, 0x3, 0xf0, 0x7, 0xe0,
    0xf, 0xc0, 0x1f, 0x80, 0x3f, 0x0, 0x7e, 0x0,
    0xee, 0x3, 0x9c, 0x7, 0x1c, 0x1c, 0x1e, 0xf0,
    0x1f, 0xc0, 0xa, 0x0,

    /* U+0031 "1" */
    0x1f, 0x7f, 0x7f, 0x7, 0x7, 0x7, 0x7, 0x7,
    0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
    0x7, 0x7, 0x7,

    /* U+0032 "2" */
    0x3f, 0xf, 0xfc, 0xe0, 0xe0, 0x7, 0x0, 0x70,
    0x7, 0x0, 0x70, 0xf, 0x1, 0xe0, 0x3c, 0x7,
    0xc0, 0xf8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0xf, 0xff, 0x7f, 0xf0,

    /* U+0033 "3" */
    0xff, 0xef, 0xfe, 0x1, 0xe0, 0x3c, 0x3, 0x80,
    0x70, 0xe, 0x1, 0xc0, 0x3f, 0x3, 0xfc, 0x1,
    0xe0, 0xf, 0x0, 0x70, 0x7, 0x0, 0x70, 0xe,
    0xc1, 0xef, 0xfc, 0x3f, 0x0,

    /* U+0034 "4" */
    0x1, 0xe0, 0xf, 0x80, 0x3e, 0x1, 0xf8, 0xf,
    0xe0, 0x3b, 0x81, 0xce, 0xf, 0x38, 0x38, 0xe1,
    0xc3, 0x8f, 0xe, 0x38, 0x38, 0xff, 0xfb, 0xff,
    0xe0, 0xe, 0x0, 0x38, 0x0, 0xe0, 0x3, 0x80,
    0xe, 0x0,

    /* U+0035 "5" */
    0x3f, 0xe7, 0xfe, 0x60, 0x6, 0x0, 0x60, 0x6,
    0x0, 0x60, 0xe, 0x0, 0xff, 0x7, 0xfc, 0x1,
    0xe0, 0x7, 0x0, 0x70, 0x7, 0x0, 0x70, 0xf,
    0xc1, 0xef, 0xfc, 0x7f, 0x0,

    /* U+0036 "6" */
    0x3, 0xe0, 0x7f, 0xc7, 0xc, 0x70, 0x7, 0x0,
    0x38, 0x3, 0x80, 0x1d, 0xf0, 0xff, 0xe7, 0xc7,
    0xb8, 0x1d, 0xc0, 0x7e, 0x3, 0xf0, 0x1d, 0xc0,
    0xee, 0xe, 0x38, 0xf0, 0xff, 0x3, 0xe0,

    /* U+0037 "7" */
    0xff, 0xef, 0xff, 0x0, 0x70, 0x6, 0x0, 0xe0,
    0xe, 0x0, 0xc0, 0x1c, 0x1, 0x80, 0x38, 0x3,
    0x0, 0x70, 0x7, 0x0, 0xe0, 0xe, 0x1, 0xc0,
    0x1c, 0x1, 0x80, 0x0, 0x0,

    /* U+0038 "8" */
    0x1f, 0x81, 0xfe, 0x1c, 0x39, 0xc0, 0xee, 0x7,
    0x70, 0x3b, 0x81, 0xce, 0x1c, 0x7f, 0xe3, 0xff,
    0x1c, 0x3d, 0xc0, 0x7e, 0x3, 0xf0, 0x1f, 0x80,
    0xee, 0xe, 0x7d, 0xf1, 0xff, 0x3, 0x60,

    /* U+0039 "9" */
    0xf, 0x81, 0xfe, 0x1e, 0x38, 0xe0, 0xee, 0x7,
    0x70, 0x1f, 0x80, 0xfc, 0x7, 0x70, 0x3b, 0xc7,
    0xcf, 0xfe, 0x1f, 0x70, 0x3, 0x80, 0x38, 0x1,
    0xc0, 0x1c, 0x61, 0xc7, 0xfc, 0xf, 0xc0,

    /* U+003A ":" */
    0xff, 0xf0, 0x0, 0x3, 0xff, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 152, .box_w = 8, .box_h = 2, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 2, .adv_w = 265, .box_w = 15, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 154, .box_w = 8, .box_h = 19, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 226, .box_w = 12, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 86, .adv_w = 219, .box_w = 12, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 115, .adv_w = 235, .box_w = 14, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 149, .adv_w = 224, .box_w = 12, .box_h = 19, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 178, .adv_w = 239, .box_w = 13, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 216, .box_w = 12, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 238, .adv_w = 240, .box_w = 13, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 269, .adv_w = 239, .box_w = 13, .box_h = 19, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 300, .adv_w = 116, .box_w = 3, .box_h = 14, .ofs_x = 2, .ofs_y = 0}
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

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 1, 2, 0, 3, 4, 5, 6,
    7, 8, 9, 2, 10
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 8, 9, 2, 10
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, -18, -18, -11, 7, -2, -31,
    -4, 0, 0, 0, -9, -9, -4, -2,
    -4, -18, -2, 0, -9, -4, -7, -9,
    -2, -9, -9, -7, -4, 0, -4, -2,
    -2, -4, 0, -2, -4, 0, 0, 0,
    0, 0, -4, 0, 0, 2, 0, -7,
    2, 0, -4, -4, -4, -4, -2, 0,
    -7, -4, -4, 0, 0, -4, -2, -4,
    0, 0, -4, -4, 0, 0, -31, -11,
    0, -7, -4, -22, -13, -9, -7, -13,
    -4, -4, -9, -7, -4, -4, -4, -16,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 10,
    .right_class_cnt     = 10,
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
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 1,
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
const lv_font_t tilt_neon_28 = {
#else
lv_font_t tilt_neon_28 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 19,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 2,
    .underline_thickness = 3,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &lv_font_montserrat_28,
#endif
    .user_data = NULL,
};



#endif /*#if TILT_NEON_28*/
