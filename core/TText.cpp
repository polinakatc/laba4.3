#include "TText.h"
#include <cstring>
#include <fstream>
#include <iostream>

TText::TText(PTTextLink pl)
  : pFirst(pl)
  , pCurrent(pl)
{
  if (pFirst != nullptr)
    Path.push(pFirst);
}

PTTextLink TText::GetFirstAtom(PTTextLink p1)
{
  if (p1 == nullptr)
    return nullptr;
  PTTextLink p = p1;
  while (p != nullptr && !p->IsAtom())
    p = p->GetDown();
  return p;
}

void TText::PrintText(PTTextLink ptl)
{
  PTTextLink p = ptl;
  while (p != nullptr)
  {
    if (!p->IsAtom())
    {
      std::cout << p->GetStr() << std::endl;
      PrintText(p->GetDown());
    }
    else
      std::cout << p->GetStr() << std::endl;
    p = p->GetNext();
  }
}

void TText::Print()
{
  if (pFirst == nullptr)
  {
    std::cout << "(текст пуст)" << std::endl;
    return;
  }
  PrintText(pFirst);
}

PTTextLink TText::ReadText(std::ifstream& TxtFile)
{
  PTTextLink pFirstLocal = nullptr;
  PTTextLink pLast = nullptr;
  char buf[TextLineLength];

  while (TxtFile.getline(buf, TextLineLength))
  {
    if (buf[0] == '\0')
      break;

    PTTextLink pNew = new TTextLink(buf);

    // строка заканчивается на '{' — это раздел
    size_t len = std::strlen(buf);
    if (len > 0 && buf[len - 1] == '{')
    {
      PTTextLink pDown = ReadText(TxtFile);
      pNew->SetDown(pDown);
    }

    if (pFirstLocal == nullptr)
      pFirstLocal = pNew;
    else
      pLast->SetNext(pNew);
    pLast = pNew;
  }
  return pFirstLocal;
}

void TText::Read(const char* pFileName)
{
  std::ifstream file(pFileName);
  if (!file.is_open())
  {
    std::cerr << "Ошибка открытия файла: " << pFileName << std::endl;
    return;
  }
  pFirst = nullptr;
  pCurrent = nullptr;
  while (!Path.empty())
    Path.pop();
  while (!St.empty())
    St.pop();

  pFirst = ReadText(file);
  pCurrent = pFirst;
  if (pCurrent != nullptr)
    Path.push(pCurrent);
  file.close();
}

static void WriteTextFunc(std::ofstream& file, PTTextLink ptl)
{
  PTTextLink p = ptl;
  while (p != nullptr)
  {
    if (!p->IsAtom())
    {
      file << p->GetStr() << std::endl;
      WriteTextFunc(file, p->GetDown());
    }
    else
      file << p->GetStr() << std::endl;
    p = p->GetNext();
  }
}

void TText::Write(const char* pFileName)
{
  std::ofstream file(pFileName);
  if (!file.is_open())
  {
    std::cerr << "Ошибка создания файла: " << pFileName << std::endl;
    return;
  }
  WriteTextFunc(file, pFirst);
  file.close();
}

int TText::GoFirstLink()
{
  if (pFirst == nullptr)
    return 0;
  while (!Path.empty())
    Path.pop();
  PTTextLink p = pFirst;
  Path.push(p);
  while (p != nullptr && !p->IsAtom())
  {
    PTTextLink down = p->GetDown();
    if (down != nullptr)
    {
      p = down;
      Path.push(p);
    }
    else
      break;
  }
  pCurrent = p;
  return 1;
}

int TText::GoNextLink()
{
  if (pCurrent == nullptr || pCurrent->GetNext() == nullptr)
    return 0;
  pCurrent = pCurrent->GetNext();
  Path.push(pCurrent);
  return 1;
}

int TText::GoDownLink()
{
  if (pCurrent == nullptr)
    return 0;
  if (pCurrent->IsAtom())
    return 0;
  PTTextLink down = pCurrent->GetDown();
  if (down == nullptr)
    return 0;
  Path.push(down);
  pCurrent = down;
  return 1;
}

int TText::GoPrevLink()
{
  if (Path.size() <= 1)
    return 0;
  Path.pop();
  pCurrent = Path.top();
  return 1;
}

std::string TText::GetLine()
{
  if (pCurrent == nullptr)
    return "";
  return std::string(pCurrent->GetStr());
}

void TText::SetLine(std::string s)
{
  if (pCurrent == nullptr)
    return;
  pCurrent->SetStr(s.c_str());
}

static PTTextLink CopyLinkFunc(PTTextLink src)
{
  if (src == nullptr)
    return nullptr;
  PTTextLink copy = new TTextLink(src->GetStr());
  if (src->GetNext() != nullptr)
    copy->SetNext(CopyLinkFunc(src->GetNext()));
  if (src->GetDown() != nullptr)
    copy->SetDown(CopyLinkFunc(src->GetDown()));
  return copy;
}

PTTextLink TText::GetCopy()
{
  return CopyLinkFunc(pFirst);
}

void TText::InsNextLine(std::string s)
{
  if (pCurrent == nullptr)
    return;
  PTTextLink pNew = new TTextLink(s.c_str(), pCurrent->GetNext());
  pCurrent->SetNext(pNew);
}

// если текущая — атом, она перестаёт быть атомом (становится разделом со старой строкой как
// заголовком)
void TText::InsDownLine(std::string s)
{
  if (pCurrent == nullptr)
    return;
  PTTextLink pNew = new TTextLink(s.c_str());

  if (pCurrent->IsAtom())
    pCurrent->SetDown(pNew);
  else
  {
    // идём в конец подуровня
    PTTextLink p = pCurrent->GetDown();
    while (p->GetNext() != nullptr)
      p = p->GetNext();
    p->SetNext(pNew);
  }
}

// Раздел — это звено с заголовком s и пустым подуровнем (в подуровне одна пустая строка-атом)
void TText::InsNextSection(std::string s)
{
  if (pCurrent == nullptr)
    return;
  PTTextLink pNew = new TTextLink(s.c_str(), pCurrent->GetNext(), nullptr);
  pCurrent->SetNext(pNew);
}

// вставка раздела в подуровень текущей
void TText::InsDownSection(std::string s)
{
  if (pCurrent == nullptr)
    return;
  PTTextLink pNew = new TTextLink(s.c_str(), nullptr, nullptr);

  if (pCurrent->IsAtom())
    pCurrent->SetDown(pNew);
  else
  {
    PTTextLink p = pCurrent->GetDown();
    if (p == nullptr)
      pCurrent->SetDown(pNew);
    else
    {
      while (p->GetNext() != nullptr)
        p = p->GetNext();
      p->SetNext(pNew);
    }
  }
}

// удаление следующей строки (атома) на том же уровне
void TText::DelNextLine()
{
  if (pCurrent == nullptr)
    return;
  PTTextLink pDel = pCurrent->GetNext();
  if (pDel == nullptr)
    return;
  if (!pDel->IsAtom())
    return;
  pCurrent->SetNext(pDel->GetNext());
  delete pDel;
}

// удаление строки в подуровне (первого атома)
void TText::DelDownLine()
{
  if (pCurrent == nullptr)
    return;
  if (pCurrent->IsAtom())
    return;

  PTTextLink pDel = pCurrent->GetDown();
  if (pDel == nullptr)
    return;
  if (!pDel->IsAtom())
    return;

  pCurrent->SetDown(pDel->GetNext());
  delete pDel;

  if (pCurrent->GetDown() == nullptr)
  {
  }
}

// удаление следующего раздела (и всего его содержимого)
void TText::DelNextSection()
{
  if (pCurrent == nullptr)
    return;
  PTTextLink pDel = pCurrent->GetNext();
  if (pDel == nullptr)
    return;
  if (pDel->IsAtom())
    return; // удаляем только разделы (не атомы)

  pCurrent->SetNext(pDel->GetNext());

  DeleteSection(pDel);
}

// Удаление раздела в подуровне
void TText::DelDownSection()
{
  if (pCurrent == nullptr)
    return;
  if (pCurrent->IsAtom())
    return;

  PTTextLink pDel = pCurrent->GetDown();
  if (pDel == nullptr)
    return;
  if (pDel->IsAtom())
    return; // удаляем только разделы

  pCurrent->SetDown(pDel->GetNext());
  DeleteSection(pDel);

  // Если подуровень опустел — становимся атомом
  if (pCurrent->GetDown() == nullptr)
  {
  }
}
// удаление содержимого раздела
void TText::DeleteSection(PTTextLink p)
{
  if (p == nullptr)
    return;
  // Рекурсивно удаляем подуровень
  PTTextLink down = p->GetDown();
  while (down != nullptr)
  {
    PTTextLink next = down->GetNext();
    if (!down->IsAtom())
      DeleteSection(down);
    delete down;
    down = next;
  }

  delete p;
}

// итератор (обход TDN: Top → Down → Next)

// инициализация: установка на первый атом текста
int TText::Reset()
{
  while (!St.empty())
    St.pop();

  if (pFirst == nullptr)
    return 0;

  PTTextLink p = pFirst;
  St.push(p);
  while (p != nullptr && !p->IsAtom())
  {
    p = p->GetDown();
    St.push(p);
  }
  return 1;
}

// проверка завершения обхода
int TText::IsTextEnded() const
{
  return St.empty() ? 1 : 0;
}

// переход к следующему атому (схема TDN)
int TText::GoNext()
{
  if (St.empty())
    return 0;

  PTTextLink p = St.top();
  St.pop();

  // вправо (Next)
  if (p->GetNext() != nullptr)
  {
    p = p->GetNext();
    St.push(p);

    while (!p->IsAtom())
    {
      p = p->GetDown();
      St.push(p);
    }
    return 1;
  }

  // Next нет — поднимаемся по стеку, пока не найдём Next
  while (!St.empty())
  {
    p = St.top();
    St.pop();
    if (p->GetNext() != nullptr)
    {
      p = p->GetNext();
      St.push(p);
      while (!p->IsAtom())
      {
        p = p->GetDown();
        St.push(p);
      }
      return 1;
    }
  }

  return 0;
}

std::string TText::GetIteratorLine() const
{
  if (St.empty())
    return "";
  return std::string(St.top()->GetStr());
}