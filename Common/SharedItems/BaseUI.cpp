#include "BaseUI.h"

GuiScale computeGuiScale(int windowW, int windowH, int minVirtualW, int minVirtualH)
{
   int scaleX = windowW / minVirtualW;  
   int scaleY = windowH / minVirtualH;  
   int scale = std::max(1, std::min(scaleX, scaleY)); // Removed extra closing parenthesis  
   return {scale, windowW, windowH}; 
}
