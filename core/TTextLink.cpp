#include "TTextLink.h"
#include "TText.h"       // <-- добавить (полное определение TText)
#include <stack>         // <-- добавить
#include <cstring>
#include <iostream>

// ---------- Статический член ----------
TTextMem TTextLink::MemHeader;

// ---------- Очистка звена ----------
void TTextLink::Clear() {
    Str[0] = '\0';
    pNext = nullptr;
    pDown = nullptr;
}

// ---------- Инициализация пула памяти ----------
void TTextLink::InitMemSystem(int size) {
    // Выделяем непрерывный блок памяти под size звеньев
    MemHeader.pFirst = reinterpret_cast<PTTextLink>(
        new char[size * sizeof(TTextLink)]
    );
    MemHeader.pLast = MemHeader.pFirst + size;

    // Связываем все звенья в список свободных
    MemHeader.pFree = MemHeader.pFirst;
    PTTextLink current = MemHeader.pFree;
    for (int i = 0; i < size - 1; ++i) {
        current->Clear();
        current->pNext = current + 1;
        current = current + 1;
    }
    current->Clear();
    current->pNext = nullptr;
}

// ---------- Печать свободных звеньев (для отладки) ----------
void TTextLink::PrintFreeLink() {
    std::cout << "Free links: ";
    PTTextLink p = MemHeader.pFree;
    int count = 0;
    while (p != nullptr) {
        ++count;
        p = p->pNext;
    }
    std::cout << count << std::endl;
}

// ---------- Перегрузка new ----------
void* TTextLink::operator new(size_t size) {
    if (MemHeader.pFree == nullptr) {
        throw std::bad_alloc();
    }
    PTTextLink p = MemHeader.pFree;
    MemHeader.pFree = p->pNext;
    return p;
}

// ---------- Перегрузка delete ----------
void TTextLink::operator delete(void *pM) {
    if (pM == nullptr) return;
    PTTextLink p = static_cast<PTTextLink>(pM);
    p->pNext = MemHeader.pFree;
    MemHeader.pFree = p;
}

// ---------- Конструктор ----------
TTextLink::TTextLink(const char* s, PTTextLink pn, PTTextLink pd)
    : pNext(pn), pDown(pd)
{
    if (s != nullptr)
        std::strncpy(Str, s, TextLineLength - 1);
    else
        Str[0] = '\0';
}

// ---------- Установка строки ----------
void TTextLink::SetStr(const char* s) {
    if (s != nullptr)
        std::strncpy(Str, s, TextLineLength - 1);
    else
        Str[0] = '\0';
}

// ---------- Оператор вывода ----------
std::ostream& operator<<(std::ostream &os, const TTextLink &link) {
    os << link.Str;
    return os;
}

// ========== Сборка мусора ==========
void TTextLink::MemCleaner(TText &txt) {
    if (MemHeader.pFirst == nullptr) return;
    
    int totalLinks = MemHeader.pLast - MemHeader.pFirst;
    
    // Массив флагов маркировки
    bool *marked = new bool[totalLinks]();
    
    // ---------- Этап 1: рекурсивная маркировка всех используемых звеньев ----------
    struct Helper {
        static void MarkAll(PTTextLink p, PTTextLink first, bool *m, int total) {
            if (p == nullptr) return;
            int idx = p - first;
            if (idx >= 0 && idx < total) m[idx] = true;
            MarkAll(p->GetDown(), first, m, total);
            MarkAll(p->GetNext(), first, m, total);
        }
    };
    Helper::MarkAll(txt.pFirst, MemHeader.pFirst, marked, totalLinks);
    
    // ---------- Этап 2: маркировка уже свободных звеньев ----------
    PTTextLink freeLink = MemHeader.pFree;
    while (freeLink != nullptr) {
        int index = freeLink - MemHeader.pFirst;
        if (index >= 0 && index < totalLinks) {
            marked[index] = true;
        }
        freeLink = freeLink->pNext;
    }
    
    // ---------- Этап 3: возврат неотмеченных в список свободных ----------
    for (int i = 0; i < totalLinks; ++i) {
        if (!marked[i]) {
            PTTextLink p = MemHeader.pFirst + i;
            p->Clear();
            p->pNext = MemHeader.pFree;
            MemHeader.pFree = p;
        }
    }
    
    delete[] marked;
}