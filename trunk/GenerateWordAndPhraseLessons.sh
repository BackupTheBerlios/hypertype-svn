#!/bin/bash

##### Capital letters #####
grep "^[hjkl;][asdfghjklertuiopnm]\{0,8\}$" master.txt > src/data/lesson_data/WordsCommonKeysWithHJKLCaps.txt
./CapitalizeLines.pl src/data/lesson_data/WordsCommonKeysWithHJKLCaps.txt

grep "^[uiop][asdfghjklertuiopnm]\{0,8\}$" master.txt > src/data/lesson_data/WordsCommonKeysWithUIOPCaps.txt
./CapitalizeLines.pl src/data/lesson_data/WordsCommonKeysWithUIOPCaps.txt

grep "^[asdfg][asdfghjklertuiopnm]\{0,8\}$" master.txt > src/data/lesson_data/WordsCommonKeysWithASDFGCaps.txt
./CapitalizeLines.pl src/data/lesson_data/WordsCommonKeysWithASDFGCaps.txt

grep "^[ert][asdfghjklertuiopnm]\{0,8\}$" master.txt > src/data/lesson_data/WordsCommonKeysWithERTCaps.txt
./CapitalizeLines.pl src/data/lesson_data/WordsCommonKeysWithERTCaps.txt

grep "^[nm][asdfghjklertuiopnm]\{0,8\}$" master.txt > src/data/lesson_data/WordsCommonKeysWithNMCaps.txt
./CapitalizeLines.pl src/data/lesson_data/WordsCommonKeysWithNMCaps.txt

##### Capital letters practice #####
grep "^[asdfghjklertuiopnm]\{0,8\}$" master.txt > src/data/lesson_data/WordsCommonKeysWithAllCaps.txt
./CapitalizeLines.pl src/data/lesson_data/WordsCommonKeysWithAllCaps.txt

cp src/data/lesson_data/PhrasesHomeRowPlusCommonKeys.txt src/data/lesson_data/PhrasesCommonKeysWithAllCaps.txt
./CapitalizeLines.pl src/data/lesson_data/PhrasesCommonKeysWithAllCaps.txt


##### Remaining keys #####
grep "^[asdfghjklertuiopnmyb]\{0,8\}$" master.txt | grep "[yb]" > src/data/lesson_data/WordsCumPlusYB.txt
grep "^[asdfghjklertuiopnmybqw]\{0,8\}$" master.txt | grep "[qw]" > src/data/lesson_data/WordsCumPlusQW.txt
grep "^[asdfghjklertuiopnmybqwzxcv]\{0,8\}$" master.txt | grep "[zxcv]" > src/data/lesson_data/WordsCumPlusZXCV.txt
grep "^[ybqwzxcv].\{0,8\}$" master.txt > src/data/lesson_data/WordsCumPlusRemainingCaps.txt
./CapitalizeLines.pl src/data/lesson_data/WordsCumPlusRemainingCaps.txt

##### Remaining keys Practice #####
grep "^[asdfghjklertuiopnmybqwzxcv]\{0,8\}$" master.txt | grep "[ybqwzxcv]" > src/data/lesson_data/WordsCumPlusYBQWZXCV.txt

##### Books #####
# War of the Worlds
./extract_words.pl src/data/source_data/WarOfTheWorlds.txt "abcdefghijklmnopqrstuvwxyz;:.," > src/data/lesson_data/PhrasesAllLetters_WotW.txt
./CapitalizeLines.pl src/data/lesson_data/PhrasesAllLetters_WotW.txt
# Treasure Island
./extract_words.pl src/data/source_data/TreasureIsland.txt "abcdefghijklmnopqrstuvwxyz;:.," > src/data/lesson_data/PhrasesAllLetters_TreasureIsland.txt
./CapitalizeLines.pl src/data/lesson_data/PhrasesAllLetters_TreasureIsland.txt

##### Games #####
grep "^[asdfghjklertuiopnm]\{0,5\}$" master.txt > src/data/lesson_data/GameWordsCommonKeys.txt # End of common keys lesson set
grep "^[asdfghjklertuiopnm]\{0,5\}$" master.txt > src/data/lesson_data/GameWordsModifierKeys.txt # End of modifier keys lesson set
./CapitalizeLines.pl src/data/lesson_data/GameWordsModifierKeys.txt
grep "^[abcdefghijklmnopqrstuvwxyz]\{0,5\}$" master.txt | grep "[ybqwzxcv]"  > src/data/lesson_data/GameWordsRemainingLetters.txt # End of remaining keys lesson set

##### Numbers #####
cat src/data/source_data/NumberPhrases.txt | sed 's/+/plus/g' | sed 's/=/equals/g' | sed 's/-/minus/g' | sed 's/+\n/\n/g' | tr -d '[()]' > src/data/lesson_data/AllNumberPhrases.txt

##### Books #####
./extract_book_phrases.pl src/data/source_data/RansomOfRedChief.txt > src/data/lesson_data/BookRansomOfRedChief.txt
./extract_book_phrases.pl src/data/source_data/AfterTwentyYears.txt > src/data/lesson_data/BookAfterTwentyYears.txt
./extract_book_phrases.pl src/data/source_data/TheCopAndTheAnthem.txt > src/data/lesson_data/BookTheCopAndTheAnthem.txt

