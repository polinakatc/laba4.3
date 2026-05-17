#include "TText.h"
#include <iostream>
#include <fstream>
#include <cstring>

// ========== Конструктор ==========
TText::TText(PTTextLink pl) : pFirst(pl), pCurrent(pl) {
    if (pFirst != nullptr) {
        Path.push(pFirst);
    }
}

// ========== GetFirstAtom ==========
PTTextLink TText::GetFirstAtom(PTTextLink p1) {
    if (p1 == nullptr) return nullptr;
    PTTextLink p = p1;
    while (p != nullptr && !p->IsAtom()) {
        p = p->GetDown();
    }
    return p;
}

// ========== PrintText (рекурсивная печать) ==========
void TText::PrintText(PTTextLink ptl) {
    PTTextLink p = ptl;
    while (p != nullptr) {
        if (!p->IsAtom()) {
            // Печатаем заголовок раздела
            std::cout << p->GetStr() << std::endl;
            // Рекурсивно печатаем подуровень
            PrintText(p->GetDown());
        } else {
            // Атом — просто строка
            std::cout << p->GetStr() << std::endl;
        }
        p = p->GetNext();
    }
}

// ========== Print ==========
void TText::Print() {
    if (pFirst == nullptr) {
        std::cout << "(текст пуст)" << std::endl;
        return;
    }
    PrintText(pFirst);
}

// ========== ReadText (чтение уровня из потока) ==========
// ========== ReadText (чтение уровня из потока) ==========
PTTextLink TText::ReadText(std::ifstream &TxtFile) {
    PTTextLink pFirstLocal = nullptr;
    PTTextLink pLast = nullptr;
    char buf[TextLineLength];

    while (TxtFile.getline(buf, TextLineLength)) {
        if (buf[0] == '\0') break;  // пустая строка — конец уровня

        PTTextLink pNew = new TTextLink(buf);

        // Если строка заканчивается на '{' — это раздел
        size_t len = std::strlen(buf);
        if (len > 0 && buf[len - 1] == '{') {
            // Читаем подуровень
            PTTextLink pDown = ReadText(TxtFile);
            pNew->SetDown(pDown);
        }

        if (pFirstLocal == nullptr) {
            pFirstLocal = pNew;
        } else {
            pLast->SetNext(pNew);
        }
        pLast = pNew;
    }
    return pFirstLocal;
}

// ========== Read (из файла) ==========
void TText::Read(const char* pFileName) {
    std::ifstream file(pFileName);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << pFileName << std::endl;
        return;
    }
    pFirst = nullptr;
    pCurrent = nullptr;
    while (!Path.empty()) Path.pop();
    while (!St.empty()) St.pop();

    pFirst = ReadText(file);
    pCurrent = pFirst;
    if (pCurrent != nullptr) {
        Path.push(pCurrent);
    }
    file.close();
}

// ========== WriteText (приватный статический метод или внешняя с геттерами) ==========
// Использую внешнюю функцию, но с GetNext()/GetDown()
static void WriteTextFunc(std::ofstream &file, PTTextLink ptl) {
    PTTextLink p = ptl;
    while (p != nullptr) {
        if (!p->IsAtom()) {
            file << p->GetStr() << std::endl;
            WriteTextFunc(file, p->GetDown());
        } else {
            file << p->GetStr() << std::endl;
        }
        p = p->GetNext();
    }
}

// ========== Write (в файл) ==========
void TText::Write(const char* pFileName) {
    std::ofstream file(pFileName);
    if (!file.is_open()) {
        std::cerr << "Ошибка создания файла: " << pFileName << std::endl;
        return;
    }
    WriteTextFunc(file, pFirst);
    file.close();
}

// ========== GoFirstLink ==========
int TText::GoFirstLink() {
    if (pFirst == nullptr) return 0;
    while (!Path.empty()) Path.pop();
    PTTextLink p = pFirst;
    Path.push(p);
    while (p != nullptr && !p->IsAtom()) {
        PTTextLink down = p->GetDown();
        if (down != nullptr) {
            p = down;
            Path.push(p);
        } else {
            break;
        }
    }
    pCurrent = p;
    return 1;
}

// ========== GoNextLink ==========
int TText::GoNextLink() {
    if (pCurrent == nullptr || pCurrent->GetNext() == nullptr) return 0;
    // Не pop! Просто идём вперёд и кладём в стек
    pCurrent = pCurrent->GetNext();
    Path.push(pCurrent);
    return 1;
}

// ========== GoDownLink ==========
int TText::GoDownLink() {
    if (pCurrent == nullptr) return 0;
    if (pCurrent->IsAtom()) return 0;
    PTTextLink down = pCurrent->GetDown();
    if (down == nullptr) return 0;
    Path.push(down);
    pCurrent = down;
    return 1;
}

// ========== GoPrevLink ==========
int TText::GoPrevLink() {
    if (Path.size() <= 1) return 0;
    Path.pop();                    // убираем текущее положение
    pCurrent = Path.top();         // возвращаемся к предыдущему
    return 1;
}
// ========== GetLine ==========
std::string TText::GetLine() {
    if (pCurrent == nullptr) return "";
    return std::string(pCurrent->GetStr());
}

// ========== SetLine ==========
void TText::SetLine(std::string s) {
    if (pCurrent == nullptr) return;
    pCurrent->SetStr(s.c_str());
}

// ========== CopyLink (рекурсивное копирование, через геттеры/сеттеры) ==========
static PTTextLink CopyLinkFunc(PTTextLink src) {
    if (src == nullptr) return nullptr;
    PTTextLink copy = new TTextLink(src->GetStr());
    if (src->GetNext() != nullptr) {
        copy->SetNext(CopyLinkFunc(src->GetNext()));
    }
    if (src->GetDown() != nullptr) {
        copy->SetDown(CopyLinkFunc(src->GetDown()));
    }
    return copy;
}

// ========== GetCopy ==========
PTTextLink TText::GetCopy() {
    return CopyLinkFunc(pFirst);
}

// ========== InsNextLine ==========
// Вставка строки (атома) после текущей на том же уровне
void TText::InsNextLine(std::string s) {
    if (pCurrent == nullptr) return;
    PTTextLink pNew = new TTextLink(s.c_str(), pCurrent->GetNext());
    pCurrent->SetNext(pNew);
}

// ========== InsDownLine ==========
// Вставка строки (атома) в подуровень текущей
// Если текущая — атом, она перестаёт быть атомом (становится разделом со старой строкой как заголовком)
// ========== InsDownLine ==========
void TText::InsDownLine(std::string s) {
    if (pCurrent == nullptr) return;
    PTTextLink pNew = new TTextLink(s.c_str());

    if (pCurrent->IsAtom()) {
        pCurrent->SetDown(pNew);
    } else {
        // Идём в конец подуровня
        PTTextLink p = pCurrent->GetDown();
        while (p->GetNext() != nullptr) {
            p = p->GetNext();
        }
        p->SetNext(pNew);
    }
}

// ========== InsNextSection ==========
// Вставка раздела после текущей на том же уровне
// Раздел — это звено с заголовком s и пустым подуровнем (в подуровне одна пустая строка-атом)
// ========== InsNextSection ==========
void TText::InsNextSection(std::string s) {
    if (pCurrent == nullptr) return;
    PTTextLink pNew = new TTextLink(s.c_str(), pCurrent->GetNext(), nullptr);
    pCurrent->SetNext(pNew);
}

// ========== InsDownSection ==========
// Вставка раздела в подуровень текущей
// ========== InsDownSection ==========
void TText::InsDownSection(std::string s) {
    if (pCurrent == nullptr) return;
    PTTextLink pNew = new TTextLink(s.c_str(), nullptr, nullptr);

    if (pCurrent->IsAtom()) {
        pCurrent->SetDown(pNew);
    } else {
        PTTextLink p = pCurrent->GetDown();
        if (p == nullptr) {
            pCurrent->SetDown(pNew);
        } else {
            while (p->GetNext() != nullptr) {
                p = p->GetNext();
            }
            p->SetNext(pNew);
        }
    }
}

// ========== DelNextLine ==========
// Удаление следующей строки (атома) на том же уровне
void TText::DelNextLine() {
    if (pCurrent == nullptr) return;
    PTTextLink pDel = pCurrent->GetNext();
    if (pDel == nullptr) return;
    if (!pDel->IsAtom()) return;  // удаляем только атомы

    pCurrent->SetNext(pDel->GetNext());
    delete pDel;
}

// ========== DelDownLine ==========
// Удаление строки в подуровне (первого атома)
void TText::DelDownLine() {
    if (pCurrent == nullptr) return;
    if (pCurrent->IsAtom()) return;  // у атома нет подуровня

    PTTextLink pDel = pCurrent->GetDown();
    if (pDel == nullptr) return;
    if (!pDel->IsAtom()) return;  // удаляем только атомы

    pCurrent->SetDown(pDel->GetNext());
    delete pDel;

    // Если подуровень опустел — превращаем раздел обратно в атом
    if (pCurrent->GetDown() == nullptr) {
        // Заголовок раздела становится строкой атома
        // (уже так и есть, просто подуровня больше нет)
    }
}

// ========== DelNextSection ==========
// Удаление следующего раздела (и всего его содержимого)
void TText::DelNextSection() {
    if (pCurrent == nullptr) return;
    PTTextLink pDel = pCurrent->GetNext();
    if (pDel == nullptr) return;
    if (pDel->IsAtom()) return;  // удаляем только разделы (не атомы)

    pCurrent->SetNext(pDel->GetNext());

    // Рекурсивно удаляем всё содержимое раздела
    DeleteSection(pDel);
}

// ========== DelDownSection ==========
// Удаление раздела в подуровне
void TText::DelDownSection() {
    if (pCurrent == nullptr) return;
    if (pCurrent->IsAtom()) return;

    PTTextLink pDel = pCurrent->GetDown();
    if (pDel == nullptr) return;
    if (pDel->IsAtom()) return;  // удаляем только разделы

    pCurrent->SetDown(pDel->GetNext());
    DeleteSection(pDel);

    // Если подуровень опустел — становимся атомом
    if (pCurrent->GetDown() == nullptr) {
        // всё, теперь IsAtom() == true
    }
}
// ========== DeleteSection (рекурсивное удаление содержимого раздела) ==========
void TText::DeleteSection(PTTextLink p) {
    if (p == nullptr) return;
    // Рекурсивно удаляем подуровень
    PTTextLink down = p->GetDown();
    while (down != nullptr) {
        PTTextLink next = down->GetNext();
        if (!down->IsAtom()) {
            DeleteSection(down);  // рекурсивно удаляем вложенные разделы
        }
        delete down;
        down = next;
    }
    // Удаляем само звено раздела
    delete p;
}

// ========== Итератор (обход TDN: Top → Down → Next) ==========

// Инициализация: установка на первый атом текста
int TText::Reset() {
    // Очищаем стек итератора
    while (!St.empty()) St.pop();
    
    if (pFirst == nullptr) return 0;
    
    // Начинаем с корня, спускаемся до первого атома
    PTTextLink p = pFirst;
    St.push(p);
    while (p != nullptr && !p->IsAtom()) {
        p = p->GetDown();
        St.push(p);
    }
    // pCurrent не меняем — итератор работает независимо от навигации
    return 1;
}

// Проверка завершения обхода
int TText::IsTextEnded() const {
    return St.empty() ? 1 : 0;
}

// Переход к следующему атому (схема TDN)
int TText::GoNext() {
    if (St.empty()) return 0;
    
    // Снимаем текущий атом со стека
    PTTextLink p = St.top();
    St.pop();
    
    // Пробуем пойти вправо (Next)
    if (p->GetNext() != nullptr) {
        p = p->GetNext();
        St.push(p);
        // Спускаемся до атома
        while (!p->IsAtom()) {
            p = p->GetDown();
            St.push(p);
        }
        return 1;
    }
    
    // Next нет — поднимаемся по стеку, пока не найдём уровень с Next
    while (!St.empty()) {
        p = St.top();
        St.pop();
        if (p->GetNext() != nullptr) {
            p = p->GetNext();
            St.push(p);
            while (!p->IsAtom()) {
                p = p->GetDown();
                St.push(p);
            }
            return 1;
        }
    }
    
    // Стек пуст — обход завершён
    return 0;
}

std::string TText::GetIteratorLine() const {
    if (St.empty()) return "";
    return std::string(St.top()->GetStr());
}