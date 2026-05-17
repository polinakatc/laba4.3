#ifndef TTEXTVIEWER_H
#define TTEXTVIEWER_H

#include "TText.h"

class TTextViewer {
protected:
    int maxDepth;  // максимальная глубина отображения (-1 = без ограничений)
    
    void PrintLevel(PTTextLink ptl, int currentDepth, int indent);

public:
    TTextViewer(int depth = -1) : maxDepth(depth) {}
    
    void SetDepth(int depth) { maxDepth = depth; }
    int  GetDepth() const { return maxDepth; }
    
    void View(const TText &text);  // отображение текста
};

#endif // TTEXTVIEWER_H