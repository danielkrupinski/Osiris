#pragma once

#include <Utils/StringBuilder.h>

inline void buildFootstepDirectionScript(StringBuilder& script, float dx, float dy,
    float rx, float ry, bool firstArrow) noexcept
{
    // Integer serialization avoids introducing floating-point CRT
    // formatting or trigonometry imports into the Windows release DLL.
    script.put("(function(){var p=$.GetContextPanel(),root=p.GetParent();",
        "var dx=", static_cast<int>(dx * 100.0f), ",dy=", static_cast<int>(dy * 100.0f),
        ",rx=", static_cast<int>(rx * 1000000.0f), ",ry=", static_cast<int>(ry * 1000000.0f), ";",
        "var n=Math.sqrt(rx*rx+ry*ry);rx/=n;ry/=n;",
        "var right=dx*rx+dy*ry,forward=-dx*ry+dy*rx;",
        "var a=Math.atan2(right,forward),lane=0;");
    if (firstArrow)
        script.put("root.osirisFootstepAngles=[];");
    script.put("for(var i=0;i<root.osirisFootstepAngles.length;i++){",
        "var d=a-root.osirisFootstepAngles[i];if(Math.abs(Math.atan2(Math.sin(d),Math.cos(d)))<0.21)lane++;}",
        "root.osirisFootstepAngles.push(a);",
        "var radius=Math.min(180,Math.min(root.actuallayoutwidth,root.actuallayoutheight)*0.18)+lane*16;",
        "p.style.transform='translate3d('+(Math.sin(a)*radius)+'px,'+(-Math.cos(a)*radius)+'px,0px) rotateZ('+(a*180/Math.PI)+'deg)';",
        "})();");
}
