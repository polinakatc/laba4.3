#include "TTextViewer.h"
#include <iostream>

void TTextViewer::PrintLevel(PTTextLink ptl, int currentDepth, int indent)
{
  PTTextLink p = ptl;
  while (p != nullptr)
  {
    for (int i = 0; i < indent; ++i)
      std::cout << "  ";

    if (!p->IsAtom())
    {
      std::cout << p->GetStr() << std::endl;
      if (maxDepth == -1 || currentDepth < maxDepth)
      {
        PrintLevel(p->GetDown(), currentDepth + 1, indent + 1);
      }
      else if (p->GetDown() != nullptr)
      {
        for (int i = 0; i < indent + 1; ++i)
          std::cout << "  ";
        std::cout << "..." << std::endl;
      }
    }
    else
    {
      std::cout << p->GetStr() << std::endl;
    }
    p = p->GetNext();
  }
}

void TTextViewer::View(const TText& text)
{
  if (text.pFirst == nullptr)
  {
    std::cout << "(текст пуст)" << std::endl;
    return;
  }
  PrintLevel(text.pFirst, 0, 0);
}