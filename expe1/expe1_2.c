#include <stdio.h>
#include <stdlib.h>
#include <signal.h>     
#include <sys/wait.h>     
#include <unistd.h>     

int p1,p2;

void main()
{ 
  void ppdo();  
  void p1do();  
  void p2do();  

  signal(SIGINT,ppdo);  
  p1=fork();  
  if(p1==0) 
  {    
    signal(SIGUSR1,p1do);    
    for(;;);  
  }  
  else 
  {    
    p2=fork();    
    if(p2==0) 
    {      
      signal(SIGUSR2,p2do);      
      for(;;);    
    }  
  }
  
  wait(0);  
  wait(0);  
  printf("\nParent process is killed!\n");  
  exit(0);
}

void ppdo()
{ 
  kill(p1,SIGUSR1);  
  kill(p2,SIGUSR2);
}

void p1do()
{ 
  printf("\nChild process p1 is killed by parent!\n");  
  exit(0);
}

void p2do()
{ 
  printf("\nChild process p2 is killed by parent!\n");  
  exit(0);
}

