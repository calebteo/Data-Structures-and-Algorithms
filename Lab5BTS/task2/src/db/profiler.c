//==== NOTE ==================================================================
// You do not need to change anything in this file, but feel free to read it
// if it is of interest.
//
// You only need to update bstdb.c
//============================================================================

#include "profiler.h"

#include <math.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32     
#include <windows.h>
#endif

#ifdef __MACH__
#include <sys/time.h>
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define MU_COLLECTION    100000.0f // average collection size
#define SIGMA_COLLECTION   5000.0f // standard deviation of collection size

#define MU_BOOK           10000.0f // average word count of book
#define SIGMA_BOOK          500.0f // standard deviation of book word count

#define LONG_STRING 64 // Maximum size of buffer allocated to book title

//----------------------------------------------------------------------------
// Book title generation method blatantly stolen from:
// http://novelistvmd.awardspace.com/WesternTitleGenerator2.htm
//
// Essentially we choose random adjectives and random nouns from the two
// arrays below. These are combined according to one of 8 randomly chosen
// title patterns
//
//  1. <ADJECTIVE> <NOUN>
//  2. The <ADJECTIVE> <NOUN>
//  3. <NOUN> of <NOUN>
//  4. The <NOUN>'s <NOUN>
//  5. The <NOUN> of the <NOUN>
//  6. The <NOUN> in the <NOUN>
//  7. The <NOUN> of the <ADJECTIVE> <NOUN>
//  8. The <NOUN> and the <NOUN>
//----------------------------------------------------------------------------

static char *nouns[] = {
    "Cowboy","Dream","Dreamer","Settlers","Gunslingers","Maverick","Skies",
    "Class","Sheriff","Desert","Arms","Beauty","Redhead","Gaslight",
    "Lover","Ruby","Widow","Mine","Hope","Bluff","Plains","Lariat","Sierra",
    "Antelope","Prairie","Horizon","Graveyard","Churchyard","Hawk","Landowner",
    "Soul","Souls","Touch","Meadow","Boots","Spurs","Saddle","Gold","Clouds",
    "Men","Women","Mountains","Guitar","Smoke","Cattle","Drive","Stirrup",
    "Ice","Snow","Night","Silk","Secret","Secrets","Dove","Coyote","Wagon",
    "Solitude","Husband","Wife","Man","Woman","Boy","Loner","Wagons","Land",
    "Girl","Raven","Truth","Soldier","Beau","Eagle","Sierras","Outlaw",
    "Body","Captive","Evening","Velvet","Lash","Deer","Rustlers","Leather",
    "Teacher","Lake","Lawman","Temptation","Wolves","Spinet","Drums",
    "Rein","Rainbow","Rain","Flight","Flying","Lark","Dust","Sun","Gun",
    "Soaring","Stallion","Wings","Mist","Sky","Wind","Race","Vultures",
    "Seeker","Winter","River","Door","Mountain","Magic","Yucca","Brook",
    "Honor","Gate","Cloud","Glen","Duster","End","Beginning","Feather",
    "Tale","Waltz","Tales","Steer","Church","Fences","Gentleman","Lady",
    "Vine","Path","Willow","Birch","Birches","Trail","Petals","Destiny",
    "Pine","Garden","Theft","Thief","Legend","Rifle","Squaw","Arapahoe",
    "Chance","Oak","Spark","Sparks","Stream","Streams","Mare","Canoe","Wolf",
    "Bible","Darkness","Silence","Kiss","Book","Butterfly","Death","Sioux",
    "Shadow","Ring","Diary","Storm","Storms","Bullets","Holster","Winchester",
    "Necklace","Stranger","Lord","Lords","Navajo","Tumbleweed","Barrel",
    "Star","Locket","Stars","Painting","Heart","Heat","Cougar","Bandit",
    "Sketch","Twilight","Moon","Shores","Paint","Pinto","Bullet","Calf","Town",
    "Frame","Courage","Doctor","School","Rose","Apache","Spinster",
    "Roses","Stone","Stones","Panel","Creek","Pass","Badlands","Butte","Lamp",
    "Tree","Rattlesnake","Rancher","Tracks","Buffalo","Spring","Autumn","Colt",
    "Spirit","Return","Father","Mother","Brave","Savage","Summer","Winter",
    "Dog","Legacy","Birth","Healer","Healing","Year","Years","Death","Dying",
    "Fall","Lace","Luck","Pearl","Treasure","Tears","Son","Sons","Child",
    "Children","Riders","Destruction","Devotion","Promise","Gift","Word",
    "Stable","Barn","Renegade","Words","Thought","Thoughts","Guard","Angel",
    "Angels","Search","Eye","Lamplight","Eyes","Danger","Game","Fire","Flame",
    "Flames","Bride","Time","Spaniard","Light","Lights","Doors","Window",
    "Windows","Circus","Road","Sombrero","Dirt","Ashes","Memory","Thorn",
    "Thorns","Name","Names","Dozen","Senorita","Sage","Future","Past",
    "Nothing","Arroyo","Gulch","Mesa","Medicine","Map","Bible","Way","Lily",
    "Valley","Abyss","Hunter","Basin","Roundup","Canyon","Edge","Smile",
    "Embrace","Diamond","Grave"
};

static char *adjectives[] = {
    "Lost","Only","Last","First","Untamed","Fearless","Third","Sacred",
    "Bold","Lovely","Iron","Fourth","Missing","Shadowy","Seventh","Towering",
    "Unabashed","Unloved","Flaming","Fifth","Vacant","Lace",
    "Cold","Hot","Golden","Enthralled","Lone","Silent","Solitary","Unknown",
    "Sixth","Brave","Whispering","Smooth","Fiery","Enchanted","Silken",
    "Rough","Frozen","Wild","Moonlit","Slender","Trembling","Fallen","Lacey",
    "Ragged","Burned","Splintered","Silk","Distant","Reluctant","Haunted",
    "Magnificent","Luscious","Sparkling","Raven","White","Hurried","Ancient",
    "Hidden","Jeweled","Pastel","Captured","Stolen","Miraculous","Lilac",
    "Cloudy","Unbound","Growing","Kissing","Verdant","Red","Azure",
    "Rising","Falling","Elemental","Unfinished","Bound","Prized","Dungeon",
    "Obsessed","Unwilling","Dancing","Hard","Eager","Ravaged","Sleeping",
    "Mysterious","Proud","Wanton","Professional","Willing","Devoted",
    "Auburn","Misty","Irresistible","Locked","Secluded","Ruby","Final",
    "Missing","Plaintive","Dark","Darkest","Silver","Silvery","Living",
    "Yearning","Black","White","Entwined","Invisible","Legato","Next",
    "Seventh","Scarlet","Green","Blue","Poignant","Violet","Grey","Wounded",
    "Bloody","Emerald","Crystal","Lyrical","Sharp","Delicious","Shattered",
    "Dangerous","Somber","Deep","Twinkling","Dwindling","Absent","Soft",
    "Fierce","Vacant","Burning","Summer","Loneliest","Some","No","All",
    "Tinkling","Grave","Engraved","Living","Dead","Happiest","Echoing",
    "Every","Each","Which","What","Playful","Slumbering","Weeping","Softer",
    "Dying","Lonely","Laughing","Whispering","Forgotten","Empty","Frozen",
    "Magical","Unlucky","Lucky","New","Tentative","Clear","Faraway","Sweet",
    "Hungry","Autumn","Spring","Winter","Broken","Searching","Fleeting",
    "Spellbound"
};


struct book {
    char title[LONG_STRING]; // Randomly generated book title
    int word_count;          // Randomly generated book length in words
    int db_id;               // Book's assigned ID in the database
};

static struct book *g_books = NULL; // We'll make a random number of books
static size_t       g_collection_size = 0; // Number of books created

double 
get_time ( void ) {
#ifdef _WIN32     
	LARGE_INTEGER time;
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&time);
		
	return ((double)time.QuadPart) / frequency.QuadPart;	
#else
    struct timespec t;
#if __MACH__   
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    int r = clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    t.tv_sec = mts.tv_sec;
    t.tv_nsec = mts.tv_nsec;
#else
    static double prev_value = 0.0;

    // CLOCK_MONOTONIC represents the absolute elapsed wall-clock time since 
    // some arbitrary, fixed point in the past. It isn't affected by changes in 
    // the system time-of-day clock.
    int r = clock_gettime( CLOCK_MONOTONIC, &t );    
#endif    
    if ( r < 0 ) {
        // gettime can fail, so we need to do a check and possibly print error
        fprintf(stderr, "%s\n", strerror(errno)); 
        // On error return previous time
        return prev_value;
    }
    double ns = t.tv_nsec;        // get elapsed nanoseconds
    double s = ns * 0.000000001;  // nanoseconds to seconds
    time_t tts = t.tv_sec;        // get elapsed seconds
    s += difftime( tts, 0 );      // count seconds since epoch and add to total
    prev_value = s;               // backup in case future call fails
    return s;                     // return time
#endif
}

//=== FUNCTION ================================================================
//         Name: box_muller
//  Description: Uses the Box-Muller transform to generate pseudorandom values
//               according to a Gaussian distribution as determined by the
//               values of mu and sigma.
// 
//               The rand() function returns random values according to a
//               uniform distribution. This is fine if we want to give the
//               impression of true randomness. Box-Muller lets us generate
//               random numbers which are clustered around some average value
//               (mu) with a spread that is determined by sigma.
//=============================================================================
static double
box_muller ( double mu, double sigma ) {
    static int gen = 0;
    static double z0, z1;
    double a, r;

    // we need to generate two values on every alternating call
    // flip the boolean so we know whether or not to generate
    gen = !gen;

    // check to see if we still have a value in reserve.
    if(!gen) {
        // if so, return it 
        return z1 * sigma + mu;
    }

    // get the angle and radius we'll use to generate the new random values
    a = 2 * M_PI * (rand()/(double)RAND_MAX); 
    r = sqrt(-2 * log(rand()/(double)RAND_MAX));

    // generate two new Gaussian distributed values from sides of triangle
    z0 = r*cos(a);
    z1 = r*sin(a);

    // return one of the two values 
    return z0 * sigma + mu;
}

//=== FUNCTION ================================================================
//         Name: welford
//  Description: Welford's online algorithm lets us compute a running value
//               for average and variance without needing to store all sample
//               values in memory. With each new value that is generated, we
//               simply update mean and variance in response and then discard
//               the value. Useful for saving space.
//=============================================================================
static void
welford ( double val, size_t i, double *avg, double *var ) {
    double d1, d2, mu=*avg, sig=*var;

    d1 = val - mu;
    mu += d1/(i+1);
    d2 = val - mu;
    sig += d1*d2;

    *avg = mu;
    *var = sig;
}

//=== FUNCTION ================================================================
//         Name: generate_book
//  Description: Generate a book with a random title and length.
//=============================================================================
static void
generate_book ( struct book *book ) {
    static size_t num_nouns  = sizeof(nouns)/sizeof(nouns[0]);
    static size_t num_adj   = sizeof(adjectives)/sizeof(adjectives[0]);
    
    // Book length is derived from normal distribution
    book->word_count = box_muller( MU_BOOK, SIGMA_BOOK );

    // Pick random title pattern, then pick random title nouns/adjectives
    switch (rand()%8) {
        case 0:
            sprintf( book->title, "%s %s",
                adjectives[rand()%num_adj], nouns[rand()%num_nouns]);
            break;
        case 1:
            sprintf( book->title, "The %s %s",
                adjectives[rand()%num_adj], nouns[rand()%num_nouns]);
            break;
        case 2:
            sprintf( book->title, "%s of %s",
                nouns[rand()%num_nouns], nouns[rand()%num_nouns]);
            break;
        case 3:
            sprintf( book->title, "The %s's %s",
                nouns[rand()%num_nouns], nouns[rand()%num_nouns]);
            break;
        case 4:
            sprintf( book->title, "The %s of the %s",
                nouns[rand()%num_nouns], nouns[rand()%num_nouns]);
            break;
        case 5:
            sprintf( book->title, "%s in the %s",
                nouns[rand()%num_nouns], nouns[rand()%num_nouns]);
            break;
        case 6:
            sprintf( book->title, "The %s of the %s %s",
                nouns[rand()%num_nouns], adjectives[rand()%num_adj],
                nouns[rand()%num_nouns]);
            break;
        case 7:     
        default:
            // If something goes wrong, just default to the eigth pattern
            sprintf( book->title, "The %s and the %s",
                nouns[rand()%num_nouns], nouns[rand()%num_nouns]);
            break;
    }
}

//=== FUNCTION ================================================================
//         Name: insertion
//  Description: Perform an insertion using profiler's database. Return 0 if 
//               no error occurred and 1 if something went wrong
//=============================================================================
static int
insertion ( struct profile *p, int i ) {    
    g_books[i].db_id = p->db->add( g_books[i].title, g_books[i].word_count );   
    return ( g_books[i].db_id < 0 );
}

//=== FUNCTION ================================================================
//         Name: search_title
//  Description: Perform random title search using profiler's database.
//               Return 0 if no error occurred and 1 if something went wrong
//=============================================================================
static int
search_title ( struct profile *p, int i ) { 
    size_t j = rand()%g_collection_size;
    char *t = p->db->get_name( g_books[j].db_id );  
    return ( !t || strcmp( g_books[j].title, t ) != 0 );
}

//=== FUNCTION ================================================================
//         Name: search_word_count
//  Description: Perform random word count search using profiler's database.
//               Return 0 if no error occurred and 1 if something went wrong
//=============================================================================
static int
search_word_count ( struct profile *p, int i ) {    
    size_t j = rand()%g_collection_size;
    int wc = p->db->get_word_count( g_books[j].db_id ); 
    return ( wc < 0 || wc != g_books[j].word_count );
}

//=== FUNCTION ================================================================
//         Name: profile_timed_op
//  Description: Repeatedly performs one of the three operations defined above
//               and maintains values for average execution time, number of
//               errors, etc.
//=============================================================================
static void
profile_timed_op ( struct profile *p, int (*op)(struct profile *p, int i),
        double *total_time, double *avg_time, double *var_time, int *errors,
        int *total_ops, size_t iterations ) {
    double start, end, t1, t2;
    
    *avg_time = *var_time = *errors = 0;

    start = get_time();
    for ( *total_ops=0; *total_ops<iterations; (*total_ops)++ ) {       
        t1 = get_time();
        *errors += op(p,*total_ops);
        t2 = get_time();
        // welford's algorithm for computing running variance and average
        welford ( t2-t1, *total_ops, avg_time, var_time );
    }
    end = get_time();
    *total_time = end-start;
}

//=== FUNCTION ================================================================
//         Name: profiler_init
//  Description: Initialize any global data that is required by the profiler
//=============================================================================
int
profiler_init ( void ) {
    srand(time(NULL));

    // Generate our random collection of books. Make the size of the collection
    // too, just so we aren't tempted to solve for only one collection size.
    g_collection_size = box_muller( MU_COLLECTION, SIGMA_COLLECTION );
    g_books = malloc( sizeof(struct book)*g_collection_size );
    if (!g_books) {
        return 0;
    }

    printf("Generating %lu books... ", g_collection_size);
    fflush(stdout);
    for ( size_t i=0; i<g_collection_size; i++ ) {
        generate_book( &g_books[i] );
    }
    printf("OK\n");

    return 1;
}

//=== FUNCTION ================================================================
//         Name: profiler_run
//  Description: Profile a database
//=============================================================================
void
profiler_run ( struct profile *p ) {
    // profile insertion time
    profile_timed_op(
        p,
        insertion,
        &p->total_insert_time,
        &p->avg_insert_time,
        &p->var_insert_time,
        &p->insert_errors,
        &p->total_insert,
        g_collection_size
    );

    // profile title search time across 10% of inserted documents
    profile_timed_op(
        p,
        search_title,
        &p->total_search_time_title,
        &p->avg_search_time_title,
        &p->var_search_time_title,
        &p->search_title_errors,
        &p->total_search_title,
        g_collection_size/10
    );

    // profile word count search time across 10% of inserted documents
    profile_timed_op(
        p,
        search_word_count,
        &p->total_search_time_word_count,
        &p->avg_search_time_word_count,
        &p->var_search_time_word_count,
        &p->search_word_count_errors,
        &p->total_search_word_count,
        g_collection_size/10
    );
}

//=== FUNCTION ================================================================
//         Name: profiler_quit
//  Description: Free any memory that was allocated to the profiler
//=============================================================================
void
profiler_quit ( void ) {
    if (g_books) {
        free(g_books);
    }
}