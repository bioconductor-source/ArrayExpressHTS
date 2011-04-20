#include <stdio.h> 
#include <string.h>  
#include <stddef.h> 
#include <R.h>
#include <Rdefines.h>


/*
    Andrew Tikhonov <andrew@ebi.ac.uk>
*/

#define LINESIZE 1000
#define NEWLINE '\n'


void getReadLength(const char** infname , int* length) {  
    char line[LINESIZE]; 
    
    FILE *infile;
    
    /* init to "undefined" */
    length[0] = 0;
    
    if((infile = fopen(infname[0], "r")) == NULL) { 
        printf("Error opening file %s\n", infname[0]); 
        length[0] = -1;
        
    } else {
        fgets(line, sizeof(line), infile); // read first line and discard it
        fgets(line, sizeof(line), infile); // read second line and check its length
        
        fclose(infile); 
        
        for (int i = 0; i < LINESIZE;i++) {
            if (line[i] == NEWLINE || line[i] == 0) {
                length[0] = i;
                break;
            }
        }
    }
    
}


//void count_polyL( char **letter, int *len, char **seq, int *res ) {
SEXP count_polyL(SEXP letter, SEXP len, SEXP seq) {
    SEXP result;
    int *p_result; 
    
    //printf("Printed from countL2:\n");
    
    int length = INTEGER_VALUE(len);
    //printf("length= %d\n", length);
    
    char char0 = CHAR(STRING_ELT(letter, 0))[0];
    //printf("char0= %c\n", char0);
    
    PROTECT(result = NEW_INTEGER(length));
    p_result = INTEGER_POINTER(result);

    //for (int i0=0; i0 < length; i0++) {
    //    printf("i0=%d\n", i0);
    //    char *p_seq = CHAR(STRING_ELT(seq, i0));
    //    int p_seq_strlen = strlen(p_seq);
    //    printf("p_seq_strlen=%d\n", p_seq_strlen);
    //    printf("p_seq[%d]=%s\n", i0, p_seq);
    //}
    //printf("p_result[0]= %d\n", p_result[0]);
    
    int j, count;
 
    for(int i = 0; i < length; i++) { 
        //printf("i= %d\n", i);
    
        count = 0;
        j = 0;
        
        char *p_seq = (char *) CHAR(STRING_ELT(seq, i));
        
        int strlength = strlen(p_seq);
        
        //printf("strlength= %d\n", strlength);
        //printf("p_seq[j]= %c\n", p_seq[j]);
        
        while( j < strlength && p_seq[j] == char0 ) {
            count++;
            j++;
        
            //printf("count= %d\n", count);
            //printf("j= %d\n", j);
        }
        
        p_result[i] = count;
        
        //printf("p_result[i]= %d\n", p_result[i]);
        
        count = 0;
        j = strlength -1;
        
        //printf("j= %d\n", j);
        //printf("p_seq[j]= %c\n", p_seq[j]);

        while( j >= 0 && p_seq[j] == char0 ) {
            count++;
            j--;
        
            //printf("count= %d\n", count);
            //printf("j= %d\n", j);
        }
        
        if( p_result[i] < count ) {
            p_result[i] = count; 
        }
    
        //printf("p_result[i]= %d\n", p_result[i]);
    }
    
    UNPROTECT(1);
    return result;
}


//void phred_to_average_qual( int *len, char **quals, double *res ) {
SEXP phred_to_average_qual(SEXP len, SEXP quals) {
    
    SEXP result;
    double *p_result; 
    
    //printf("Printed from phred_to_average_qual2:\n");
    
    int length = INTEGER_VALUE(len);
    //printf("length= %d\n", length);
    
    PROTECT(result = NEW_NUMERIC(length));
    p_result = NUMERIC_POINTER(result);

    int i, j;
    double r;
    
    for( i = 0; i < length; i++ ) { 
        
        char *p_qual = (char *) CHAR(STRING_ELT(quals, i));
        int strlength = strlen(p_qual);
        
        r = 0;
    
        for( j = 0; j < strlength; j++ ) {
            r = r + p_qual[j] - 33;
        }
        
        p_result[i] = r / strlength; 
    }
    
    UNPROTECT(1);
    return result;
    
}

// int *len, int *flag
SEXP is_firstmate( SEXP len, SEXP flag ) {
    SEXP result;
    int *p_result;

    int length = INTEGER_VALUE(len);

    PROTECT(result = NEW_INTEGER(length));
    p_result = INTEGER_POINTER(result);

    int i;

    for( i = 0; i < length; i++ ) {
        p_result[i] = (INTEGER(flag)[i] & 0x41) == 0x41;
    }

    UNPROTECT(1);
    return result;
}

SEXP is_secondmate( SEXP len, SEXP flag ) {
    SEXP result;
    int *p_result;

    int length = INTEGER_VALUE(len);

    PROTECT(result = NEW_INTEGER(length));
    p_result = INTEGER_POINTER(result);

    int i;

    for( i = 0; i < length; i++ ) {
        p_result[i] = (INTEGER(flag)[i] & 0x81) == 0x81;
    }

    UNPROTECT(1);
    return result;
}

//void is_polyX( int *len, int *limit, char *base, char **seq, int *res ) {
SEXP is_polyX( SEXP len, SEXP lim, SEXP base, SEXP seq) {

    SEXP result;
    int *p_result; 
    
    int length = INTEGER_VALUE(len);
    //printf("length= %d\n", length);
    
    int limit = INTEGER_VALUE(lim);
    //printf("limit= %d\n", limit);
    
    PROTECT(result = NEW_INTEGER(length));
    p_result = INTEGER_POINTER(result);
    
    int i, j, count;
    
    for( i = 0; i < length; i++ ) { 
        count = 0;
        j = 0;
        
        char *p_seq = (char *) CHAR(STRING_ELT(seq, i));
        int strlength = strlen(p_seq);
        
        while( count < limit && j < strlength && p_seq[j] == 'A' ) {
            count++;
            j++;
        }
        if( count >= limit ) {
            p_result[i] = 1;
        } else {
            count = 0;
            j = strlength -1;
            while( count < limit && j >= 0 && p_seq[j] == 'A' ) {
                count++;
                j--;
            }
            if( count >= limit ) {
                p_result[i] = 1;
            }
            else {
                p_result[i] = 0;
            }
        }
    }
    
    UNPROTECT(1);
    return result;

}
