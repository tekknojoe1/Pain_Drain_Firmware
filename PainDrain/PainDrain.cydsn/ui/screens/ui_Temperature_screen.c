// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.1
// LVGL version: 8.3.6
// Project name: Pain Drain

#include "../ui.h"

void ui_Temperature_screen_screen_init(void)
{
    ui_Temperature_screen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Temperature_screen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Temperature_screen, lv_color_hex(0x615A5A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Temperature_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Temperature_screen, lv_color_hex(0xEFEFEF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Temperature_screen, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Temperature_label = lv_label_create(ui_Temperature_screen);
    lv_obj_set_width(ui_Temperature_label, LV_SIZE_CONTENT);   /// 15
    lv_obj_set_height(ui_Temperature_label, LV_SIZE_CONTENT);    /// 5
    lv_obj_set_x(ui_Temperature_label, -4);
    lv_obj_set_y(ui_Temperature_label, lv_pct(12));
    lv_obj_set_align(ui_Temperature_label, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Temperature_label, "Temperature");
    lv_obj_set_style_text_font(ui_Temperature_label, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_scrollDotsPg2 = lv_obj_create(ui_Temperature_screen);
    lv_obj_set_width(ui_scrollDotsPg2, 145);
    lv_obj_set_height(ui_scrollDotsPg2, 30);
    lv_obj_set_x(ui_scrollDotsPg2, 0);
    lv_obj_set_y(ui_scrollDotsPg2, -15);
    lv_obj_set_align(ui_scrollDotsPg2, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_scrollDotsPg2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_scrollDotsPg2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrollDotsPg2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_scrollDotsPg2, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_scrollDotsPg2, lv_color_hex(0xFEFDFD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_scrollDotsPg2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_blend_mode(ui_scrollDotsPg2, LV_BLEND_MODE_NORMAL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_scrollDotsPg2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_dot1Pg2 = lv_obj_create(ui_scrollDotsPg2);
    lv_obj_set_width(ui_dot1Pg2, 20);
    lv_obj_set_height(ui_dot1Pg2, 20);
    lv_obj_set_x(ui_dot1Pg2, -60);
    lv_obj_set_y(ui_dot1Pg2, 10);
    lv_obj_set_align(ui_dot1Pg2, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_dot1Pg2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_dot1Pg2, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_dot1Pg2, lv_color_hex(0x979797), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dot1Pg2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_dot2Pg2 = lv_obj_create(ui_scrollDotsPg2);
    lv_obj_set_width(ui_dot2Pg2, 20);
    lv_obj_set_height(ui_dot2Pg2, 20);
    lv_obj_set_x(ui_dot2Pg2, -30);
    lv_obj_set_y(ui_dot2Pg2, 10);
    lv_obj_set_align(ui_dot2Pg2, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_dot2Pg2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_dot2Pg2, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_dot2Pg2, lv_color_hex(0x383838), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dot2Pg2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_dot3Pg2 = lv_obj_create(ui_scrollDotsPg2);
    lv_obj_set_width(ui_dot3Pg2, 20);
    lv_obj_set_height(ui_dot3Pg2, 20);
    lv_obj_set_x(ui_dot3Pg2, 0);
    lv_obj_set_y(ui_dot3Pg2, 10);
    lv_obj_set_align(ui_dot3Pg2, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_dot3Pg2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_dot3Pg2, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_dot3Pg2, lv_color_hex(0x979797), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dot3Pg2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_dot4Pg2 = lv_obj_create(ui_scrollDotsPg2);
    lv_obj_set_width(ui_dot4Pg2, 20);
    lv_obj_set_height(ui_dot4Pg2, 20);
    lv_obj_set_x(ui_dot4Pg2, 30);
    lv_obj_set_y(ui_dot4Pg2, 10);
    lv_obj_set_align(ui_dot4Pg2, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_dot4Pg2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_dot4Pg2, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_dot4Pg2, lv_color_hex(0x979797), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dot4Pg2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_dot5Pg2 = lv_obj_create(ui_scrollDotsPg2);
    lv_obj_set_width(ui_dot5Pg2, 20);
    lv_obj_set_height(ui_dot5Pg2, 20);
    lv_obj_set_x(ui_dot5Pg2, 60);
    lv_obj_set_y(ui_dot5Pg2, 10);
    lv_obj_set_align(ui_dot5Pg2, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_dot5Pg2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_dot5Pg2, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_dot5Pg2, lv_color_hex(0x979797), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_dot5Pg2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Temperature_slider = lv_arc_create(ui_Temperature_screen);
    lv_obj_set_width(ui_Temperature_slider, 150);
    lv_obj_set_height(ui_Temperature_slider, 150);
    lv_obj_set_align(ui_Temperature_slider, LV_ALIGN_CENTER);
    lv_arc_set_range(ui_Temperature_slider, 0, 10);
    lv_arc_set_value(ui_Temperature_slider, 5);
    lv_arc_set_mode(ui_Temperature_slider, LV_ARC_MODE_SYMMETRICAL);

    ui_snowflake = lv_img_create(ui_Temperature_screen);
    lv_img_set_src(ui_snowflake, &ui_img_snowflake_icon_small_png);
    lv_obj_set_width(ui_snowflake, LV_SIZE_CONTENT);   /// 30
    lv_obj_set_height(ui_snowflake, LV_SIZE_CONTENT);    /// 30
    lv_obj_set_x(ui_snowflake, -50);
    lv_obj_set_y(ui_snowflake, 80);
    lv_obj_set_align(ui_snowflake, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_snowflake, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_snowflake, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_fire = lv_img_create(ui_Temperature_screen);
    lv_img_set_src(ui_fire, &ui_img_fire_icon_small_png);
    lv_obj_set_width(ui_fire, LV_SIZE_CONTENT);   /// 30
    lv_obj_set_height(ui_fire, LV_SIZE_CONTENT);    /// 30
    lv_obj_set_x(ui_fire, 50);
    lv_obj_set_y(ui_fire, 80);
    lv_obj_set_align(ui_fire, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_fire, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_fire, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_add_event_cb(ui_Temperature_screen, ui_event_Temperature_screen, LV_EVENT_ALL, NULL);

}
