//
//  TFLTabs.hpp
//  Einstein
//
//  Created by Matthias Melcher on 7/19/20.
//

#ifndef TFLTabs_hpp
#define TFLTabs_hpp

#include <FL/Fl_Tabs.H>
#include <FL/Fl_Button.H>

class TFLTabs: public Fl_Tabs
{
public:
    TFLTabs(int x, int y, int w, int h, const char *label=nullptr)
    : Fl_Tabs(x, y, w, h, label)
    { }
    ~TFLTabs() = default;
protected:
    void draw_tab(int x1, int x2, int W, int H, Fl_Widget* o, int sel=0) override;
};


class TFLPCMCIAButton : public Fl_Button
{
public:
    TFLPCMCIAButton(int x, int y, int w, int h, const char* l = nullptr)
        : Fl_Button(x, y, w, h, l)
    {}
    ~TFLPCMCIAButton() = default;
    void images(
        unsigned char *inTopIn, unsigned char* inTopOut, unsigned char* inTopHi, 
        unsigned char* inBotIn, unsigned char* inBotOut, unsigned char* inBotHi);
    void setTopID(const char* label);
    void clearTopID() { setTopID(nullptr); }
    void setBottomID(const char* label);
    void clearBottomID() { setBottomID(nullptr); }
protected:
    const int kMouseOutside = 0;
    const int kMouseInLowerHalf = 1;
    const int kMouseInUpperHalf = 2;

    int mMouseFocus = kMouseOutside;

    Fl_Image* mImgTopIn = nullptr;
    Fl_Image* mImgTopOut = nullptr;
    Fl_Image* mImgTopHi = nullptr;
    Fl_Image* mImgBotIn = nullptr;
    Fl_Image* mImgBotOut = nullptr;
    Fl_Image* mImgBotHi = nullptr;

    void draw() override;
    int handle(int inEvent) override;

    char mTopID[32] = { 0 };
    char mBottomID[32] = { 0 };
};

#endif /* TFLTabs_hpp */
