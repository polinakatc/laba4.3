#include "TTextUsercom.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <termios.h>

TTextUsercom::TTextUsercom()
  : running(false)
  , currentFile("")
{
}

void TTextUsercom::Clean()
{
  std::cout << "\033[2J\033[H" << std::flush;
  std::cout << "\n======================================\n";
}

void TTextUsercom::EnsureRoot()
{
  if (text.pFirst == nullptr)
  {
    PTTextLink root = new TTextLink("root");
    text = TText(root);
    text.GoFirstLink();
  }
}

void TTextUsercom::Help()
{
  std::cout << "\n=== Команды редактора ===\n";
  std::cout << "  open <file>      - загрузить текст из файла\n";
  std::cout << "  save <file>      - сохранить текст в файл\n";
  std::cout << "  print            - показать текст\n";
  std::cout << "  first            - перейти к первой строке\n";
  std::cout << "  next             - следующая строка\n";
  std::cout << "  prev             - предыдущая строка\n";
  std::cout << "  down             - перейти на подуровень\n";
  std::cout << "  show             - показать текущую строку\n";
  std::cout << "  set <текст>      - заменить текущую строку\n";
  std::cout << "  insnext <текст>  - вставить строку после текущей\n";
  std::cout << "  insdown <текст>  - вставить строку в подуровень\n";
  std::cout << "  insnexts <текст> - вставить раздел после текущего\n";
  std::cout << "  insdowns <текст> - вставить раздел в подуровень\n";
  std::cout << "  delnext          - удалить следующую строку\n";
  std::cout << "  delnexts         - удалить следующий раздел\n";
  std::cout << "  deldown          - удалить строку в подуровне\n";
  std::cout << "  deldowns         - удалить раздел в подуровне\n";
  std::cout << "  iter             - обход итератором\n";
  std::cout << "  depth <число>    - глубина просмотра (-1 = вся)\n";
  std::cout << "  clean            - очистить консоль\n";
  std::cout << "  help             - справка\n";
  std::cout << "  quit             - выход\n";
  std::cout << "================================\n\n";
}

void TTextUsercom::ParseAndExec(const std::string& cmd)
{
  Clean();
  std::istringstream iss(cmd);
  std::string op;
  iss >> op;

  if (op == "insdown" || op == "insdowns" || op == "insnext" || op == "insnexts" || op == "set" ||
      op == "first" || op == "next" || op == "prev" || op == "down" || op == "show" ||
      op == "delnext" || op == "delnexts" || op == "deldown" || op == "deldowns" || op == "print")
  {
    EnsureRoot();
  }

  if (op == "help")
    Help();
  else if (op == "clean" || op == "cls")
    Clean();
  else if (op == "quit" || op == "exit")
  {
    running = false;
    std::cout << "Выход.\n";
  }
  else if (op == "open")
  {
    std::string filename;
    iss >> filename;
    if (filename.empty())
    {
      std::cout << "Укажите имя файла: open <file>\n";
    }
    else
    {
      text.Read(filename.c_str());
      currentFile = filename;
      std::cout << "Загружено из " << filename << "\n";
    }
  }
  else if (op == "save")
  {
    std::string filename;
    iss >> filename;
    if (filename.empty() && !currentFile.empty())
      filename = currentFile;
    if (filename.empty())
      std::cout << "Укажите имя файла: save <file>\n";
    else
    {
      text.Write(filename.c_str());
      currentFile = filename;
      std::cout << "Сохранено в " << filename << "\n";
    }
  }
  else if (op == "print")
    viewer.View(text);
  else if (op == "first")
    if (text.GoFirstLink())
      std::cout << "OK\n";
    else
      std::cout << "Текст пуст\n";
  else if (op == "next")
    if (text.GoNextLink())
      std::cout << "OK: " << text.GetLine() << "\n";
    else
      std::cout << "Достигнут конец уровня\n";
  else if (op == "prev")
    if (text.GoPrevLink())
      std::cout << "OK: " << text.GetLine() << "\n";
    else
      std::cout << "Нет предыдущей позиции\n";
  else if (op == "down")
    if (text.GoDownLink())
      std::cout << "OK: " << text.GetLine() << "\n";
    else
      std::cout << "Нет подуровня или это атом\n";
  else if (op == "show")
    std::cout << "Текущая строка: [" << text.GetLine() << "]\n";
  else if (op == "set")
  {
    std::string line;
    std::getline(iss, line);
    if (!line.empty() && line[0] == ' ')
      line.erase(0, 1);
    text.SetLine(line);
    std::cout << "Заменено: " << line << "\n";
  }
  else if (op == "insnext")
  {
    std::string line;
    std::getline(iss, line);
    if (!line.empty() && line[0] == ' ')
      line.erase(0, 1);
    text.InsNextLine(line);
    std::cout << "Вставлено: " << line << "\n";
  }
  else if (op == "insdown")
  {
    std::string line;
    std::getline(iss, line);
    if (!line.empty() && line[0] == ' ')
      line.erase(0, 1);
    text.InsDownLine(line);
    std::cout << "Вставлено в подуровень: " << line << "\n";
  }
  else if (op == "insnexts")
  {
    std::string line;
    std::getline(iss, line);
    if (!line.empty() && line[0] == ' ')
      line.erase(0, 1);
    text.InsNextSection(line);
    std::cout << "Вставлен раздел: " << line << "\n";
  }
  else if (op == "insdowns")
  {
    std::string line;
    std::getline(iss, line);
    if (!line.empty() && line[0] == ' ')
      line.erase(0, 1);
    text.InsDownSection(line);
    std::cout << "Вставлен раздел в подуровень: " << line << "\n";
  }
  else if (op == "delnext")
  {
    text.DelNextLine();
    std::cout << "Удалена следующая строка\n";
  }
  else if (op == "delnexts")
  {
    text.DelNextSection();
    std::cout << "Удалён следующий раздел\n";
  }
  else if (op == "deldown")
  {
    text.DelDownLine();
    std::cout << "Удалена строка в подуровне\n";
  }
  else if (op == "deldowns")
  {
    text.DelDownSection();
    std::cout << "Удалён раздел в подуровне\n";
  }
  else if (op == "iter")
  {
    if (text.Reset())
    {
      int count = 0;
      while (!text.IsTextEnded())
      {
        ++count;
        std::cout << "  [" << count << "] " << text.GetIteratorLine() << "\n";
        text.GoNext();
      }
      std::cout << "Всего атомов: " << count << "\n";
    }
    else
      std::cout << "Текст пуст\n";
  }
  else if (op == "depth")
  {
    int d;
    iss >> d;
    viewer.SetDepth(d);
    std::cout << "Глубина просмотра: " << d << "\n";
  }
  else if (op.empty() || op[0] == '#')
  {
  }
  else
    std::cout << "Неизвестная команда: " << op << " (help — справка)\n";
}

void TTextUsercom::Run()
{
  running = true;
  Clean();
  std::cout << "=== Редактор иерархических текстов ===\n";
  std::cout << "Введите help для списка команд.\n\n";

  std::string line;
  while (running)
  {
    std::cout << "> " << std::flush;
    line = ReadCommandWithHistory();
    ParseAndExec(line);
  }
}

std::string TTextUsercom::ReadCommandWithHistory()
{
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO); // Отключаем буферизацию и авто-вывод символов
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  std::string currentInput = "";
  int historyIndex = history.size();

  while (true)
  {
    int c = getchar();
    if (c == EOF)
      break;

    if (c == '\n' || c == '\r') // Нажат Enter
    {
      std::cout << '\n';
      break;
    }
    else if (c == 127 || c == 8) // Нажат Backspace
    {
      if (!currentInput.empty())
      {
        currentInput.pop_back();
        // Затираем символ на экране (\b шаг назад, пробел стирает, \b шаг назад)
        std::cout << "\b \b"; 
      }
    }
    else if (c == 27) // Нажат Escape (начало спец-кода стрелочек)
    {
      c = getchar();
      if (c == 91) // '['
      {
        c = getchar();
        if (c == 65) // 'A' -> Стрелка ВВЕРХ
        {
          if (historyIndex > 0)
          {
            historyIndex--;
            currentInput = history[historyIndex];
            // \r - в начало строки, \033[2K - очистить всю строку, печатаем "> " и команду
            std::cout << "\r\033[2K> " << currentInput;
          }
        }
        else if (c == 66)
        {
          if (historyIndex < (int)history.size() - 1)
          {
            historyIndex++;
            currentInput = history[historyIndex];
            std::cout << "\r\033[2K> " << currentInput;
          }
          else if (historyIndex == (int)history.size() - 1)
          {
            historyIndex++;
            currentInput = "";
            std::cout << "\r\033[2K> ";
          }
        }
      }
    }
    else if (c >= 32 && c <= 126) 
    {
      currentInput += (char)c;
      std::cout << (char)c;
    }
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  if (!currentInput.empty() && (history.empty() || history.back() != currentInput))  
    history.push_back(currentInput);

  return currentInput;
}