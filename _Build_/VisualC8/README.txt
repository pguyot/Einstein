

+------------------------------------------------------------+

VisualC 8 does not recognize C++ files ending in .cp by 
default. To allow compilation of this Solution, you will have 
to teach VisualC the .cp file extension first, then reload 
the solution.

 - click on the menu "Tools", the choose "Options"
 - open "Projects and Solution"
 - click on "VisualC++ Project Settings"
 - in "C/C++ File Extensions", append  ;*.cp

 - please make sure that you installed the current VisualC 
   service pack

Happy coding,

  Matthias