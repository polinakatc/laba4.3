#include "gtest/gtest.h"
#include "core/TText.h"
#include "core/TTextLink.h"
#include <fstream>
#include <cstdio>

class TTextTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    static bool isMemInitialized = false;
    if (!isMemInitialized)
    {
      TTextLink::InitMemSystem(2000); // Увеличили пул до 2000 для массовых тестов
      isMemInitialized = true;
    }

    text = new TText(new TTextLink("Root"));
    emptyText = new TText(nullptr);
  }

  void TearDown() override
  {
    delete text;
    delete emptyText;
  }

  // Строит сложное дерево для тестов итератора и копирования
  void BuildComplexTree()
  {
    // [Root] (Section)
    //  |- [Sub 1] (Section)
    //  |   |- Atom 1.1
    //  |   |- Atom 1.2
    //  |- Atom 2
    //  |- [Sub 3] (Section)
    //      |- Atom 3.1
    text->InsDownSection("Sub 1");
    text->GoDownLink();
    text->InsDownLine("Atom 1.1");
    text->GoDownLink();
    text->InsNextLine("Atom 1.2");
    
    text->GoPrevLink(); // Возврат к Sub 1
    text->GoPrevLink(); // Возврат к Root
    
    text->GoDownLink(); // Переход к Sub 1
    text->InsNextLine("Atom 2");
    
    text->GoNextLink(); // Переход к Atom 2
    text->InsNextSection("Sub 3");
    
    text->GoNextLink(); // Переход к Sub 3
    text->InsDownLine("Atom 3.1");
    
    text->Reset(); // Сброс итератора после постройки
  }

  TText *text;
  TText *emptyText;
};

// ==================== 1. ТЕСТЫ ПУСТОГО ТЕКСТА (ГРАНИЧНЫЕ УСЛОВИЯ) ====================

TEST_F(TTextTest, EmptyText_Navigation_FailsGracefully)
{
  EXPECT_EQ(emptyText->GoFirstLink(), 0);
  EXPECT_EQ(emptyText->GoNextLink(), 0);
  EXPECT_EQ(emptyText->GoDownLink(), 0);
  EXPECT_EQ(emptyText->GoPrevLink(), 0);
}

TEST_F(TTextTest, EmptyText_Modifications_DoNotCrash)
{
  EXPECT_NO_THROW({
    emptyText->InsNextLine("Test");
    emptyText->InsDownSection("Test");
    emptyText->DelNextLine();
    emptyText->DelDownSection();
    emptyText->SetLine("Test");
  });
  EXPECT_EQ(emptyText->GetLine(), "");
}

TEST_F(TTextTest, EmptyText_Iterator_FailsGracefully)
{
  EXPECT_EQ(emptyText->Reset(), 0);
  EXPECT_TRUE(emptyText->IsTextEnded());
  EXPECT_EQ(emptyText->GoNext(), 0);
  EXPECT_EQ(emptyText->GetIteratorLine(), "");
}

// ==================== 2. ТЕСТЫ НАВИГАЦИИ ====================

TEST_F(TTextTest, GoPrevLink_FailsAtRoot)
{
  // Изначально мы в корне, нет истории шагов
  EXPECT_EQ(text->GoPrevLink(), 0);
}

TEST_F(TTextTest, DeepNavigation_WorksCorrectly)
{
  text->InsDownLine("Level 1");
  text->GoDownLink();
  text->InsDownLine("Level 2");
  text->GoDownLink();
  text->InsDownLine("Level 3");
  text->GoDownLink();

  EXPECT_EQ(text->GetLine(), "Level 3");
  
  // Поднимаемся обратно
  EXPECT_EQ(text->GoPrevLink(), 1);
  EXPECT_EQ(text->GetLine(), "Level 2");
  EXPECT_EQ(text->GoPrevLink(), 1);
  EXPECT_EQ(text->GetLine(), "Level 1");
  EXPECT_EQ(text->GoPrevLink(), 1);
  EXPECT_EQ(text->GetLine(), "Root");
}

TEST_F(TTextTest, GoFirstLink_FindsFirstAtom)
{
  BuildComplexTree();
  EXPECT_EQ(text->GoFirstLink(), 1);
  // Root и Sub 1 стали разделами, первый атом в дереве — Atom 1.1
  EXPECT_EQ(text->GetLine(), "Atom 1.1");
}

// ==================== 3. ТЕСТЫ ПРЕОБРАЗОВАНИЯ ТИПОВ УЗЛОВ ====================

TEST_F(TTextTest, InsDownLine_ConvertsAtomToSection)
{
  text->InsNextLine("Atom Node");
  text->GoNextLink();
  
  text->InsDownLine("Child Line"); // "Atom Node" теперь раздел
  
  EXPECT_EQ(text->GoDownLink(), 1);
  EXPECT_EQ(text->GetLine(), "Child Line");
}

TEST_F(TTextTest, DelDownLine_ConvertsSectionBackToAtom_IfEmpty)
{
  text->InsDownLine("Child Line"); // Root стал разделом
  text->DelDownLine(); // Root потерял единственный подуровень
  
  // Теперь у Root нет подуровней, он снова атом. Переход вниз невозможен.
  EXPECT_EQ(text->GoDownLink(), 0);
}

// ==================== 4. СЛОЖНЫЕ УДАЛЕНИЯ ====================

TEST_F(TTextTest, DelNextLine_MiddleNode)
{
  text->InsNextLine("Node 1");
  text->InsNextLine("Node 2");
  text->InsNextLine("Node 3");
  // Порядок: Root -> Node 3 -> Node 2 -> Node 1 (из-за вставки "после текущего")
  
  // Текущий - Root. Следующий - Node 3.
  text->DelNextLine(); // Удаляем Node 3
  
  EXPECT_EQ(text->GoNextLink(), 1);
  EXPECT_EQ(text->GetLine(), "Node 2"); // Проверяем, что Node 2 встала на место Node 3
}

TEST_F(TTextTest, DelNextSection_RemovesEntireSubtree)
{
  text->InsNextSection("Section");
  
  text->GoNextLink();
  text->InsDownLine("Sec Child 1");
  text->InsDownLine("Sec Child 2");
  
  text->GoPrevLink(); // Возврат в Root
  text->DelNextSection(); // Уничтожаем "Section" со всеми детьми
  
  EXPECT_EQ(text->GoNextLink(), 0); // Рядом с Root больше ничего нет
}

// ==================== 5. ТЕСТЫ ИТЕРАТОРА ====================

TEST_F(TTextTest, Iterator_SkipsSections_YieldsOnlyAtoms)
{
  text->InsDownSection("Section 1");
  text->GoDownLink();
  text->InsDownLine("Atom 1");
  
  text->Reset();
  EXPECT_FALSE(text->IsTextEnded());
  EXPECT_EQ(text->GetIteratorLine(), "Atom 1"); // Root и Section 1 пропущены
}

TEST_F(TTextTest, Iterator_ComplexTree_TDN_Order)
{
  BuildComplexTree();
  
  std::vector<std::string> expectedAtoms = {"Atom 1.1", "Atom 1.2", "Atom 2", "Atom 3.1"};
  std::vector<std::string> actualAtoms;
  
  while (!text->IsTextEnded()) {
    actualAtoms.push_back(text->GetIteratorLine());
    text->GoNext();
  }
  
  EXPECT_EQ(expectedAtoms, actualAtoms);
}

// ==================== 6. ТЕСТЫ КОПИРОВАНИЯ ====================

TEST_F(TTextTest, Copy_CreatesExactStructure)
{
  BuildComplexTree();
  TText copyText(text->GetCopy());
  
  copyText.Reset();
  text->Reset();
  
  while (!text->IsTextEnded()) {
    EXPECT_EQ(text->GetIteratorLine(), copyText.GetIteratorLine());
    text->GoNext();
    copyText.GoNext();
  }
  EXPECT_TRUE(copyText.IsTextEnded());
}

TEST_F(TTextTest, Copy_IsIndependent_ModificationsDoNotAffectOriginal)
{
  text->InsNextLine("Original Line");
  TText copyText(text->GetCopy());
  
  copyText.GoNextLink();
  copyText.SetLine("Modified Copy Line");
  
  text->GoNextLink();
  EXPECT_EQ(text->GetLine(), "Original Line"); // Оригинал не изменился
  EXPECT_EQ(copyText.GetLine(), "Modified Copy Line");
}

// ==================== 7. ТЕСТЫ ФАЙЛОВОЙ СИСТЕМЫ ====================

// TEST_F(TTextTest, ReadWrite_PreservesHierarchy)
// {
//   BuildComplexTree();
//   const char* filename = "hierarchy_test.txt";
  
//   text->Write(filename);
  
//   TText loadedText;
//   loadedText.Read(filename);
  
//   loadedText.Reset();
//   text->Reset();
  
//   while (!text->IsTextEnded()) {
//     EXPECT_EQ(text->GetIteratorLine(), loadedText.GetIteratorLine());
//     text->GoNext();
//     loadedText.GoNext();
//   }
  
//   std::remove(filename);
// }

// ==================== 8. СБОРЩИК МУСОРА (GARBAGE COLLECTOR) ====================

TEST_F(TTextTest, MemCleaner_DoesNotCorruptActiveTree)
{
  BuildComplexTree();
  
  EXPECT_NO_THROW({
    TTextLink::MemCleaner(*text);
  });
  
  // Убеждаемся, что дерево не пострадало после прохода очистителя
  EXPECT_EQ(text->Reset(), 1);
  EXPECT_EQ(text->GetIteratorLine(), "Atom 1.1");
}

TEST_F(TTextTest, MemCleaner_SafelyCollectsOrphanedNodes)
{
  text->InsNextSection("Orphaned Section");
  text->GoNextLink();
  text->InsDownLine("Orphaned Atom");
  text->GoPrevLink();
  
  text->DelNextSection(); // Удаляем раздел (он отправляется в свободный пул в DelNextSection)
  
  // Прогоняем GC (в твоей реализации delete сразу кладёт в pFree, 
  // но MemCleaner делает проход по всем массивам памяти)
  EXPECT_NO_THROW({
    TTextLink::MemCleaner(*text);
  });
}

// ==================== ТОЧКА ВХОДА ====================

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}