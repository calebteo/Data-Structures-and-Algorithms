//==== NOTE ==================================================================
// You do not need to change anything in this file, but feel free to read it
// if it is of interest.
//
// You only need to update bstdb.c
//============================================================================

#ifndef DATABASE_H
#define DATABASE_H

// This database struct acts as an abstraction layer on top of your database
// implementation. The attributes of the struct are function pointers. Once
// a function conforms to the requirements of the pointer (e.g. function must
// return an int and take an int as an argument), the pointer can point to it.
// We can then call the function via the pointer.
//
// This is essentially how the profiler works with both listdb and bstdb.
// This abstraction layer points to the functions that the profiler should
// test and the profiler calls the appropriate functions from a distance
//
// Pretty neat, eh?
struct database {
    int   (*init)           ( void );
    int   (*add)            ( char *name, int word_count );
    int   (*get_word_count) ( int doc_id );
    char* (*get_name)       ( int doc_id );
    void  (*stat)           ( void );
    void  (*quit)           ( void );
};

void database_bind_listdb ( struct database *db );
void database_bind_bstdb  ( struct database *db );

#endif