#ifndef TTEXTUSERCOM_H
#define TTEXTUSERCOM_H

#include "TText.h"
#include "TTextViewer.h"
#include <string>

class TTextUsercom
{
protected:
  TText text;
  TTextViewer viewer;
  std::string currentFile;
  bool running;

  void Help();
  void ParseAndExec(const std::string& cmd);
  void EnsureRoot();

public:
  TTextUsercom();
  void Clean();
  void Run();
};

#endif // TTEXTUSERCOM_H