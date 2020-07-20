//
//  TFLTabs.hpp
//  Einstein
//
//  Created by Matthias Melcher on 7/19/20.
//

#ifndef TFLTabs_hpp
#define TFLTabs_hpp

#include <FL/Fl_Tabs.H>

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

#endif /* TFLTabs_hpp */
