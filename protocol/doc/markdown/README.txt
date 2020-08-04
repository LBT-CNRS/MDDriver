The main page of the MDDriver tutorial is generated using Markdown.
The source file of the tutorial is mddriver_markdown.txt.

To regenerate the html from mddriver_markdown.txt, and integrate 
this in the doxygen documentation, use this command, BEFORE 
generating documentation with Doxygen : 

perl Markdown.pl --html4tags mddriver_markdown.txt > ../html/mddriver.html

It generate mddriver.html which contains the tutorial and put it in
the parent html directory. 
