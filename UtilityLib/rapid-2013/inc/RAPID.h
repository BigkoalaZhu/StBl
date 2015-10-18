#pragma once
#include "RAPID_model.h"

struct box;
struct tri;

// Find all pairwise intersecting triangles
const int RAPID_ALL_CONTACTS = 1;
// Just report one intersecting triangle pair, if there are any.
const int RAPID_FIRST_CONTACT = 2;

// this is for the client
struct collision_pair
{
	int id1;
	int id2;
};

class RAPID
{
public:
	RAPID(void);
	~RAPID(void);

public:

	// This is the collision query invocation.  It assumes that the 
	// models are not being scaled up or down, but have their native
	// dimensions.
	int RAPID_Collide(double R1[3][3], double T1[3], RAPID_model *o1,
		double R2[3][3], double T2[3], RAPID_model *o2,
		int flag = RAPID_ALL_CONTACTS);

	// This collision query permits the models to each be scaled by
	// some positive factor (must be greater than 0).
	int RAPID_Collide(double R1[3][3], double T1[3], double s1, RAPID_model *o1,
		double R2[3][3], double T2[3], double s2, RAPID_model *o2,
		int flag = RAPID_ALL_CONTACTS);

	void RAPID_initialize();

private:
	int	tri_contact(box *b1, box *b2);
	int add_collision(int id1, int id2);
	int collide_recursive(box *b1, box *b2, double R[3][3], double T[3], double s);

public:
		int RAPID_num_box_tests;
	int RAPID_num_tri_tests;
	int RAPID_num_cols_alloced;
	int RAPID_num_contacts;
	collision_pair *RAPID_contact;

	double RAPID_mR[3][3];
	double RAPID_mT[3];
	double RAPID_ms;

	int RAPID_first_contact;
	int RAPID_initialized;

	moment *RAPID_moment;
	tri *RAPID_tri;
	box *RAPID_boxes;
	int RAPID_boxes_inited;
};


