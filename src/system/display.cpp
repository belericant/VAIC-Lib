#include "system/display.h"
#include "funni.c"
#include "system/Components.h"
#include "utils/sync.h"

static lv_theme_t* th;
static lv_obj_t* tabview;
static std::unordered_map<std::string, std::string> constantsMap;
static lv_obj_t* const_table;
static int botIdNum = -1;

void gui_initialize() {
    lib7405x::display::Terminal::serial = new lib7405x::display::Terminal();
    lib7405x::display::Terminal::logs = new lib7405x::display::Terminal();
    th = lv_theme_alien_init(210, nullptr);
    lib7405x::display::reloadConstants();
    lib7405x::display::renderGUI();
}

//Main function
void lib7405x::display::renderGUI() {
    lv_theme_set_current(th);

    /*Create a Tab view object*/
    tabview = lv_tabview_create(lv_scr_act(), nullptr);
    //Shorten Tab Height
    static lv_style_t shorterTabStyle;
    lv_style_copy(&shorterTabStyle, th->tabview.btn.bg);
    shorterTabStyle.body.padding.ver = -10;
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_BG, &shorterTabStyle);

    //Shorten Indic Height
    static lv_style_t shorterTabIndicStyle;
    lv_style_copy(&shorterTabIndicStyle, th->tabview.indic);
    shorterTabIndicStyle.body.padding.inner = 5;
    lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_INDIC, &shorterTabIndicStyle);

    //Create Tab Labels
    static lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "Home");
    static lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "Serial");
    static lv_obj_t* tab3 = lv_tabview_add_tab(tabview, "Logs");
    static lv_obj_t* tab4 = lv_tabview_add_tab(tabview, "Config");
    static lv_obj_t* tab5 = lv_tabview_add_tab(tabview, "Funni");
    create_home_tab(tab1);
    Terminal::serial->setParent(tab2);
    Terminal::logs->setParent(tab3);
    create_config_tab(tab4);
    create_funni_tab(tab5);
}
/*===============Funni Tab===============*/
void lib7405x::display::create_funni_tab(lv_obj_t* parent) {
    lv_obj_t* img1 = lv_img_create(parent, nullptr);
    lv_img_set_src(img1, &funni);
}
/*===============HOME TAB ===============*/
typedef struct ancestor_btn_s {
    lv_btn_ext_t btn;
    lib7405x::display::team_e_t teamColor;
} ancestor_btn_s_t;

static lv_obj_t* teambtn;
static lv_obj_t* teambtn_label;
static lv_obj_t* botId;

lv_res_t lib7405x::display::teamChange(lv_obj_t* button) {
    auto* ext = static_cast<ancestor_btn_s_t*>(lv_obj_get_ext_attr(button));
    ext->teamColor = static_cast<team_e_t>(ext->teamColor ^ 1);
    lv_style_t* btn_style = lv_obj_get_style(button);
    lv_label_set_text(lv_obj_get_child(button, nullptr), ext->teamColor ? "RED TEAM" : "BLUE TEAM");
    btn_style->body.main_color = ext->teamColor ? LV_COLOR_RED : LV_COLOR_BLUE;
    btn_style->body.grad_color = ext->teamColor ? LV_COLOR_RED : LV_COLOR_BLUE;
    return LV_RES_OK;
}

typedef enum reload_btn_e {
    CONFIG,
    PORTS,
    TEAM,
} reload_btn_e_t;

static void render_config() {
    auto elem = constantsMap.begin();
    for (uint32_t i = 0; i < constantsMap.size(); ++i, ++elem) {
        lv_table_set_cell_value(const_table, i, 0, (elem->first).c_str());
        lv_table_set_cell_value(const_table, i, 1, (elem->second).c_str());
    }
    lv_label_set_text(botId, ("BOT-ID: " + std::to_string(botIdNum)).c_str());
}

static lv_res_t reload_btn_click_action(lv_obj_t* btn) {
    if (reload_btn_e_t::CONFIG == lv_obj_get_free_num(btn)) {
        lib7405x::Components::Instance()->reloadConfig();
        lib7405x::display::reloadConstants();
        render_config();
    } else if (reload_btn_e_t::PORTS == lv_obj_get_free_num(btn)) {
        lib7405x::Components::Instance()->updatePortBindings();
    } else {
        //TODO Reload Team
    }
    return LV_RES_OK;
}

void lib7405x::display::create_home_tab(lv_obj_t* parent) {
    lv_page_set_sb_mode(parent, LV_SB_MODE_OFF);

    //botID
    botId = lv_label_create(parent, nullptr);
    lv_label_set_text(botId, ("BOT-ID: " + std::to_string(botIdNum)).c_str());
    lv_obj_align(botId, parent, LV_ALIGN_IN_TOP_LEFT, 20, 0);

    //Reload buttons

    //--button styles
    static lv_style_t good_lookin_buttons_REL;
    lv_style_copy(&good_lookin_buttons_REL, th->btn.rel);
    static lv_style_t good_lookin_buttons_PR;
    lv_style_copy(&good_lookin_buttons_PR, th->btn.pr);

    good_lookin_buttons_REL.body.radius = 0;
    good_lookin_buttons_REL.body.padding.ver = 5;
    good_lookin_buttons_REL.body.padding.hor = 15;

    good_lookin_buttons_PR.body.radius = 0;
    good_lookin_buttons_PR.body.padding.ver = 5;
    good_lookin_buttons_PR.body.padding.hor = 15;


    //--buttons


    //    lv_obj_t* reloadBtn2 = lv_btn_create(parent, nullptr);
    //    lv_btn_set_style(reloadBtn2, LV_BTN_STYLE_REL, &good_lookin_buttons_REL);
    //    lv_btn_set_style(reloadBtn2, LV_BTN_STYLE_PR, &good_lookin_buttons_PR);
    //    lv_cont_set_fit(reloadBtn2, true, true);
    //    lv_obj_align(reloadBtn2, reloadBtn1, LV_ALIGN_OUT_RIGHT_MID, -10, 0);
    //    lv_obj_set_free_num(reloadBtn2, reload_btn_e_t::TEAM);
    //    lv_btn_set_action(reloadBtn2, LV_BTN_ACTION_CLICK, reload_btn_click_action);
    //    lv_obj_t* label2 = lv_label_create(reloadBtn2, nullptr);
    //    lv_label_set_text(label2, "Team");
    //
    //    lv_obj_t * reloadLabel = lv_label_create(parent, nullptr);
    //    lv_obj_align(reloadLabel, reloadBtn1, LV_ALIGN_OUT_TOP_MID, -60, -10);
    //    lv_label_set_text(reloadLabel, "Reloaders");
    //team button
    teambtn = lv_btn_create(parent, nullptr);

    //magic
    lv_btn_set_action(teambtn, LV_BTN_ACTION_CLICK, teamChange); /*Assign an action function*/
    lv_obj_allocate_ext_attr(teambtn, sizeof(ancestor_btn_s_t)); /*Re-alloacte the extended data*/
    auto* ext = static_cast<ancestor_btn_s_t*>(lv_obj_get_ext_attr(teambtn));
    ext->teamColor = RED;

    lv_obj_align(teambtn, parent, LV_ALIGN_IN_BOTTOM_RIGHT, -100, 20);
    //styles
    static lv_style_t squareButtonREL;
    lv_style_copy(&squareButtonREL, &lv_style_plain);
    squareButtonREL.body.padding.ver = 7;
    squareButtonREL.body.padding.hor = 0;
    squareButtonREL.body.radius = 0;
    squareButtonREL.body.main_color = LV_COLOR_RED;
    squareButtonREL.body.grad_color = LV_COLOR_RED;
    static lv_style_t squareButtonPR;
    lv_style_copy(&squareButtonPR, th->btn.pr);
    squareButtonPR.body.padding.ver = 7;
    squareButtonPR.body.padding.hor = 0;
    squareButtonPR.body.radius = 0;
    squareButtonPR.body.main_color = LV_COLOR_RED;
    squareButtonPR.body.grad_color = LV_COLOR_RED;
    lv_btn_set_style(teambtn, LV_BTN_STYLE_REL, &squareButtonREL);
    lv_btn_set_style(teambtn, LV_BTN_STYLE_PR, &squareButtonPR);
    lv_obj_set_size(teambtn, 200, 50);
    teambtn_label = lv_label_create(teambtn, nullptr);
    static lv_style_t largeText;
    lv_style_copy(&largeText, th->label.sec);
    largeText.text.font = &lv_font_dejavu_40;//make font big
    lv_obj_set_style(teambtn_label, &largeText);
    lv_label_set_text(teambtn_label, ext->teamColor ? "RED TEAM" : "BLUE TEAM");

    //Reload btns
    lv_obj_t* reloadBtn = lv_btn_create(parent, nullptr);
    lv_btn_set_style(reloadBtn, LV_BTN_STYLE_REL, &good_lookin_buttons_REL);
    lv_btn_set_style(reloadBtn, LV_BTN_STYLE_PR, &good_lookin_buttons_PR);
    lv_cont_set_fit(reloadBtn, true, true);
    lv_obj_align(reloadBtn, teambtn, LV_ALIGN_OUT_TOP_RIGHT, 13, 0);
    lv_obj_set_free_num(reloadBtn, reload_btn_e_t::CONFIG);
    lv_btn_set_action(reloadBtn, LV_BTN_ACTION_CLICK, reload_btn_click_action);
    lv_obj_t* label = lv_label_create(reloadBtn, nullptr);
    lv_label_set_text(label, "Reload Config");
    lv_obj_set_size(reloadBtn, 100, 50);
    lv_obj_t* reloadBtn1 = lv_btn_create(parent, nullptr);
    lv_btn_set_style(reloadBtn1, LV_BTN_STYLE_REL, &good_lookin_buttons_REL);
    lv_btn_set_style(reloadBtn1, LV_BTN_STYLE_PR, &good_lookin_buttons_PR);
    lv_cont_set_fit(reloadBtn1, true, true);
    lv_obj_align(reloadBtn1, reloadBtn, LV_ALIGN_OUT_TOP_RIGHT, 13, 0);
    lv_obj_set_free_num(reloadBtn1, reload_btn_e_t::PORTS);
    lv_btn_set_action(reloadBtn1, LV_BTN_ACTION_CLICK, reload_btn_click_action);
    lv_obj_t* label1 = lv_label_create(reloadBtn1, nullptr);
    lv_label_set_text(label1, "Reload Ports");
    lv_obj_set_size(reloadBtn1, 100, 50);
}

lib7405x::display::team_e_t lib7405x::display::getTeam() {
    return static_cast<ancestor_btn_s_t*>(lv_obj_get_ext_attr(teambtn))->teamColor;
}

/*=============TERMINAL CLASS===================*/

void lib7405x::display::Terminal::setParent(lv_obj_t* parent) {
    _parent = parent;
    render();
}

void lib7405x::display::Terminal::updateText() {
    for (uint32_t i = startIdx; i < startIdx + LINE_MAX; ++i) {
        auto idx = i % LINE_MAX;
        if (!(i - startIdx)) {
            continue;
        }
        if (!terminal_text[idx].empty()) {
            lv_label_set_text(lcd_text[(i - startIdx) % LINE_MAX], "");
            lv_label_set_text(lcd_text[(i - startIdx) % LINE_MAX], terminal_text[idx].c_str());
            lv_obj_set_width(lcd_text[(i - startIdx) % LINE_MAX], 426);
        }
    }
}

void lib7405x::display::Terminal::println(const std::string& str) {
    synchronized(this) {
        terminal_text[startIdx] = str;
        startIdx = (startIdx + 1) % (LINE_MAX);
        updateText();
    }
}

void lib7405x::display::Terminal::render() {
    for (auto& i : terminal_text) {
        i = std::string("");
    }
    lv_page_set_sb_mode(_parent, LV_SB_MODE_OFF);//Scrollbar OFF
    static lv_style_t style_txt;
    lv_style_copy(&style_txt, &lv_style_plain);
    style_txt.text.font = &pros_font_dejavu_mono_20;

    style_txt.text.letter_space = 2;
    style_txt.text.line_space = 3;
    style_txt.text.color = LV_COLOR_HEX(0xFFFFFF);

    for (size_t i = 0; i < LINE_MAX; ++i) {
        lcd_text[i] = lv_label_create(_parent, nullptr);
        lv_obj_set_width(lcd_text[i], 426);
        lv_obj_align(lcd_text[i], nullptr, LV_ALIGN_IN_TOP_LEFT, 5, 20 * i);
        lv_label_set_align(lcd_text[i], LV_LABEL_ALIGN_LEFT);
        lv_label_set_long_mode(lcd_text[i], LV_LABEL_LONG_CROP);
        lv_label_set_text(lcd_text[i], "");
    }
}

/*=============CONFIG TAB===================*/

void lib7405x::display::reloadConstants() {
    constantsMap = Components::Instance()->getConstants();
    botIdNum = std::stoi(constantsMap.at("id"));
}

void lib7405x::display::create_config_tab(lv_obj_t* parent) {
    static lv_style_t fat_scrollbar_style;
    lv_style_copy(&fat_scrollbar_style, th->page.sb);
    fat_scrollbar_style.body.padding.inner = 80;
    lv_page_set_style(parent, LV_PAGE_STYLE_SB, &fat_scrollbar_style);
    const_table = lv_table_create(parent, nullptr);
    lv_table_set_col_cnt(const_table, 2);
    lv_table_set_row_cnt(const_table, constantsMap.size());
    lv_table_set_col_width(const_table, 0, 190);
    lv_table_set_col_width(const_table, 1, 190);
    render_config();
}