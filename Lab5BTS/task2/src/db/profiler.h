//==== NOTE ==================================================================
// You do not need to change anything in this file, but feel free to read it
// if it is of interest.
//
// You only need to update bstdb.c
//============================================================================

#ifndef PROFILER_H
#define PROFILER_H

#include "database.h"

//=== STRUCT ==================================================================
//         Name: profile
//  Description: This struct is used to store intermediate results when
//               running the profile tests. It also maintains a pointer to the
//               database that is being tested.
//=============================================================================
struct profile {
    struct database *db;        // pointer to database

    int    total_insert;        // Number of insertions performed by profiler
    int    insert_errors;       // Number of insertions which returned error
    double avg_insert_time;     // Average time of insertion in seconds
    double var_insert_time;     // Variance of insertion time
    double total_insert_time;   // Total duration of insertion profile test
    

    int    total_search_title;       // Number of titles searched for
    int    search_title_errors;      // Number of incorrect books returned
    double avg_search_time_title;    // Average time to find a book
    double var_search_time_title;    // Variance of time taken to find a book
    double total_search_time_title;  // Total duration of search title test
    
    int    total_search_word_count;      // Number of word_counts searched for
    int    search_word_count_errors;     // Number of incorrect counts returned
    double avg_search_time_word_count;   // Average time to find a book
    double var_search_time_word_count;   // Variance of time taken to find book
    double total_search_time_word_count; // Total duraction of search word count
    
};

int  profiler_init ( void );
void profiler_run  ( struct profile *p );
void profiler_quit ( void );

#endif