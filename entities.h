#include "includes.h"

#ifndef _ENTITIES_H
#define _ENTITIES_H


class Entitie
{
	int ent_id, gid;
	float ent_space; //(EID    GID    SPACE) 
	public:
		Entitie();
		~Entitie();
		Entitie(int e_id, int e_gid, float e_space);
		int get_eid();
		int get_gid();
		float get_space();
		void set_ent(int e_id, int e_gid, float e_space);
		void show();
};

/************************* Constructor **********************/
Entitie::Entitie(){
	//cout << "Objeto de entidad creado" << endl;
}

/************************* Destructor **********************/
Entitie::~Entitie(){
	//cout << "Objeto de entidad eliminado" << endl;
}

/************************* Constructor **********************
Input
-e_id: identificador de la entidad
-e_gid: grupo al que pertenece la entiadad
-e_space: espacio que requiere la entidad 
************************************************************/
Entitie::Entitie(int e_id, int e_gid, float e_space){
	//cout << e_id << " " << e_gid << " " << e_space << endl;
	ent_id = e_id;
	gid = e_gid;
	ent_space = e_space;
}

/************************* Devuelve el identificador de la entidad **********************/
int Entitie::get_eid(){
	return ent_id;
}

/************************* Modifica el identificador de la entidad **********************/
int Entitie::get_gid(){
	return gid;
}

/************************* Modifica el espacio requerido por la entidad **********************/
float Entitie::get_space(){
	return ent_space;
}

/************************* Modifica las propiedades de la entidad **********************/
void Entitie::set_ent(int e_id, int e_gid, float e_space){
	ent_id = e_id;
	gid = e_gid;
	ent_space = e_space;
}

/************************* miestra iformacion de la entidad **********************/
void Entitie::show(){
	cout << "Entidad " << ent_id << " perteneciente al grupo " << gid << ", y requiere un espacio de " << ent_space << endl;
}


#endif