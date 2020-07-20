//
//  TFLTabs.cpp
//  Einstein
//
//  Created by Matthias Melcher on 7/19/20.
//

#include "TFLTabs.h"

#include <FL/fl_draw.H>


void TFLTabs::draw_tab(int x1, int x2, int W, int H, Fl_Widget* o, int what) {
    int sel = (what == 2); // SELECTED
    int dh = Fl::box_dh(box());
    int dy = Fl::box_dy(box());
    char prev_draw_shortcut = fl_draw_shortcut;
    fl_draw_shortcut = 1;

    Fl_Boxtype bt = (o == push() && !sel) ? fl_down(box()) : box();
    Fl_Color bc = sel ? selection_color() : o->selection_color();

    // Save the label color and label type
    Fl_Color oc = o->labelcolor();
    Fl_Labeltype ot = o->labeltype();
    Fl_Font of = o->labelfont();

    // Set a labeltype that really draws a label
    if (ot == FL_NO_LABEL)
        o->labeltype(FL_NORMAL_LABEL);

    // compute offsets to make selected tab look bigger
    int yofs = 0; //sel ? 0 : 2;

    if ((x2 < x1+W) && what == 1 /* RIGHT */) x1 = x2 - W;

    H += dh;

    draw_box(FL_FLAT_BOX, x1, y() + yofs, W, H - yofs, bc);

    if (sel) {
        o->labelcolor(FL_SELECTION_COLOR);
        o->labelfont(of+FL_BOLD);
//        o->labeltype(FL_SHADOW_LABEL);
//        o->labeltype(FL_EMBOSSED_LABEL);
//        fl_rectf(x1, y() + H - 2, W, 2, fl_color_average(FL_FOREGROUND_COLOR, bc, 0.5));
        fl_rectf(x1, y() + H - 2, W, 2, FL_SELECTION_COLOR);
    }
    o->draw_label(x1, y() + yofs, W, H - yofs, tab_align());

    if (Fl::focus() == this && o->visible())
        draw_focus(box(), x1, y()+yofs, W, H-yofs-3);
    fl_draw_shortcut = prev_draw_shortcut;

    // Restore the original label color and label type
    o->labelfont(of);
    o->labelcolor(oc);
    o->labeltype(ot);
}

