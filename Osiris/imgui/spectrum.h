#pragma once

/*
Color definitions in ImGui are a good starting point, 
but do not cover all the intricacies of Spectrum's possible colors
in controls and widgets. 

One big difference is that ImGui communicates widget activity 
(hover, pressed) with their background, while spectrum uses a mix
of background and border, with border being the most common choice.

Because of this, we reference extra colors in spectrum from 
imgui.cpp and imgui_widgets.cpp directly, and to make that work, 
we need to have them defined at here at compile time. 
*/

/// Pick one
#define SPECTRUM_USE_LIGHT_THEME
//#define SPECTRUM_USE_DARK_THEME

namespace ImGui {
    namespace Spectrum {
        // a list of changes introduced to change the look of the widgets. 
        // Collected here as const rather than being magic numbers spread 
        // around imgui.cpp and imgui_widgets.cpp.
        const float CHECKBOX_BORDER_SIZE = 2.0f;
        const float CHECKBOX_ROUNDING = 2.0f;

        void StyleColorsSpectrum();

        namespace { // Unnamed namespace, since we only use this here. 
            const unsigned int Color(unsigned int c) {
                // add alpha.
                // also swap red and blue channel for some reason.
                // todo: figure out why, and fix it.
                const short a = 0xFF;
                const short r = (c >> 16) & 0xFF;
                const short g = (c >> 8) & 0xFF;
                const short b = (c >> 0) & 0xFF;
                return(a << 24)
                    | (r << 0)
                    | (g << 8)
                    | (b << 16);
            }
        }
        // all colors are from http://spectrum.corp.adobe.com/color.html

        inline unsigned int color_alpha(unsigned int alpha, unsigned int c) {
            return ((alpha & 0xFF) << 24) | (c & 0x00FFFFFF);
        }

        namespace Static { // static colors
            const unsigned int NONE = 0x00000000; // transparent
            const unsigned int WHITE = Color(0xFFFFFF);
            const unsigned int BLACK = Color(0x000000);
            const unsigned int GRAY200 = Color(0xF4F4F4);
            const unsigned int GRAY300 = Color(0xEAEAEA);
            const unsigned int GRAY400 = Color(0xD3D3D3);
            const unsigned int GRAY500 = Color(0xBCBCBC);
            const unsigned int GRAY600 = Color(0x959595);
            const unsigned int GRAY700 = Color(0x767676);
            const unsigned int GRAY800 = Color(0x505050);
            const unsigned int GRAY900 = Color(0x323232);
            const unsigned int BLUE400 = Color(0x378EF0);
            const unsigned int BLUE500 = Color(0x2680EB);
            const unsigned int BLUE600 = Color(0x1473E6);
            const unsigned int BLUE700 = Color(0x0D66D0);
            const unsigned int RED400 = Color(0xEC5B62);
            const unsigned int RED500 = Color(0xE34850);
            const unsigned int RED600 = Color(0xD7373F);
            const unsigned int RED700 = Color(0xC9252D);
            const unsigned int ORANGE400 = Color(0xF29423);
            const unsigned int ORANGE500 = Color(0xE68619);
            const unsigned int ORANGE600 = Color(0xDA7B11);
            const unsigned int ORANGE700 = Color(0xCB6F10);
            const unsigned int GREEN400 = Color(0x33AB84);
            const unsigned int GREEN500 = Color(0x2D9D78);
            const unsigned int GREEN600 = Color(0x268E6C);
            const unsigned int GREEN700 = Color(0x12805C);
        }

#ifdef SPECTRUM_USE_LIGHT_THEME
        const unsigned int GRAY50 = Color(0xFFFFFF);
        const unsigned int GRAY75 = Color(0xFAFAFA);
        const unsigned int GRAY100 = Color(0xF5F5F5);
        const unsigned int GRAY200 = Color(0xEAEAEA);
        const unsigned int GRAY300 = Color(0xE1E1E1);
        const unsigned int GRAY400 = Color(0xCACACA);
        const unsigned int GRAY500 = Color(0xB3B3B3);
        const unsigned int GRAY600 = Color(0x8E8E8E);
        const unsigned int GRAY700 = Color(0x707070);
        const unsigned int GRAY800 = Color(0x4B4B4B);
        const unsigned int GRAY900 = Color(0x2C2C2C);
        const unsigned int BLUE400 = Color(0x2680EB);
        const unsigned int BLUE500 = Color(0x1473E6);
        const unsigned int BLUE600 = Color(0x0D66D0);
        const unsigned int BLUE700 = Color(0x095ABA);
        const unsigned int RED400 = Color(0xE34850);
        const unsigned int RED500 = Color(0xD7373F);
        const unsigned int RED600 = Color(0xC9252D);
        const unsigned int RED700 = Color(0xBB121A);
        const unsigned int ORANGE400 = Color(0xE68619);
        const unsigned int ORANGE500 = Color(0xDA7B11);
        const unsigned int ORANGE600 = Color(0xCB6F10);
        const unsigned int ORANGE700 = Color(0xBD640D);
        const unsigned int GREEN400 = Color(0x2D9D78);
        const unsigned int GREEN500 = Color(0x268E6C);
        const unsigned int GREEN600 = Color(0x12805C);
        const unsigned int GREEN700 = Color(0x107154);
        const unsigned int INDIGO400 = Color(0x6767EC);
        const unsigned int INDIGO500 = Color(0x5C5CE0);
        const unsigned int INDIGO600 = Color(0x5151D3);
        const unsigned int INDIGO700 = Color(0x4646C6);
        const unsigned int CELERY400 = Color(0x44B556);
        const unsigned int CELERY500 = Color(0x3DA74E);
        const unsigned int CELERY600 = Color(0x379947);
        const unsigned int CELERY700 = Color(0x318B40);
        const unsigned int MAGENTA400 = Color(0xD83790);
        const unsigned int MAGENTA500 = Color(0xCE2783);
        const unsigned int MAGENTA600 = Color(0xBC1C74);
        const unsigned int MAGENTA700 = Color(0xAE0E66);
        const unsigned int YELLOW400 = Color(0xDFBF00);
        const unsigned int YELLOW500 = Color(0xD2B200);
        const unsigned int YELLOW600 = Color(0xC4A600);
        const unsigned int YELLOW700 = Color(0xB79900);
        const unsigned int FUCHSIA400 = Color(0xC038CC);
        const unsigned int FUCHSIA500 = Color(0xB130BD);
        const unsigned int FUCHSIA600 = Color(0xA228AD);
        const unsigned int FUCHSIA700 = Color(0x93219E);
        const unsigned int SEAFOAM400 = Color(0x1B959A);
        const unsigned int SEAFOAM500 = Color(0x16878C);
        const unsigned int SEAFOAM600 = Color(0x0F797D);
        const unsigned int SEAFOAM700 = Color(0x096C6F);
        const unsigned int CHARTREUSE400 = Color(0x85D044);
        const unsigned int CHARTREUSE500 = Color(0x7CC33F);
        const unsigned int CHARTREUSE600 = Color(0x73B53A);
        const unsigned int CHARTREUSE700 = Color(0x6AA834);
        const unsigned int PURPLE400 = Color(0x9256D9);
        const unsigned int PURPLE500 = Color(0x864CCC);
        const unsigned int PURPLE600 = Color(0x7A42BF);
        const unsigned int PURPLE700 = Color(0x6F38B1);
#endif
#ifdef SPECTRUM_USE_DARK_THEME
        const unsigned int GRAY50 = Color(0x252525);
        const unsigned int GRAY75 = Color(0x2F2F2F);
        const unsigned int GRAY100 = Color(0x323232);
        const unsigned int GRAY200 = Color(0x393939);
        const unsigned int GRAY300 = Color(0x3E3E3E);
        const unsigned int GRAY400 = Color(0x4D4D4D);
        const unsigned int GRAY500 = Color(0x5C5C5C);
        const unsigned int GRAY600 = Color(0x7B7B7B);
        const unsigned int GRAY700 = Color(0x999999);
        const unsigned int GRAY800 = Color(0xCDCDCD);
        const unsigned int GRAY900 = Color(0xFFFFFF);
        const unsigned int BLUE400 = Color(0x2680EB);
        const unsigned int BLUE500 = Color(0x378EF0);
        const unsigned int BLUE600 = Color(0x4B9CF5);
        const unsigned int BLUE700 = Color(0x5AA9FA);
        const unsigned int RED400 = Color(0xE34850);
        const unsigned int RED500 = Color(0xEC5B62);
        const unsigned int RED600 = Color(0xF76D74);
        const unsigned int RED700 = Color(0xFF7B82);
        const unsigned int ORANGE400 = Color(0xE68619);
        const unsigned int ORANGE500 = Color(0xF29423);
        const unsigned int ORANGE600 = Color(0xF9A43F);
        const unsigned int ORANGE700 = Color(0xFFB55B);
        const unsigned int GREEN400 = Color(0x2D9D78);
        const unsigned int GREEN500 = Color(0x33AB84);
        const unsigned int GREEN600 = Color(0x39B990);
        const unsigned int GREEN700 = Color(0x3FC89C);
        const unsigned int INDIGO400 = Color(0x6767EC);
        const unsigned int INDIGO500 = Color(0x7575F1);
        const unsigned int INDIGO600 = Color(0x8282F6);
        const unsigned int INDIGO700 = Color(0x9090FA);
        const unsigned int CELERY400 = Color(0x44B556);
        const unsigned int CELERY500 = Color(0x4BC35F);
        const unsigned int CELERY600 = Color(0x51D267);
        const unsigned int CELERY700 = Color(0x58E06F);
        const unsigned int MAGENTA400 = Color(0xD83790);
        const unsigned int MAGENTA500 = Color(0xE2499D);
        const unsigned int MAGENTA600 = Color(0xEC5AAA);
        const unsigned int MAGENTA700 = Color(0xF56BB7);
        const unsigned int YELLOW400 = Color(0xDFBF00);
        const unsigned int YELLOW500 = Color(0xEDCC00);
        const unsigned int YELLOW600 = Color(0xFAD900);
        const unsigned int YELLOW700 = Color(0xFFE22E);
        const unsigned int FUCHSIA400 = Color(0xC038CC);
        const unsigned int FUCHSIA500 = Color(0xCF3EDC);
        const unsigned int FUCHSIA600 = Color(0xD951E5);
        const unsigned int FUCHSIA700 = Color(0xE366EF);
        const unsigned int SEAFOAM400 = Color(0x1B959A);
        const unsigned int SEAFOAM500 = Color(0x20A3A8);
        const unsigned int SEAFOAM600 = Color(0x23B2B8);
        const unsigned int SEAFOAM700 = Color(0x26C0C7);
        const unsigned int CHARTREUSE400 = Color(0x85D044);
        const unsigned int CHARTREUSE500 = Color(0x8EDE49);
        const unsigned int CHARTREUSE600 = Color(0x9BEC54);
        const unsigned int CHARTREUSE700 = Color(0xA3F858);
        const unsigned int PURPLE400 = Color(0x9256D9);
        const unsigned int PURPLE500 = Color(0x9D64E1);
        const unsigned int PURPLE600 = Color(0xA873E9);
        const unsigned int PURPLE700 = Color(0xB483F0);
#endif
    }
}
