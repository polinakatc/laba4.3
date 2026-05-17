#ifndef TTEXTLINK_H
#define TTEXTLINK_H

#include <cstring>
#include <iostream>

const int TextLineLength = 256;
const int MemSize = 1000;

typedef char TStr[TextLineLength];

class TTextLink;
typedef TTextLink* PTTextLink;

class TTextMem
{
public:
  PTTextLink pFirst;
  PTTextLink pLast;
  PTTextLink pFree;
  TTextMem()
    : pFirst(nullptr)
    , pLast(nullptr)
    , pFree(nullptr)
  {
  }
};

class TTextLink
{
protected:
  TStr Str;
  PTTextLink pNext;
  PTTextLink pDown;
  static TTextMem MemHeader;

public:
  static void InitMemSystem(int size = MemSize);
  static void PrintFreeLink();
  static void MemCleaner(class TText& txt);

  void* operator new(size_t size);
  void operator delete(void* pM);

  TTextLink(const char* s = nullptr, PTTextLink pn = nullptr, PTTextLink pd = nullptr);
  ~TTextLink() {}

  void Clear();

  bool IsAtom() const { return pDown == nullptr; }
  PTTextLink GetNext() const { return pNext; }
  PTTextLink GetDown() const { return pDown; }
  void SetNext(PTTextLink pn) { pNext = pn; }
  void SetDown(PTTextLink pd) { pDown = pd; }
  const char* GetStr() const { return Str; }
  void SetStr(const char* s);

  friend std::ostream& operator<<(std::ostream& os, const TTextLink& link);
  friend class TText;
};

#endif