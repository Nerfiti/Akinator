------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------
# Akinator
------------------------------------------------------------------------------------------------------------------------------

This program have a not large database characters from "The Witcher" and can use your own database (see "Database" section).

Akinator can work in three modes. You can use command-line flags (see "Flags" section) or choose mode in program.

------------------------------------------------------------------------------------------------------------------------------
# Mode
------------------------------------------------------------------------------------------------------------------------------

First mode: Guessing characters. You have to ask Akinator's questions about your character. If Akinator don't guess your character, you have to write who you have made a wish for and what a difference between your character and Akinator's. After this your character will be added to the current tree. And before finish program, you can write changes to the database.

Second mode: Get a definition of the characters. You have to write the name of your character and Akinator give you it's definition (or say, that character didn't find)

Third mode: Compare two characters, you wrote.

------------------------------------------------------------------------------------------------------------------------------
# Flags
------------------------------------------------------------------------------------------------------------------------------

-MG: start program with the first  mode (Guessing)
-MD: start program with the second mode (Definitions)
-MC: start program with the third  mode (Comparing)

------------------------------------------------------------------------------------------------------------------------------
# Database
------------------------------------------------------------------------------------------------------------------------------

Database consists of the element like:

{"element_name"
    left_element("NO")
    right_element("YES")
}

Empty element has the form: {}

To write something to the database with "TreePrint" function you must use PRE_ORDER print mode.

------------------------------------------------------------------------------------------------------------------------------
# Espeak
------------------------------------------------------------------------------------------------------------------------------

Akinator have voise.

To use this function your system must have "espeak" in the PATH.

------------------------------------------------------------------------------------------------------------------------------
# Graphviz
------------------------------------------------------------------------------------------------------------------------------

Program can create html page with dumps ("TreeGraphDump" function). 

To use this function your system must have "graphviz" (command: "dot")

------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------
