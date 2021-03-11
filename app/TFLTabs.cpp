//
//  TFLTabs.cpp
//  Einstein
//
//  Created by Matthias Melcher on 7/19/20.
//

#include "TFLTabs.h"

#include <FL/fl_draw.H>
#include <FL/Fl_PNG_Image.H>


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



void TFLPCMCIAButton::images(
    unsigned char* inTopIn, unsigned char* inTopOut, unsigned char* inTopHi,
    unsigned char* inBotIn, unsigned char* inBotOut, unsigned char* inBotHi)
{
    mImgTopIn = new Fl_PNG_Image("PCCardTopIn", inTopIn, 0x7fffffff);
    mImgTopOut = new Fl_PNG_Image("PCCardTopOut", inTopOut, 0x7fffffff);
    mImgTopHi = new Fl_PNG_Image("PCCardTopHi", inTopHi, 0x7fffffff);
    mImgBotIn = new Fl_PNG_Image("PCCardBotIn", inBotIn, 0x7fffffff);
    mImgBotOut = new Fl_PNG_Image("PCCardBotOut", inBotOut, 0x7fffffff);
    mImgBotHi = new Fl_PNG_Image("PCCardBotHi", inBotHi, 0x7fffffff);
}

void TFLPCMCIAButton::setTopID(const char* label)
{
    if (label) {
        strncpy(mTopID, label, sizeof(mBottomID - 1));
        mTopID[sizeof(mTopID) - 1] = 0;
    } else
        mTopID[0] = 0;
    redraw();
}

void TFLPCMCIAButton::setBottomID(const char* label)
{
    if (label) {
        strncpy(mBottomID, label, sizeof(mBottomID-1));
        mBottomID[sizeof(mBottomID)-1] = 0;
    }
    else
        mBottomID[0] = 0;
    redraw();
}

void TFLPCMCIAButton::draw()
{
    if (type() == FL_HIDDEN_BUTTON) return;

    fl_push_clip(x(), y(), w(), h());

    Fl_Color col = value() ? selection_color() : color();
    draw_box(value() ? (down_box() ? down_box() : fl_down(box())) : box(), col);

    int imgX = x()+2, imgY = y()+2;
    if (mMouseFocus == kMouseInUpperHalf) mImgTopHi->draw(imgX, imgY);
    if (mMouseFocus == kMouseInLowerHalf) mImgBotHi->draw(imgX, imgY);

    if (mTopID[0]) {
        mImgTopIn->draw(imgX, imgY);
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA_BOLD, 8);
        fl_draw(mTopID, imgX + 23, imgY + 13);
    } else {
        mImgTopOut->draw(imgX, imgY);
    }
    if (mBottomID[0]) {
        mImgBotIn->draw(imgX, imgY);
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA_BOLD, 8);
        fl_draw(mBottomID, imgX + 23, imgY + 25);
    } else {
        mImgBotOut->draw(imgX, imgY);
    }

    if (labeltype() == FL_NORMAL_LABEL && value()) {
        Fl_Color c = labelcolor();
        labelcolor(fl_contrast(c, col));
        draw_label();
        labelcolor(c);
    } else draw_label();
    if (Fl::focus() == this) draw_focus();

    fl_pop_clip();
}

int TFLPCMCIAButton::handle(int inEvent)
{
    int oldMouseFocus = mMouseFocus;

    switch (inEvent) {
    case FL_ENTER: // make sure that we will get FL_MOVE events
        Fl_Button::handle(inEvent);
        return 1;
    case FL_MOVE:
        mMouseFocus = (Fl::event_y() > y() + h() / 2) ? kMouseInLowerHalf : kMouseInUpperHalf;
        if (mMouseFocus != oldMouseFocus) redraw();
        break;
    case FL_LEAVE:
        mMouseFocus = kMouseOutside;
        redraw();
        break;
    }
    return Fl_Button::handle(inEvent);
}