#include "includes.h"

#ifndef _CONSTRAINTS_H
#define _CONSTRAINTS_H


class Constraint
{
	int c_id, c_type, c_1, c_2; //(CID CTYPE SorH C1 C2)
	bool sorh;
	public:
		Constraint();
		~Constraint();
		Constraint(int cons_id, int cons_type, bool s_or_h, int cons_1, int cons_2);
		int get_cid();
		int get_ctype();
		int get_c1();
		int get_c2();
		bool get_sorh();
		void set_cons(int cons_id, int cons_type, bool s_or_h, int cons_1, int cons_2);
		void show();
};

Constraint::Constraint(){
	//cout << "Objeto de restriccion creado" << endl;
}

Constraint::~Constraint(){
	//cout << "Objeto de restriccion eliminado" << endl;
}

Constraint::Constraint(int cons_id, int cons_type, bool s_or_h, int cons_1, int cons_2){
	//cout << e_id << " " << e_gid << " " << e_space << endl;
	c_id = cons_id;
	c_type = cons_type;
	c_1 = cons_1;
	c_2 = cons_2;
	sorh = s_or_h;
}

int Constraint::get_cid(){
	return c_id;
}

int Constraint::get_ctype(){
	return c_type;
}

int Constraint::get_c1(){
	return c_1;
}

int Constraint::get_c2(){
	return c_2;
}

bool Constraint::get_sorh(){
	return sorh;
}

void Constraint::set_cons(int cons_id, int cons_type, bool s_or_h, int cons_1, int cons_2){
	c_id = cons_id;
	c_type = cons_type;
	c_1 = cons_1;
	c_2 = cons_2;
	sorh = s_or_h;
}

void Constraint::show(){
	cout << "Restricción " << c_id << " de tipo " << c_type << ", con parámetros " << c_1 << " " << c_2;
	if(sorh) //true=1
		cout << " dura" << endl;
	else
		cout << " blanda" << endl;
}


#endif