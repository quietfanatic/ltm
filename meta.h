
#ifndef HAVE_DEFAULT_METAS
#define HAVE_DEFAULT_METAS


// Character type
#ifndef MChar_t
#define MChar_t char
#endif


// String type
#ifndef MStr_t
#define MStr_t MChar_t* // default: null-delimited string of MChar_t
#endif

#ifndef MStr_at
#define MStr_at(str, pos) ( (str)[pos] )
#endif

#ifndef MStr_begin_at
#define MStr_begin_at(str, pos) ( (pos) == 0 )
#endif

#ifndef MStr_end_at
#define MStr_end_at(str, pos) ( MStr_at((str), (pos)) == 0 )
#endif

#ifndef MStr_after
#define MStr_after(str, pos) ( pos + 1 )
#endif

// Optional: put character into c and increment pos
// Some implementations could do this faster than
// This can be multiple lines
#ifndef MStr_read
#define MStr_read(str, pos, c) \
	(c) = MStr_at((str), (pos)); \
	(pos) = MStr_after((str), (pos));
#endif

// String position type
#ifndef MPos_t
#define MPos_t size_t
#endif



// Size of capture id.  Shorter uses less space, but limits number of captures in one scope to 65536.
#ifndef MCapID_t
#define MCapID_t uint16_t
#endif












#endif

