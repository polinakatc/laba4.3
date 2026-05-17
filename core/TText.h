#ifndef TTEXT_H
#define TTEXT_H

#include <stack>
#include <string>
#include <fstream>
#include "TTextLink.h"

class TText {
protected:
    PTTextLink pFirst;                 // указатель корня дерева
    PTTextLink pCurrent;               // указатель текущей строки
    std::stack<PTTextLink> Path;       // стек траектории движения по тексту
    std::stack<PTTextLink> St;         // стек для итератора

    PTTextLink GetFirstAtom(PTTextLink p1);           // поиск первого атома
    void PrintText(PTTextLink ptl);                   // печать текста со звена ptl
    PTTextLink ReadText(std::ifstream &TxtFile);      // чтение текста из файла
    void DeleteSection(PTTextLink p);  // рекурсивное удаление раздела

    friend class TTextLink;
    friend class TTextViewer;
    friend class TTextUsercom;

public:
    TText(PTTextLink pl = nullptr);
    ~TText() { pFirst = nullptr; }

    PTTextLink GetCopy();          // копирование текста (возвращает корень копии)

    // Навигация
    int GoFirstLink();             // переход к первой строке
    int GoDownLink();              // переход к следующей строке по Down
    int GoNextLink();              // переход к следующей строке по Next
    int GoPrevLink();              // переход к предыдущей позиции в тексте

    // Доступ
    std::string GetLine();         // чтение текущей строки
    void SetLine(std::string s);   // замена текущей строки

    // Модификация
    void InsDownLine(std::string s);      // вставка строки в подуровень
    void InsDownSection(std::string s);   // вставка раздела в подуровень
    void InsNextLine(std::string s);      // вставка строки в том же уровне
    void InsNextSection(std::string s);   // вставка раздела в том же уровне
    void DelDownLine();                   // удаление строки в подуровне
    void DelDownSection();                // удаление раздела в подуровне
    void DelNextLine();                   // удаление строки в том же уровне
    void DelNextSection();                // удаление раздела в том же уровне

    // Итератор
    int Reset();                   // установить на первую запись
    int IsTextEnded() const;       // текст завершен?
    int GoNext();                  // переход к следующей записи
    std::string GetIteratorLine() const;  // получить строку текущего атома итератора

    // Работа с файлами
    void Read(const char* pFileName);    // ввод текста из файла
    void Write(const char* pFileName);   // вывод текста в файл

    // Печать
    void Print();                  // печать текста
};

#endif // TTEXT_H