#!/usr/bin/env python

import os
import templating
import math
import shutil
import re
import sys

# globals
(workingDir, ignored) = os.path.split(sys.argv[0])
booksDir = os.path.join(workingDir, "books_source")
destDir = os.path.join(workingDir, "../lesson_data/books/")
bookTemplate = os.path.join(workingDir, "BookTemplate.xml")
menuFile = os.path.join(workingDir, "../screens/practice_menu.xml")


def splitBookTextIntoLinesFile(srcFile, destFile):
   bookText = open(srcFile, "r").read()
   bookText = re.compile("[\s]+", re.S).sub(" ", bookText)
   bookText = bookText.replace(". ", ".\n")
   open(destFile, "w").write(bookText)

def getButtonCoordinates(buttonNumber, totalButtons):
   screenWidth = 640
   screenHeight = 480
   topMargin = 75
   leftMargin = 40
   buttonVertSpacing = 70
   buttonWidth = 250
   maxVertButtons = 4

   # Find top of button
   vertButtonNum = buttonNumber % maxVertButtons
   top = topMargin + (vertButtonNum * buttonVertSpacing)

   # Center either one or two columns of buttons
   numColumns = math.floor(totalButtons / maxVertButtons + 1)
   #print "Cols:",numColumns
   usableWidth = screenWidth - leftMargin*2
   columnWidth = usableWidth / numColumns
   columnNum = math.floor(buttonNumber / maxVertButtons)
   columnSpacing = columnWidth - buttonWidth
   left = leftMargin + (columnSpacing/2) + columnNum * columnWidth

   return (top, left)


# Get all the book texts
bookTextFiles = os.listdir(booksDir)
bookTextFiles = filter(lambda x: x.endswith(".txt"), bookTextFiles)
bookDefs = []
thisBookNum = 0
for bookTextFile in bookTextFiles:
   (ignored, filename) = os.path.split(bookTextFile)
   splitBookTextIntoLinesFile(os.path.join(booksDir,bookTextFile), destDir+filename)

   # Generate book definition xml file
   bookTemplate = open(os.path.join(workingDir, "BookTemplate.xml"), "r").read()
   bookDefinition = templating.templateParser().parseTemplate(bookTemplate, bookTextFile=bookTextFile)
   bookDefFile = bookTextFile.replace(".txt", ".xml")
   open(destDir+bookDefFile, "w").write(bookDefinition)

   # Organize buttons on book menu screen
   (top, left) = getButtonCoordinates(thisBookNum, len(bookTextFiles))
   bookDefs.append({"BookName" : bookDefFile, "BookTitle" : bookDefFile.replace(".xml", "").replace("_", " "), "top" : str(top), "left" : str(left)})

   thisBookNum += 1

# Generate book menu screen
menuXml = templating.templateParser().parseTemplate(open(os.path.join(workingDir, "MenuTemplate.xml"), "r").read(), books=bookDefs)
open(menuFile, "w").write(menuXml)


