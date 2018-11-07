#include <fstream>

#include "Chams.h"

Chams::Chams()
{
    std::ofstream("csgo\\materials\\simple_regular.vmt") <<        
R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
}

void Chams::render()
{

}
