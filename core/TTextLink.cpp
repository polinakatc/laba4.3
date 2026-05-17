#include "TTextLink.h"
#include "TText.h"       
#include <stack>         
#include <cstring>
#include <iostream>

TTextMem TTextLink::MemHeader;

void TTextLink::Clear() 
{
    Str[0] = '\0';
    pNext = nullptr;
    pDown = nullptr;
}

void TTextLink::InitMemSystem(int size) 
{
    MemHeader.pFirst = reinterpret_cast<PTTextLink>(
        new char[size * sizeof(TTextLink)]
    );
    MemHeader.pLast = MemHeader.pFirst + size;

    MemHeader.pFree = MemHeader.pFirst;
    PTTextLink current = MemHeader.pFree;
    for (int i = 0; i < size - 1; ++i) 
    {
        current->Clear();
        current->pNext = current + 1;
        current = current + 1;
    }
    current->Clear();
    current->pNext = nullptr;
}

void TTextLink::PrintFreeLink() 
{
    std::cout << "Free links: ";
    PTTextLink p = MemHeader.pFree;
    int count = 0;
    while (p != nullptr) 
    {
        ++count;
        p = p->pNext;
    }
    std::cout << count << std::endl;
}

void* TTextLink::operator new(size_t size) 
{
    if (MemHeader.pFree == nullptr) 
    {
        throw std::bad_alloc();
    }
    PTTextLink p = MemHeader.pFree;
    MemHeader.pFree = p->pNext;
    return p;
}

void TTextLink::operator delete(void *pM) 
{
    if (pM == nullptr) return;
    PTTextLink p = static_cast<PTTextLink>(pM);
    p->pNext = MemHeader.pFree;
    MemHeader.pFree = p;
}

TTextLink::TTextLink(const char* s, PTTextLink pn, PTTextLink pd)
    : pNext(pn), pDown(pd)
{
    if (s != nullptr)
        std::strncpy(Str, s, TextLineLength - 1);
    else
        Str[0] = '\0';
}

void TTextLink::SetStr(const char* s) 
{
    if (s != nullptr)
        std::strncpy(Str, s, TextLineLength - 1);
    else
        Str[0] = '\0';
}

std::ostream& operator<<(std::ostream &os, const TTextLink &link) {
    os << link.Str;
    return os;
}

void TTextLink::MemCleaner(TText &txt) 
{
    if (MemHeader.pFirst == nullptr) return;
    
    int totalLinks = MemHeader.pLast - MemHeader.pFirst;
    
    bool *marked = new bool[totalLinks]();
    
    struct Helper 
    {
        static void MarkAll(PTTextLink p, PTTextLink first, bool *m, int total) 
        {
            if (p == nullptr) return;
            int idx = p - first;
            if (idx >= 0 && idx < total) m[idx] = true;
            MarkAll(p->GetDown(), first, m, total);
            MarkAll(p->GetNext(), first, m, total);
        }
    };
    Helper::MarkAll(txt.pFirst, MemHeader.pFirst, marked, totalLinks);
    
    PTTextLink freeLink = MemHeader.pFree;
    while (freeLink != nullptr) 
    {
        int index = freeLink - MemHeader.pFirst;
        if (index >= 0 && index < totalLinks) 
        {
            marked[index] = true;
        }
        freeLink = freeLink->pNext;
    }
    
    for (int i = 0; i < totalLinks; ++i) 
    {
        if (!marked[i]) 
        {
            PTTextLink p = MemHeader.pFirst + i;
            p->Clear();
            p->pNext = MemHeader.pFree;
            MemHeader.pFree = p;
        }
    }
    
    delete[] marked;
}