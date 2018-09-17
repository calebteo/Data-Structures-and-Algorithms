//==== NOTE ==================================================================
// You do not need to change anything in this file, but feel free to read it
// if it is of interest.
//
// You only need to update bstdb.c
//============================================================================

#include "db/database.h"
#include "db/profiler.h"

#include <stdio.h>
#include <stdlib.h>

//=== FUNCTION ================================================================
//         Name: profile
//  Description: Bind to a database using the function pointer passed as an
//               argument and run our simple profiling tests to assess its
//               performance.
//=============================================================================
void
profile ( char *dbname, void (*bind)(struct database *db) ) {
    struct database db;
    struct profile p;

    // Bind to the appropriate database
    bind(&db);
    
    printf("\nProfiling %s\n", dbname);
    printf("-------------------------------------------\n\n");

    // Attempt to initialize the database
    if (!db.init()) {
        printf("%s failed to initialize\n", dbname);
        return;
    }

    // point the profiler at the database
    p.db = &db;

    // run the profiler
    profiler_run(&p);

    // print results of profiling
    printf("Total Inserts                : %12d\n", p.total_insert);
    printf("Num Insert Errors            : %12d\n", p.insert_errors);
    printf("Avg Insert Time              : %10.6lf s\n", p.avg_insert_time);
    printf("Var Insert Time              : %10.6lf s\n", p.var_insert_time);
    printf("Total Insert Time            : %10.6lf s\n", p.total_insert_time);  
    printf("\n");
    printf("Total Title Searches         : %12d\n", p.total_search_title);
    printf("Num Title Search Errors      : %12d\n", p.search_title_errors);
    printf("Avg Title Search Time        : %10.6lf s\n",
        p.avg_search_time_title);
    printf("Var Title Search Time        : %10.6lf s\n",
        p.var_search_time_title);
    printf("Total Title Search Time      : %10.6lf s\n",
        p.total_search_time_title); 
    printf("\n");
    printf("Total Word Count Searches    : %12d\n", 
        p.total_search_word_count);
    printf("Num Word Count Search Errors : %12d\n",
        p.search_word_count_errors);
    printf("Avg Word Count Search Time   : %10.6lf s\n",
        p.avg_search_time_word_count);
    printf("Var Word Count Search Time   : %10.6lf s\n",
        p.var_search_time_word_count);
    printf("Total Word Count Search Time : %10.6lf s\n",
        p.total_search_time_word_count);    
    printf("\n");

    // show off
    db.stat();  
    
    // close the database
    db.quit();
}

//=== FUNCTION ================================================================
//         Name: main
//  Description: Program entry point
//=============================================================================
int
main ( int argc, char *argv[] ) {
    if (!profiler_init()) {
        printf("Profiler failed to initialize\n");
        return EXIT_FAILURE;
    }

    // you can comment out this line to ignore profiling the linked list
    profile( "listdb", database_bind_listdb );

    profile( "bstdb", database_bind_bstdb );

    profiler_quit();

    puts("Press Enter to quit...");
    fgetc(stdin);

    return EXIT_SUCCESS;
}