#include "includes.h"

#ifndef _ROOMS_H
#define _ROOMS_H


class Room
{
	int room_id, floor_id, i; //(RID  FID   SPACE    ADJ_LIST)
	float room_space;
	vector<int> room_adj;
	public:
		Room();
		~Room();
		Room(int r_id, int f_id, float r_space, vector<int> r_adj);
		int get_rid();
		int get_fid();
		float get_rspace();
		Room find(int rid);
		vector<int> get_radj();
		void set_room(int r_id, int f_id, float r_space, vector<int> r_adj);
		void show();
};

Room::Room(){
	//cout << "Objeto de habitación creado" << endl;
}

Room::~Room(){
	//cout << "Objeto de habitación eliminado" << endl;
}

Room::Room(int r_id, int f_id, float r_space, vector<int> r_adj){
	//cout << e_id << " " << e_gid << " " << e_space << endl;
	room_id = r_id;
	floor_id = f_id;
	room_space = r_space;
	room_adj = r_adj;
}

int Room::get_rid(){
	return room_id;
}

int Room::get_fid(){
	return floor_id;
}

float Room::get_rspace(){
	return room_space;
}

vector<int> Room::get_radj(){
	return room_adj;
}

void Room::set_room(int r_id, int f_id, float r_space, vector<int> r_adj){
	room_id = r_id;
	floor_id = f_id;
	room_space = r_space;
	room_adj = r_adj;
}

void Room::show(){
	cout << "Habitación " << room_id << " en el piso " << floor_id << ", con espacio " << room_space << " y habitaciones adyacentes: ";
	for (i = 0; i < room_adj.size(); i++)
    	cout << room_adj[i] << ' ';
    cout << endl;
}


#endif