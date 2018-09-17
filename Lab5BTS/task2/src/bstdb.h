//==== NOTE ==================================================================
// You do not need to change anything in this file, but feel free to read it
// if it is of interest.
//
// You only need to update bstdb.c
//============================================================================

#ifndef BSTDB_H
#define BSTDB_H

int   bstdb_init           ( void );
int   bstdb_add            ( char *name, int word_count );
int   bstdb_get_word_count ( int doc_id );
char* bstdb_get_name       ( int doc_id );
void  bstdb_stat           ( void );
void  bstdb_quit           ( void );

#endif