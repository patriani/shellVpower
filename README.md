# shellVpower
### The name is just a joke. This is a simple Unix shell implementation.

1.Geting executable file: To compilethe program insert "make" in your linux cmd and "./a.out" to execute.

2.About inputs: This shell simulator accepts concatenation of commands with ';', pipe between two commands indicated by ',' and parameters;
  - concatenation: shVpower> ls ; cd .. ; ls
  
  - pipe : shVpower> ls -l , sort
  
  - both : shVpower> ls , sort ; clear
  
  - quit : 
    - shVpower> quit
    - shVpower> quit ; ls ("ls" wont execute)
    - shVpower> ls ; quit (print "ls" return and quit)
            
  
PS: the space between command and ';' is important. Don't use without it, will not compile and request for a new input. Just one more thing, the max limit of input lenght is 512.

     
