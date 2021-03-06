#include "includes.h"
#include "constraint.h"
#include "rooms.h"
#include "entities.h"

#ifndef _INDIVIDUAL_H
#define _INDIVIDUAL_H


class Individual
{
	float aptitud;
	vector<int> rep;
	vector<float> space_rooms;
	public:
		Individual();
		~Individual();
		float get_apt();
		void set_apt(float apt);
		vector<int>  get_ind();
		vector<float>  get_space_rooms();
		vector<float>  set_space_rooms(vector<Room> rooms, vector<Entitie> entities);
		void init(int seed, vector<Entitie> entities_space, vector<float> rooms_space, vector<vector<Constraint>> constraint_per_entitie, vector<Room> rooms, vector<vector<Constraint>> ezpeciales);
		void clear();
		void set_ind(vector<int> representacion);
		void show();
};

void Individual::clear(){
	rep.assign(rep.size(), -1);
}

/* ***********Inicializacion de inviduos**********************
Input: 
-seed: semilla del tiempo para que los numeros aleatorioa sean diferentes en cada instaciacion
-entities_space: entidades almacenadas segun espacio que necesitan
-rooms_space: tamaño que tiene cada habitacion
-constraint_per_entitie: restricciones tipo 3, 4, 5, 7 , 8 y 9 almacenadas para cada entidad relacionada
-rooms: todas las habitaciones
-ezpeciales: restricciones tipo 0, 1 y 6 almacenadas para cada entidad relacionada
Output:
Representación de individuo que puede ser factible o no factible.
**************************************************************/
void Individual::init(int seed, vector<Entitie> entities_space, vector<float> rooms_space, vector<vector<Constraint>> constraint_per_entitie, vector<Room> rooms, vector<vector<Constraint>> ezpeciales){
	int i, j, r, type, ent, cumplida, c1, c2, habitacion, cont_adj;
	vector<int> representacion;
	vector<int> not_sharing;
	vector<int> list_adj, representacion_init;
	vector<float> rooms_space_inicial;
	vector<int> revisados(rooms.size(), -1);
	representacion.assign(entities_space.size(), -1);
	bool is_nonsharing = false, reinicio = false;


  	srand (time(NULL) + 100000*seed); // initialize random seed
	rooms_space_inicial = rooms_space;
	/*Primero revisar los ezpeciales que tienen las restricciones de no share, no allocation y allocation*/
	i = 0;
	while( (unsigned)i < ezpeciales.size()){
		r = (rand() % rooms.size());
		cumplida = 0;
		is_nonsharing = false;
		reinicio = false;
		for(j = 0; (unsigned)j < ezpeciales[i].size(); j++){
			type = ezpeciales[i][j].get_ctype();
			switch(type){
				case 0: //Allocation C1->entidad C2->habitacion 
					c2 = ezpeciales[i][j].get_c2();  //habitacion
					c1 = ezpeciales[i][j].get_c1();	//entidad 
					r = c2;
					cumplida++;
					if((find(representacion.begin(), representacion.end(), r) != representacion.end())){
						i = 0;
						reinicio = true;
						representacion.assign(entities_space.size(), -1);
						not_sharing.clear();
						rooms_space = rooms_space_inicial;
						cumplida--;
					}
				break;
				case 1: //Non allocation C1->entidad C2->habitacion
					c2 = ezpeciales[i][j].get_c2();  //habitacion
					c1 = ezpeciales[i][j].get_c1();	//entidad 
					if(r != c2){
						cumplida++;
					}
				break;
				case 6: //Non sharing C1->entidad
					c1 = ezpeciales[i][j].get_c1();  ///entidad 
					if(!(find(representacion.begin(), representacion.end(), r) != representacion.end())){
						cumplida++;
						is_nonsharing = true;
					}
				break;
			}
		}
		if((unsigned)cumplida == ezpeciales[i].size() and ezpeciales[i].size() != 0 and !reinicio){
			representacion[i] = r;
			for (int k = 0; (unsigned)k < entities_space.size(); k++)
			{
				if(entities_space[k].get_eid() == i){
					rooms_space[r] = rooms_space[r] - entities_space[k].get_space();
				}
			}			
			i++;
			if(is_nonsharing)
				not_sharing.push_back(r); //solo guardo los que tiene restricciones de no compartit
		}
		if(ezpeciales[i].size() == 0)
			i++;
	}
	rooms_space_inicial = rooms_space;
	representacion_init = representacion;
	i = 0; //iterador de entidades entities_space
	while((unsigned)i < entities_space.size())
	{
		r = (rand() % rooms.size());
		while((find(not_sharing.begin(), not_sharing.end(), r) != not_sharing.end())){
			r = (rand() % rooms.size()); //elegir random el id de la habitación en que puede estar la entidad
		}
		ent = entities_space[i].get_eid();
		cumplida = 0;
		if(representacion[ent] == -1){
			reinicio = false;
			for (j = 0; (unsigned)j < constraint_per_entitie[ent].size(); j++)
			{
				type = constraint_per_entitie[ent][j].get_ctype();
				switch(type){
					case 0: /*Allocation C1->entidad C2->habitación ya se reviso */break;
					case 1: /*Non allocation C1->entidad  C2->habitación ya se reviso*/break;
					case 2: /*one_of no se usa */	break;
					case 3: /*Capacidad no la reviso nunca aparecera */	break; 
					case 4: //Same room
						c1 = constraint_per_entitie[ent][j].get_c1();  //entidad 1
						c2 = constraint_per_entitie[ent][j].get_c2();	//entidad 2
						if(ent == c1){
							if(r == representacion[c2] or representacion[c2] == -1){
								cumplida++;
							}
						}
						else{
							if(r == representacion[c1]  or representacion[c1] == -1){
								cumplida++;
							}
						}
					break;
					case 5: //Not same room 
						c1 = constraint_per_entitie[ent][j].get_c1();  //entidad 1
						c2 = constraint_per_entitie[ent][j].get_c2();	//entidad 2
						if(ent == c1){
							if(r != representacion[c2]){
								cumplida++;
							}
						}
						else{
							if(r != representacion[c1]){
								cumplida++;
							}
						}
					break;
					case 6:  //Not sharing ya se reviso
					break;
					case 7:  //Adjacency  REVISAR BIEN
						c1 = constraint_per_entitie[ent][j].get_c1();  //entidad 1
						c2 = constraint_per_entitie[ent][j].get_c2();	//entidad 2
						if(ent == c1){
							if(representacion[c2] == -1){ //la otra entidad no esta asignada
								cumplida++;
							}
							else{ //ya esta asignada  
								habitacion = representacion[c2];
								list_adj = rooms[habitacion].get_radj();
								if(find(list_adj.begin(), list_adj.end(), r) != list_adj.end()){//ver si r pertenece a la lista de adjacencia
									cumplida++;
								}
								else{
									reinicio = true;
								}
								cont_adj = 0;
								for (int l = 0; (unsigned)l < list_adj.size(); l++)
								{
									if((find(not_sharing.begin(), not_sharing.end(), list_adj[l]) != not_sharing.end())){ //si algun elemento de la lista de adyacencia no esta restringido por not_sharing
										cont_adj++;
									}
								}
								if((unsigned)cont_adj == list_adj.size()){ //todos los de la lista de adyacencia no pueden ser asignados
									reinicio = true;
								}
							}
						}
						else{
							if(representacion[c1] == -1){ //la otra entidad no esta asignada
								cumplida++;
							}
							else{ //ya esta asignada
								habitacion = representacion[c1];
								list_adj = rooms[habitacion].get_radj();
								if(find(list_adj.begin(), list_adj.end(), r) != list_adj.end()){//ver si r pertenece a la lista de adjacencia
									cumplida++;
								}
								else{
									reinicio = true;
								}
								cont_adj = 0;
								for (int l = 0; (unsigned)l < list_adj.size(); l++)
								{
									if((find(not_sharing.begin(), not_sharing.end(), list_adj[l]) != not_sharing.end())){ //si algun elemento de la lista de adyacencia no esta restringido por not_sharing
										cont_adj++;
									}
								}
								if((unsigned)cont_adj == list_adj.size()){ //todos los de la lista de adyacencia no pueden ser asignados
									reinicio = true;									
								}
							}
						}
					break;
					case 8:  //Nearby  (same floor)
						c1 = constraint_per_entitie[ent][j].get_c1();  //entidad 1
						c2 = constraint_per_entitie[ent][j].get_c2();	//entidad 2
						if(ent == c1){
							if(representacion[c2] == -1){ //la otra entidad no esta asignada
								cumplida++;
							}
							else{ //ya esta asignada  
								habitacion = representacion[c2];
								if(rooms[habitacion].get_fid() == rooms[r].get_fid()){
									cumplida++;
								}
							}
						}
						else{
							if(representacion[c1] == -1){ //la otra entidad no esta asignada
								cumplida++;
							}
							else{ //ya esta asignada
								habitacion = representacion[c1];
								if(rooms[habitacion].get_fid() == rooms[r].get_fid()){
									cumplida++;
								}
							}
						}
					break;
					case 9:  //AwayFrom  (not same floor)
						c1 = constraint_per_entitie[ent][j].get_c1();  //entidad 1
						c2 = constraint_per_entitie[ent][j].get_c2();	//entidad 2
						if(ent == c1){
							if(representacion[c2] == -1){ //la otra entidad no esta asignada
								cumplida++;
							}
							else{ //ya esta asignada  
								habitacion = representacion[c2];
								if(rooms[habitacion].get_fid() != rooms[r].get_fid()){
									cumplida++;
								}
							}
						}
						else{
							if(representacion[c1] == -1){ //la otra entidad no esta asignada
								cumplida++;
							}
							else{ //ya esta asignada
								habitacion = representacion[c1];
								if(rooms[habitacion].get_fid() != rooms[r].get_fid()){
									cumplida++;
								}
							}
						}
					break;
					default:
					break;
				}
			}
			if((unsigned)cumplida == constraint_per_entitie[ent].size()){
				representacion[ent] = r;
				for (int k = 0; (unsigned)k < entities_space.size(); k++)
				{
					if(entities_space[k].get_eid() == ent){
						rooms_space[r] = rooms_space[r] - entities_space[k].get_space();
					}
				}
				i++;
			}
			if(reinicio){
				i = 0;
				representacion = representacion_init;
				rooms_space = rooms_space_inicial;
			}
		}
		else{
			i++;
		}
	}
	rep = representacion;
	/*for (i = 0; i < representacion.size(); i++)
	{
		cout << representacion[i] << " ";
	}
	cout << endl;*/

	space_rooms = rooms_space;
}

/************************* Constructor **********************/
Individual::Individual(){
	//cout << "Objeto de individuo creado" << endl;
//	cout << CANT_TYPES_CONS;
//	cout << endl;
}

/************************* Destructor **********************/
Individual::~Individual(){
	//cout << "Objeto de individuo eliminado" << endl;
}

/************************* Cambia la funcion de evaluacion de un individuo **********************/
void Individual::set_apt(float apt){
	aptitud = apt;
}

/************************* Devuelve la funcion de evaluacion de un individuo **********************/
float Individual::get_apt(){
	return aptitud;
}

/************************* Devuelve la representacion de un individuo **********************/
vector<int>  Individual::get_ind(){
	return rep;
}

/************************* Devuelve espacio que queda en habitaciones**********************/
vector<float>  Individual::get_space_rooms(){
	return space_rooms;
}

/* ***********Calculo de espacio que queda en habitaciones**********************
Input: 
-rooms: todas las habitaciones
-entities: todas las entidades
Output:
Vector con el espacio que queda disponible en la habitacón i. 
Si es positivo es espacio mal utilizado, si es negativo es espacio sobre utilizado.
**************************************************************/
vector<float>  Individual::set_space_rooms(vector<Room> rooms, vector<Entitie> entities){
	int i;
	vector<float> rooms_space;
	for (i = 0; (unsigned)i < rooms.size(); i++)
	{
		rooms_space.push_back(rooms[i].get_rspace());
	}
	for (i = 0; (unsigned)i < rep.size(); i++)
	{
		rooms_space[rep[i]] -= entities[i].get_space();
	}

	space_rooms = rooms_space;
	return space_rooms;
}

/************************* Modifica la representacion de un individuo **********************/
void Individual::set_ind(vector<int> representacion){
	rep = representacion;
}

/************************* Muestra información de un individuo **********************/
void Individual::show(){
	for (int i = 0; (unsigned)i < rep.size(); i++)
	{
		cout << rep[i] << " ";
	}
	cout << endl;
	cout << "El individio tienen una funcion de evaluacion: " << aptitud << endl;
}


#endif