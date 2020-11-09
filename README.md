# shellVpower
### The name is just a joke. This is a simple Unix shell implementation.

1.Geting executable file: To compilethe program insert "make" in your linux cmd and "./a.out" to execute.

2.About inputs: This shell simulator accepts concatenation of commands with ';', pipe between two commands indicated by ',' and parameters;
  - concatenation: [...]$ ls ; cd .. ; ls
  
  - pipe : [...]$ ls -l , sort
  
  - both : [...]$ ls , sort ; clear
  
  - quit : 
    - [...]$ quit
    - [...]$ quit ; ls (ls wont execute)
    - [...]$ ls ; quit (print ls and quit)
            
  
PS: the space between command and ';' is important. Don't use without it, will not compile and request for a new input

     
