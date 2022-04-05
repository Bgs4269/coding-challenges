/*
*
* This is a mildly optimised C code for integer factorisation.
* In its current form it hadnles 64-bit integers in a fraction of a second.
* e.g. finding the primes for 718061908054592475 on my laptop took less than 1ms.
*
* Note 1: Why C?
* In this challenge, most went with Python, which is a good language, but as any
* scripting language has its weak spots. Especially in such operations.
* I thought this would be a good demo on how C is different and how it can be more
* efficient.
*
* Note 2: Why this way?
* I made a few choices that were more about showing the difference over optimal
* solution. e.g. I chose to always allocate one block of 64-bit pairs in the array,
* even though a generous for 64-bit BUFSIZ allocation would have made it easier and
* today we can freely "waste" that amount of memory.
*
* Note 3: 64-bit?
* With some tweaks (mostly around formatting), this can be tweaked to use unsigned __int128
* for primeSize, but I did not bother to go through the code again. Past 128 bits, 
* dedicated arithmetic functions or libraries are needed.
*
* Note 4: Further improvements?
* I see a few options to improve for larger integers and faster execution:
* - Multithreading, with each thread handling a batch of odds, and toFactor is only decreased
*   when results are in. A fixed size array or shared memory could be used for IPC.
* - CUDA or opencl for utilise GPU cores.
* - As we only need a small set of operations, custom build them for arbitrary sized types.
* - Use an arbitrary precision library like libgcrypt, openssl, bigz etc.
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//#define DEBUG = 1

/*#ifdef __SIZEOF_INT128__
  typedef unsigned __int128	primeSize;
#elif
  typedef	uint64_t	primeSize;
#endif*/

typedef	uint64_t	primeSize;
int			factCnt=0;


/*
*
* print factor table in a human readable format.
*
*/
void printFactors(primeSize *factorArray)
{
primeSize	i;
primeSize	*l_factors=factorArray;

  for(i=0; i<factCnt; ++i) printf("%ld^%ld ", l_factors[i*2], l_factors[(i*2)+1]);
  printf("\n");
}


/*
*	MAIN
*/
int main(int argc, char **argv)
{
primeSize	toFactor, odd;
primeSize	*factors = NULL;
int		f=0;

factors=(primeSize *)malloc(2*sizeof(primeSize));
if(factors==NULL)
  {
    printf("ERROR: Unable to allocate initial memory for factors!\n");
    exit(1);
  }

if(argc==1) exit(1);

toFactor=atol(argv[1]);
memset(factors, 0, 2*sizeof(primeSize));

printf("Factorising %ld\n", toFactor);

// Handle 2.
if(toFactor%2==0)
  {
    factCnt=1;
    factors[0]=2;

    while(toFactor%2==0)
      {
        factCnt=1;
        ++factors[1];
        toFactor/=2;
      }
  
    // Add new memory to factor array.
    primeSize *new_factors=(primeSize *)realloc(factors, (2*factCnt+2)*sizeof(primeSize));
    if(new_factors==NULL)
      {
        printf("ERROR: Unable to allocate more memory for factors.\n");
        exit(1);
      } else {
        factors=new_factors;
      }
    factors[2]=0;
    factors[3]=0;
    printf("2s=%ld\n", factors[1]);
  }


// Handle 3.
f=3;
if(toFactor%f==0)
  {
    ++factCnt;

    factors[2*(factCnt-1)]=f;
    factors[2*(factCnt-1)+1]=0;
    
    while(toFactor%f==0)
      {
        ++factors[2*(factCnt-1)+1];
        toFactor/=f;
      }
      
    // Add new memory to factor array.  
    primeSize *new_factors=(primeSize *)realloc(factors, 2*(factCnt+1)*sizeof(primeSize));
    if(new_factors==NULL)
      {
        printf("ERROR: Unable to allocate more memory for factors.\n");
        exit(1);
      } else {
        factors=new_factors;
      }
    factors[2*factCnt]=0;
    factors[2*factCnt+1]=0;
    
    printf("%ds=%ld\n", f, factors[2*(factCnt-1)+1]);
  }

// Handle 5.
f=5;
if(toFactor%f==0)
  {
    ++factCnt;

    factors[2*(factCnt-1)]=f;
    factors[2*(factCnt-1)+1]=0;
    
    while(toFactor%f==0)
      {
        ++factors[2*(factCnt-1)+1];
        toFactor/=f;
      }
      
    // Add new memory to factor array.  
    primeSize *new_factors=(primeSize *)realloc(factors, 2*(factCnt+1)*sizeof(primeSize));
    if(new_factors==NULL)
      {
        printf("ERROR: Unable to allocate more memory for factors.\n");
        exit(1);
      } else {
        factors=new_factors;
      }
    factors[2*factCnt]=0;
    factors[2*factCnt+1]=0;
    
    printf("%ds=%ld\n", f, factors[2*(factCnt-1)+1]);
  }

// Handle 7.
f=7;
if(toFactor%f==0)
  {
    ++factCnt;

    factors[2*(factCnt-1)]=f;
    factors[2*(factCnt-1)+1]=0;
    
    while(toFactor%f==0)
      {
        ++factors[2*(factCnt-1)+1];
        toFactor/=f;
      }
      
    // Add new memory to factor array.  
    primeSize *new_factors=(primeSize *)realloc(factors, 2*(factCnt+1)*sizeof(primeSize));
    if(new_factors==NULL)
      {
        printf("ERROR: Unable to allocate more memory for factors.\n");
        exit(1);
      } else {
        factors=new_factors;
      }
    factors[2*factCnt]=0;
    factors[2*factCnt+1]=0;
    
    printf("%ds=%ld\n", f, factors[2*(factCnt-1)+1]);
  }



// Compute the rest of the odds.
  for(odd=f+2; odd<sqrt(toFactor); odd+=2)
    {
      if( (odd%3==0) || (odd%5==0) || (odd%7==0) ) continue;
      
      int g=0;
      while(toFactor%odd==0)
        {
          toFactor/=odd;
          ++g;
        }
      if(g!=0)
        {
          // We had at least one match.
          ++factCnt;
          factors[2*(factCnt-1)]=odd;
          factors[2*(factCnt-1)+1]=g;
          printf("%lds=%d\n", odd, g);
          
          // Add new memory to factor array.
          primeSize *new_factors=(primeSize *)realloc(factors, 2*(factCnt+1)*sizeof(primeSize));
          if(new_factors==NULL)
            {
              printf("ERROR: Unable to allocate more memory for factors.\n");
              exit(1);
            } else {
              factors=new_factors;
            }
          factors[2*factCnt]=0;
          factors[2*factCnt+1]=0;    
        }
        
      if(toFactor==1) break;
    }


// Got one left.
  if(toFactor!=1) 
    {
      factors[2*factCnt]=toFactor;
      factors[2*factCnt+1]=1;    
      ++factCnt;
      printf("DEBUG: %lds=1\n", toFactor);
    }
  
  printFactors(factors);

exit(0);
}

