#include "core/TText.h"
#include <iostream>

int main() {
    TTextLink::InitMemSystem(200);

    TTextLink *root = new TTextLink("Программа", nullptr, nullptr);
    TText text(root);

    text.GoFirstLink();
    text.InsDownLine("#include <iostream>");
    text.InsDownLine("int main() {");
    text.InsDownLine("    return 0;");
    text.InsDownLine("}");

    std::cout << "=== Исходный текст ===" << std::endl;
    text.Print();

    // Идём к строке "    return 0;" и вставляем раздел после неё
    text.GoFirstLink();          // Программа
    text.GoDownLink();           // #include <iostream>
    text.GoNextLink();           // int main() {
    text.GoNextLink();           //     return 0;
    text.InsNextSection("void foo() {");

    // Переходим в раздел
    text.GoNextLink();           // void foo() {
    text.InsDownLine("    int x = 5;");
    text.InsDownLine("    x = x + 1;");

    // Возвращаемся к "    return 0;" и вставляем вызов foo()
    text.GoPrevLink();           //     return 0;
    text.InsNextLine("    foo();");

    std::cout << "\n=== После добавления раздела ===" << std::endl;
    text.Print();

    text.Write("output.txt");
    std::cout << "\nЗаписано в output.txt" << std::endl;

    TText text2;
    text2.Read("output.txt");
    std::cout << "\n=== Прочитано из файла ===" << std::endl;
    text2.Print();

    return 0;
}
