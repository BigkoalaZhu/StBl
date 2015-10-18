#include "Detect_collision.h"
#include <windows.h>
#include <stdio.h>

DetectCollision::DetectCollision(QVector<SurfaceMeshModel *> components)
{
	rapid = new RAPID();

	mdl.resize(components.size());
	int objcount = 0;
	for each (SurfaceMeshModel * smm in components)
	{
		mdl[objcount] = new RAPID_model(rapid);
		SurfaceMeshModel * new_smm = smm;
		components_copied.push_back(new_smm);
		Surface_mesh::Vertex_property<Vector3> points = smm->vertex_property<Vector3>("v:point");
		int fcount = 0;
		for each (Face f in smm->faces())
		{
			double p[3][3];
			int tcount = 0;
			Surface_mesh::Vertex_around_face_circulator fvit, fvend;
			fvit=fvend=smm->vertices(f);
			do
			{
				p[tcount][0] = points[fvit].x();
				p[tcount][1] = points[fvit].y();
				p[tcount][2] = points[fvit].z();
				tcount++;
			}while(++fvit != fvend);
			mdl[objcount]->AddTri(p[0],p[1],p[2],fcount);
			fcount++;
		}
		objcount++;
	}
	for(int i = 0;i<mdl.size();i++)
	{
		mdl[i]->EndModel();
	}
	contact_faces.resize(mdl.size());
}

DetectCollision::~DetectCollision()
{
	for (auto m:mdl)
	{
		if (m)
		{
			delete m;
		}
	}
	mdl.clear();

	if (rapid)
	{
		delete rapid;
	}

}

bool DetectCollision::DoDetect(int a,int b)
{
	double R[3][3],T1[3],T2[3];
	R[0][0] = R[1][1] = R[2][2] = 1.0;
	R[0][1] = R[1][0] = R[2][0] = 0.0;
	R[0][2] = R[1][2] = R[2][1] = 0.0;
	T1[0] = 0.0;  T1[1] = 0.0; T1[2] = 0.0;
	T2[0] = 0.0;  T2[1] = 0.0; T2[2] = 0.0;

	//struct collision_pair *RAPID_contact = NULL;
	//int RAPID_num_contacts = 0;
	//RAPID_Collide(RAPID_contact, RAPID_num_contacts, R,T1,mdl[a],R,T2,mdl[b],RAPID_ALL_CONTACTS);
	rapid->RAPID_Collide(R,T1,mdl[a],R,T2,mdl[b],RAPID_ALL_CONTACTS);
	for(int i=0; i<rapid->RAPID_num_contacts; i++)
	{
		if(std::find(contact_faces[a].begin(),contact_faces[a].end(),rapid->RAPID_contact[i].id1)==contact_faces[a].end())
			contact_faces[a].push_back(rapid->RAPID_contact[i].id1);
		if(std::find(contact_faces[b].begin(),contact_faces[b].end(),rapid->RAPID_contact[i].id2)==contact_faces[b].end())
			contact_faces[b].push_back(rapid->RAPID_contact[i].id2);
	}
	if(rapid->RAPID_num_contacts>0)
		return true;
	else
		return false;
}
std::vector<int> DetectCollision::Get_contact_faces(int index)
{
	std::vector<int> tmp;
	tmp = contact_faces[index];
	return tmp;
}