#pragma once
#include "moments.h"

const int RAPID_BUILD_STATE_CONST = 0;     // "empty" state, after constructor
const int RAPID_BUILD_STATE_BEGIN = 1;     // after BeginModel()
const int RAPID_BUILD_STATE_ADDTRI = 2;    // after AddTri()
const int RAPID_BUILD_STATE_PROCESSED = 3; // after EndModel()

const int RAPID_OK = 0; 
// Used by all API routines except constructors and destructors.


const int RAPID_ERR_MODEL_OUT_OF_MEMORY = 1; 
// Returned when an API function cannot obtain enough memory to store
// or process a RAPID_model object.


const int RAPID_ERR_COLLIDE_OUT_OF_MEMORY = 2;
// Returned when RAPID_Collide() cannot allocate enough storage to hold
// collision information.  In this case, there is as much collision
// detection information available as could be allocated for.


const int RAPID_ERR_UNPROCESSED_MODEL = 3;
// Returned when an unprocessed model is passed to a function which
// expects only processed models, such as RAPID_Collide().


const int RAPID_ERR_BUILD_OUT_OF_SEQUENCE = 4;
// Returned when: 
//       1. AddTri() is called before BeginModel().  The triangle will 
//          be added anyway as if BeginModel() had been previously called.
//       2. BeginModel() is called immediately after AddTri().  The
//          model will be placed into an empty initial state.  
// This error code is something like a warning: the invoked
// operation takes place anyway, but the returned error code may tip
// off the client that something out of the ordinary is happenning.


const int RAPID_ERR_BUILD_EMPTY_MODEL = 5; 
// Returned when EndModel() is called on a model to which no
// triangles have been added.  This is similar in spirit to the
// OUT_OF_SEQUENCE return code, except that the requested operation
// has FAILED -- the model remains "unprocessed".

class RAPID;

class RAPID_model
{
public:
	// These are everything the client needs to use RAPID.

	RAPID_model(RAPID* r);
	~RAPID_model();

	int build_hierarchy();
	int BeginModel();
	int AddTri(const double *p1, const double *p2, const double *p3, int id);
	int EndModel();

public:
	box *b;
	int num_boxes_alloced;

	tri *tris;
	int num_tris;

	int num_tris_alloced;

	int build_state;

	RAPID* rapid;
};

