------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------
# Akinator
------------------------------------------------------------------------------------------------------------------------------

This program has a small database of characters from "The Witcher", and you can use your own database. (see "Database" section).

Akinator works in three modes: you can use commandline flags (see "Flags" section) or choose a mode in the program.

------------------------------------------------------------------------------------------------------------------------------
# Mode
------------------------------------------------------------------------------------------------------------------------------

First mode: guessing characters. You need to ask Akinator questions about your character. If he doesn't guess it, you need to write down who you wished for and what the difference is between your character and his. After that, your character will be added to the tree. Before the program finishes, you can make changes to the database.

Second mode: get a definition for characters. Write the name of the character and Akinator will give you its definition (or tell you that the character wasn't found). 

Third mode: compare two characters you wrote about.

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
