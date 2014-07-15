#include <stdio.h>
#include <stdlib.h>

int main()
{  
   tree_node_t *st1, *st2;
   int success;
   long i, k; 
   int  *m;
   int o[3] = {0,2,4};
   st1 = create_tree();
   st2 = create_tree();
   printf("Made two Trees\n");
   for( i=0; i < 400000; i++)
   {  k = 3*i; 
      success = insert( st1, k, &(o[1]) ); 
      if( success != 0 )
      {  printf("  insert %d failed in st1, return value %d\n", k, success);
         exit(-1);
      }
      success = insert( st2, k, &(o[2]) ); 
      if( success != 0 )
      {  printf("  insert %d failed in st2, return value %d\n", k, success);
         exit(-1);
      }
   }
   printf("Passed 800000 inserts.\n");
   for( i=0; i< 400000; i++)
   {  if( i%2 == 0 )
      {  m = deleteObj(st1, 3*i );
      }
      else
	{  m = deleteObj(st2, 3*i );
      }
      if( m == NULL )
         printf(" delete failed for %d, returned NULL\n", 3*i);
      else if ( (i%2==0 && *m !=2) || (i%2==1 && *m != 4))
	 printf(" delete failed for %d, returned %d\n", 3*i, *m);
   }
   printf("Passed 400000 deletes.\n");
   for( i=0; i< 1200000; i++)
   {  m = find(st1,i);
      if( i%3 == 0 && (i/3)%2 == 1 )
      {  if (m== NULL)
	    printf(" find failed on st1 for %d, returned NULL\n", i);
         else if (*m != 2 )
            printf(" find failed on st1 for %d, returned %d instead of 2\n", i,*m );
      }
      else if (m != NULL)
            printf(" find failed on st1 for %d, returned non-NULL pointer\n", i);
   }
   printf("Passed 1200000 finds. End of test.\n");
   return(0);
   
}
