#define SQLITE_CORE 1  /* Disable the API redefinition in sqlite3ext.h */
#include "levenshtein.h"
#include <string.h>
#include <stdlib.h>
#include <malloc/malloc.h>


const sqlite3_api_routines sqlite3_apis = {
    sqlite3_value_text,
    sqlite3_value_type
};


#define LEVENSHTEIN_MAX_STRLEN 1024
// O(n*m) !!!


#define ___MIN___(a,b) (((a)<(b))?(a):(b))

SQLITE_EXTENSION_INIT1

int levenshtein_distance(char*, char*);



static void levenFunc(
                      sqlite3_context *context,
                      int argc,
                      sqlite3_value **argv
                      ){
    
	int result;
    
    
    
	if ( sqlite3_value_type( argv[0] ) == SQLITE_NULL || sqlite3_value_type( argv[1] ) == SQLITE_NULL ){
		sqlite3_result_null( context );
		return;
	}
    
	result = levenshtein_distance(
                                  (char*) sqlite3_value_text( argv[0] ),
                                  (char*) sqlite3_value_text( argv[1] )
                                  );
    
	if ( result == -1 ){
		// one argument too long
		sqlite3_result_null( context );
        return;
	}
    
	sqlite3_result_int( context, result );
}

int sqlite3_extension_init(
                           sqlite3 *db
                           ){


	sqlite3_create_function_v2(db, "levenshtein", 2, SQLITE_ANY, 0, (void *)levenFunc, 0, 0, 0);
    
    //return sqlite3_create_module_v2(db, "fts3", &fts3Module, (void *)pHash, hashDestroy);
	return SQLITE_OK;
}


int levenshtein_distance( char* s1, char* s2 ) {
	int k,i,j,n,m,cost,*d,result;
	n=strlen(s1); 
	m=strlen(s2);
    
	if ( n > LEVENSHTEIN_MAX_STRLEN || m > LEVENSHTEIN_MAX_STRLEN ){
		return -1;
	}
    
	if( n !=0 && m != 0){
		d=malloc((sizeof(int))*(++m)*(++n));
		for(k=0;k<n;k++){
			d[k]=k;
		}
		for(k=0;k<m;k++){
			d[k*n]=k;
		}
		for(i=1;i<n;i++){
			for(j=1;j<m;j++){
				if(s1[i-1]==s2[j-1])
					cost=0;
				else
					cost=1;
				d[j*n+i]=___MIN___( ___MIN___( d[(j-1)*n+i]+1, d[j*n+i-1]+1 ), d[(j-1)*n+i-1]+cost );
			}
		}
		result=d[n*m-1];
		free(d);
		return result;
	}
	else {
		return (n>m)?n:m;
	}
}

