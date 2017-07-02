#include "entities.h"
#include "constraint.h"
#include "rooms.h"
#include "individuals.h"


/************************** Variables globales *************************************/
int params[6];
vector<Entitie> entities;
vector<Room> rooms;
vector<Constraint> cons_soft, cons_hard;
vector<Individual> poblation;


/************************** Declaracion de funciones *************************************/
void leer_instancia(string path);
bool allocation(Room r, Entitie e, vector<vector<int>> i);
bool non_allocation(Room r, Entitie e, vector<vector<int>> i);
bool capacity(Room r, vector<vector<int>> i);
bool same_room(Entitie e1, Entitie e2, vector<vector<int>> i);
bool not_same_room(Entitie e1, Entitie e2, vector<vector<int>> i);
bool not_sharing(Entitie e, vector<vector<int>> i);
bool adjacency(Entitie e1, Entitie e2, vector<vector<int>> i);
bool nearby(Entitie e1, Entitie e2, vector<vector<int>> i);
bool awayfrom(Entitie e1, Entitie e2, vector<vector<int>> i);
bool feasible(Individual ind);
bool entitie_one_room(vector<vector<int>> i);
float fun_eval(Individual ind);
Individual elite(vector<Individual> pob);
vector<Individual> ranking(vector<Individual> pob, int cant_padres);
vector<vector<int>> cross_one_point(vector<Individual> padres, float pc); //pc = probabilidad de cruzamiento Aun no osn reales individuos de la siguiente generacion
vector<vector<int>> mutation(vector<vector<int>> crossover, float pm, vector<vector<Constraint>> ezpeciales);//Mutacion
void output(Individual ind, string name);


/************************** Implemetacion de OSAP mediante AG ****************************
Input: vector argv, que cada casilla indica:
-argv[1]: instancia 
-argv[2]: tamaño de la poblacion 
-argv[3]: cantidad de generaciones a crear, es decir, criterio de termino que indica la cantidad de iteraciones
-argv[4]: probabilidad de cruzamiento
-argv[5]: probabilidad de mutacion
****************************************************************************************/
int main(int argc, char* argv[]) {
  srand (time(NULL));
	cout << to_string(argc) << endl;
	cout << argv[0] << " " << argv[1] << endl;

  int i, j, seed, generacion, tam_poblacion, cant_gen, tipos, m;
  float f_e, pc, pm;
  vector<float> rooms_space;
  vector<Entitie> entities_space;  //construir para pasarlo al creador de individuos
  vector<vector<Constraint>> constraint_per_entitie;//construir para pasarlo al creador de individuos
  vector<vector<Constraint>> ezpeciales;
  Individual ind;
  Individual elite_best, elite_now;
  vector<Individual> select;
  vector<vector<int>> crossover, mutacion;
  string salida, instancia;


  instancia = argv[1];
  tam_poblacion = atoi(argv[2]);
  cant_gen = atoi(argv[3]);
  pc = atof(argv[4]);
  pm = atof(argv[5]);

  leer_instancia(instancia);
  for (i = 0; (unsigned)i < rooms.size(); i++)
  {
    rooms_space.push_back(rooms[i].get_rspace());
  }

  entities_space = entities; //ordena entidades segun espacio requerido, de la entidad que necesita mas a la que necesita menos
  sort(entities_space.begin(), entities_space.end(), [] (Entitie a, Entitie b) { return a.get_space() > b.get_space(); }); 

  for (i = 0; (unsigned)i < entities.size(); i++) //relacionar entidades con sus restricciones 
  {
    vector<Constraint> cons_of_ent;
    for (j = 0; (unsigned)j < cons_hard.size(); j++)
    {
      if(cons_hard[j].get_ctype() == 6){  // restriccion non_sharing, entidad sola
        if(cons_hard[j].get_c1() == entities[i].get_eid()){
          cons_of_ent.push_back(cons_hard[j]);
        }
      }
      if(cons_hard[j].get_ctype() == 0 or cons_hard[j].get_ctype() == 1){ //restriccion allocation y non allocation, primer elemento es la entidadad
        if(cons_hard[j].get_c1() == entities[i].get_eid()){
          cons_of_ent.push_back(cons_hard[j]);
        }
      }
    }
    ezpeciales.push_back(cons_of_ent);
  }
  
  for (i = 0; (unsigned)i < entities.size(); i++) //relacionar entidades con sus restricciones 
  {
    vector<Constraint> cons_of_ent;
    for (j = 0; (unsigned)j < cons_hard.size(); j++)
    {
      if(cons_hard[j].get_ctype() == 4 or cons_hard[j].get_ctype() == 5 or cons_hard[j].get_ctype() == 7 or cons_hard[j].get_ctype() == 8 or cons_hard[j].get_ctype() == 9){
        if(cons_hard[j].get_c1() == entities[i].get_eid() or cons_hard[j].get_c2() == entities[i].get_eid()){
          cons_of_ent.push_back(cons_hard[j]);
        }
      }
    }
    constraint_per_entitie.push_back(cons_of_ent);
  }
  /* 1) Generar población!! */
  i=0;
  for (i = 0; i < tam_poblacion; i++)
  {
    j = 0;
    seed = (rand() % 1234567);
    ind.init(seed, entities_space, rooms_space, constraint_per_entitie, rooms, ezpeciales);
    while(!feasible(ind)){
      ind.init(seed, entities_space, rooms_space, constraint_per_entitie, rooms, ezpeciales);
      if(feasible(ind))
        cout << "individuo factible " << endl;
      else 
        cout << "individuo NO factible " << endl;
      j++;
    }
    cout << "despues de " << j+1 << " aparecio uno factible" << endl;
    //ind.show();
    poblation.push_back(ind);
  }

  for (generacion = 0; generacion < cant_gen; generacion++)
  {
    /* 2) Evaluacion de poblacion*/
    f_e = 0;
    cout << "poblacion: " << endl;
    for (i = 0; (unsigned)i < poblation.size(); i++)
    {
      f_e = fun_eval(poblation[i]);
      poblation[i].set_apt(f_e);
      poblation[i].show();
    //  poblation[i].get_space_rooms();
    //  poblation[i].set_space_rooms(rooms, entities);
    }

    /* 3) Paso de elitismo*/
    if(generacion == 0){
      elite_best = elite(poblation);
      salida = instancia + "inicial" ; //Eliminar Inicial
      output(elite_best, salida);
    }
    else{
      elite_now = elite(poblation);
      if(elite_now.get_apt() < elite_best.get_apt()){ //el mejor de la generacion actual es el mejor mejor xd
        elite_best = elite_now;
      }
      salida = instancia + "_" + to_string(generacion); //eliminar desde el _
      output(elite_best, salida);
      elite_best.show();
    }
    cout << "Función de evaluacion del mejor " << elite_best.get_apt() << endl;

    /* 4) Selcción por ruleta~*/
    select = ranking(poblation, 4);
    cout << "seleccion de padres " << endl;
    for (i = 0; (unsigned)i < select.size(); i++)
    {
      select[i].show();
    }

    poblation.clear();
    tipos = 0;
    
    while(tipos < tam_poblacion){
      /* 5) CrossOver One Point*/
      crossover = cross_one_point(select, pc);

      for (i = 0; (unsigned)i < crossover.size(); i++)
      {
        cout << "CrossOver " << i << ": ";
        for (j = 0; (unsigned)j < crossover[i].size(); j++)
        {
          cout << crossover[i][j] << " ";
        }
        cout << endl;
      }
      /* 6) Mutacion, gen a gen segun pm(probabilidad de mutacion), se revisan 3 tipos de restricciones para que no quede la embarrada con la solucion*/
      mutacion = mutation(crossover, pm, ezpeciales);
      for (m = 0; (unsigned)m < mutacion.size(); m++)
      {     
        Individual prueba;

        prueba.set_ind(mutacion[m]);

        if(feasible(prueba)){
          cout << "individuo factible " << endl;
          float ev = fun_eval(prueba);
          prueba.set_apt(ev);
          prueba.show();
          poblation.push_back(prueba);
          tipos++;
        }
        else 
          cout << "individuo NO factible " << endl;
      }
    }
  }


 	return 0;
}

/************************** Almacenamiento de instancia ****************************
Input: 
-path: ruta de la instancia

Almacena un vector de objeto para cada componente: entidad, habitacion y restriccion (blandas y duras por separado)
****************************************************************************************/
void leer_instancia(string path){  //Se lee la instancia y crea vetor de entidades habitaciones restricciones duras y blandas
  Entitie e;
  vector<int> list_adj; //lista de adyacencia de habitaciones
  Room r; 
  Constraint c;

  string word;
  int val1, val2, val4, val5;
  float val3;
  int i, j; //#entidades [0], #habitaciones[1], #pisos[2], #restricciones total[3], #rest. duras [4] y #rest. blandas[5]

  ifstream instance (path);
  if (instance.is_open())
  {
    for(i = 0; i < 6 ; i++ ) //guardar params
    {
      instance >> word >> params[i];
    }

    while(instance){
      instance >> word;
      cout << word << endl;
      if( word == "ENTITIES"){
        for (i = 0; i < params[0]; ++i)
        {
          instance >> val1  >> val2 >> val3;
          e.set_ent(val1, val2, val3);
          entities.push_back(e);
          //cout << a << " " << b << " " << d << endl;
        }

      /*  for (i = 0; i < params[0]; ++i)
        {
          entities[i].show();
        }
        cout << "Entidades almacenadas exitosamente" << endl;*/
      }
      else if( word == "ROOMS"){
        i = 0;
        while(i < params[1]){
          list_adj.clear();
          instance >> val1;
          instance >> val2;
          instance >> val3;
          instance >> val4;
          for(j = 0; j < val4; j++){
            instance >> val5;
            list_adj.push_back( val5);
          }
          r.set_room(val1, val2, val3, list_adj);
          rooms.push_back(r);
          i++;
        }
      /*  for (i = 0; i < params[1]; ++i)
        {
          rooms[i].show();
        }
        cout << "Habitaciones almacenadas exitosamente" << endl;*/
      }
      else if( word == "CONSTRAINTS"){
        for (i = 0; i < params[3]; ++i)
        {
          instance >> val1  >> val2 >> val3 >> val4 >> val5;
          c.set_cons(val1, val2, val3, val4, val5);
          if(val3  == 0)
            cons_soft.push_back(c);
          else
            cons_hard.push_back(c);
          //cout << a << " " << b << " " << d << endl;
        }

      /*  for (i = 0; i < cons_soft.size(); ++i)
        {
          cons_soft[i].show();
        }
        for (i = 0; i < cons_hard.size(); ++i)
        {
          cons_hard[i].show();
        }
        cout << "Restricciones almacenadas exitosamente" << endl;*/
        break; //ordinariooo xd
      }
      else
        cout << "Error en el formato de dataset :c. Revise que este bien escrito: ENTITIES, ROOMS y CONSTRAINTS" << endl; 
    }
    instance.close();
  }
  else 
    cout << "Error al abrir el archivo";
}             

/************************** Verifica el cumplimiento de la restriccion tipo 0 ****************************************/
bool allocation(Room r, Entitie e, vector<vector<int>> i){
  if(i[e.get_eid()][r.get_rid()] == 1)
    return true;
  return false;
}

/************************** Verifica el cumplimiento de la restriccion tipo 1 ****************************************/
bool non_allocation(Room r, Entitie e, vector<vector<int>> i){
  if(i[e.get_eid()][r.get_rid()] == 0)
    return true;
  return false;
}

/************************** Verifica el cumplimiento de la restriccion tipo 3 ****************************************/
bool capacity(Room r, vector<vector<int>> i){
  int j;
  float ocupado = 0;
  for (j = 0; (unsigned)j < i.size(); j++)
  {
    if(i[j][r.get_rid()]){
      ocupado += entities[j].get_space();
    }    
  }
  if(ocupado <= r.get_rspace())
    return true;
  return false;
}

/************************** Verifica el cumplimiento de la restriccion tipo 4 ****************************************/
bool same_room(Entitie e1, Entitie e2, vector<vector<int>> i){
  int j;
  for (j = 0; j < params[1]; j++)
  {
    if(i[e1.get_eid()][j] == 1 and i[e2.get_eid()][j] == 1)
      return true;
  }
  return false;
}

/************************** Verifica el cumplimiento de la restriccion tipo 5 ****************************************/
bool not_same_room(Entitie e1, Entitie e2, vector<vector<int>> i){
  int j, contador = 0;
  for (j = 0; j < params[1]; j++)
  {
    if(i[e1.get_eid()][j] != i[e2.get_eid()][j] or (i[e1.get_eid()][j] == 0 and i[e2.get_eid()][j] == 0))
      contador++;
  }
  if(contador == params[1])
    return true;
  return false;
}

/************************** Verifica el cumplimiento de la restriccion tipo 6 ****************************************/
bool not_sharing(Entitie e, vector<vector<int>> i){ 
  int j = 0, k;
  bool  hab_no_encontrada = true;
  while(j < params[1] and hab_no_encontrada){
    if(i[e.get_eid()][j] == 1){
      hab_no_encontrada = false;
      for(k = e.get_eid()-1; k >= 0; k--){
        if(i[k][j] == 1){
          return false;
        }
      }
      for(k = e.get_eid()+1; (unsigned)k < i.size(); k++){
        if(i[k][j] == 1){
          return false;
        }
      }
    }
    j++;
  } 
  if(hab_no_encontrada) //entidad no asignada a ninguna habitacion :c penita
    return false;
  return true;
}

/************************** Verifica el cumplimiento de la restriccion tipo 7 ****************************************/
bool adjacency(Entitie e1, Entitie e2, vector<vector<int>> i){  //probar se considera misma habitacion??
  int j, hab_e1, hab_e2; 
  vector<int> adj_e1;
  for (j = 0; j < params[1]; j++)
  {
    if(i[e1.get_eid()][j] == 1){
      hab_e1 = j;
    }
    if(i[e2.get_eid()][j] == 1){
      hab_e2 = j;
    }
  }
  adj_e1 = rooms[hab_e1].get_radj();
  for (j = 0; (unsigned)j < adj_e1.size(); j++)
  {
    if(hab_e2 == adj_e1[j])
      return true;
  }
  return false;
}

/************************** Verifica el cumplimiento de la restriccion tipo 8 ****************************************/
bool nearby(Entitie e1, Entitie e2, vector<vector<int>> i){ 
  int j;
  int hab_e1 = -1, hab_e2 = -1; 
  vector<int> adj_e1;
  for (j = 0; (unsigned)j < i[0].size(); j++)
  {
    if(i[e1.get_eid()][j] == 1){
      hab_e1 = j;
    }
    if(i[e2.get_eid()][j] == 1){
      hab_e2 = j;
    }
  }
  if(hab_e1 == -1 or hab_e2 == -1) //alguna de las entidades no esta asignada a ninguna habitacion
    return false;
  if(rooms[hab_e1].get_fid() == rooms[hab_e2].get_fid())
    return true;
  return false;
}

/************************** Verifica el cumplimiento de la restriccion tipo 9 ****************************************/
bool awayfrom(Entitie e1, Entitie e2, vector<vector<int>> i){ 
  int j, hab_e1, hab_e2; 
  vector<int> adj_e1;
  for (j = 0; (unsigned)j < i[0].size(); j++)
  {
    if(i[e1.get_eid()][j] == 1){
      hab_e1 = j;
    }
    if(i[e2.get_eid()][j] == 1){
      hab_e2 = j;
    }
  }
 /* if(hab_e1 == -1 or hab_e2 == -1) //alguna de las entidades no esta asignada a ninguna habitacion
    return false;*/
  if(rooms[hab_e1].get_fid() == rooms[hab_e2].get_fid()){
 //   cout << "hab 1 " << hab_e1 << ": " << rooms[hab_e1].get_fid() << " hab_e2 " << hab_e2 << ": " << rooms[hab_e2].get_fid() << endl;
    return false;
  }
  return true;
}

/************************** Verifica el cumplimiento de la restriccion de cada entidad en solo una habitacion ****************************************/
bool entitie_one_room(vector<vector<int>> i){
  int j, k, contador;
  for (j = 0; (unsigned)j < i.size(); j++)
  {
    contador = 0;
    for (k = 0; (unsigned)k < i[j].size(); k++)
    {
      if(i[j][k] == 1)
        contador++;
    }
    if(contador > 1)
      return false;
  }
  return true;
}


/************************** Verifica la factibilidad de un individuo ****************************************/
bool feasible(Individual ind){ //probar
  int i, j, k, type;
  vector<vector<int>> individuo;
  vector<int> rep = ind.get_ind();
  k = 0;

  individuo.assign(params[0], vector<int> (params[1], 0)); //inicializo matriz con 0
  for (i = 0; (unsigned)i < rep.size(); i++)
  {
    individuo[i][rep[i]] = 1;
  }

  for (j = 0; (unsigned)j < cons_hard.size(); j++) {
    type = cons_hard[j].get_ctype();
  //  cons_hard[j].show();
   // cout<<cons_hard.size()<< endl;
    switch(type){
      case 0: //Allocation C1->entidad C2->habitación
        if(allocation(rooms[cons_hard[j].get_c2()], entities[cons_hard[j].get_c1()], individuo))
          k++;
        else
          cout << "dead allocation " << cons_hard[j].get_cid() << endl;
      break;
      case 1: //Non allocation C1->entidad C2->habitación
        if(non_allocation(rooms[cons_hard[j].get_c2()], entities[cons_hard[j].get_c1()], individuo))
          k++;
        else
          cout << "dead non_allocation " << cons_hard[j].get_cid() << endl;
      break;
      case 2: //one_of no se usa 
      break;
      case 3: //Capacidad
        if(capacity(rooms[cons_hard[j].get_c1()], individuo))
          k++;
        else
          cout << "dead capacity " << cons_hard[j].get_cid() << endl;
      break; 
      case 4: //Same room
        if(same_room(entities[cons_hard[j].get_c1()], entities[cons_hard[j].get_c2()], individuo))
          k++;
        else
          cout << "dead same_room " << cons_hard[j].get_cid() << endl;
      break;
      case 5: //Not same room 
        if(not_same_room(entities[cons_hard[j].get_c1()], entities[cons_hard[j].get_c2()], individuo))
          k++;
        else
          cout << "dead not same room " << cons_hard[j].get_cid() << endl;
      break;
      case 6:  //Not sharing
        if(not_sharing(entities[cons_hard[j].get_c1()], individuo))
          k++;
        else
          cout << "dead nor sharing " << cons_hard[j].get_cid() << endl;
      break;
      case 7:  //Adjacency
        if(adjacency(entities[cons_hard[j].get_c1()], entities[cons_hard[j].get_c2()], individuo))
          k++;
        else
          cout << "dead adjacency " << cons_hard[j].get_cid() << endl;
      break;
      case 8:  //Nearby  (same floor)
        if(nearby(entities[cons_hard[j].get_c1()], entities[cons_hard[j].get_c2()], individuo))
          k++;
        else
          cout << "dead nearby " << cons_hard[j].get_cid() << endl;
      break;
      case 9:  //AwayFrom  (not same floor)
        if(awayfrom(entities[cons_hard[j].get_c1()], entities[cons_hard[j].get_c2()], individuo))
          k++;
        else
          cout << "dead awayfrom " << cons_hard[j].get_cid() << endl;
      break;
      default:
        //cout << "sin restriccion dura" << endl;
      break;
    }
  }
  if((unsigned)k == cons_hard.size())
    return true;
  return false;
}

/************************** Evalua a un individuo *********************
Input:
-ind: individuo a evaluar
Output:
Valor flotante que representa la penalizacion del espacio mal utilizado y restricciones violadas por el individuo
***********************************************************************/
float fun_eval(Individual ind){
  cout << "funcion de evaluacion en proceso" << endl;

  int myints[] = {20,10,0,10,10,10,50,10,10,10};
  vector<float> penalization_per_constraint (myints, myints + sizeof(myints) / sizeof(int) );

  vector<float> space_rooms;
  vector<int> rep = ind.get_ind();
  vector<vector<int>> individuo;
  int i, j, type;
  float overused, wasted, penalization_space, penalization_cons_soft, penalization;
  space_rooms = ind.set_space_rooms(rooms, entities);

  penalization = penalization_space = 0;
  for (i = 0; (unsigned)i < space_rooms.size(); i++)
  {
    overused = wasted = 0;
  //  cout << "Para la habitacion " << i << " el espacio que queda " << space_rooms[i] << endl;
    if(space_rooms[i] < 0){
  //    cout << "habitacion sobre usada" << endl;
      overused = -2*space_rooms[i];
    }
    if(space_rooms[i] > 0){
  //    cout << "habitacion con espacio perdido" << endl;
      wasted = space_rooms[i];
    }
  //  cout << "max entre " << wasted << " y " << overused << endl;
    penalization_space += max(wasted, overused);
  }
//  cout << "penalizacion de espacio " << penalization_space << endl;

/*  cout << "Penalizacion por restriccion blandas" << endl;
  for (i = 0; i < penalization_per_constraint.size(); i++)
  {
    cout << "restriccion blanda tipo " << i << " tiene penalizacion de " << penalization_per_constraint[i] << endl;
  }*/
  //Pasar individuo a matriz
  individuo.assign(params[0], vector<int> (params[1], 0)); //inicializo matriz con 0
  for (i = 0; (unsigned)i < rep.size(); i++)
  {
    individuo[i][rep[i]] = 1;
  }

  penalization_cons_soft = 0;
  for (j = 0; (unsigned)j < cons_soft.size(); j++)
  {
    type = cons_soft[j].get_ctype();
  //  cons_soft[j].show();
    switch(type){
      case 0: //Allocation C1->entidad C2->habitación  
        if(!(allocation(rooms[cons_soft[j].get_c2()], entities[cons_soft[j].get_c1()], individuo))){
      //    cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
      //  else
        //  cout << "restriccion no violada " << endl;
      break;
      case 1: //Non allocation C1->entidad  C2->habitación
        if(!(non_allocation(rooms[cons_soft[j].get_c2()], entities[cons_soft[j].get_c1()], individuo))){
 //         cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
   //     else
     //     cout << "restriccion no violada " << endl;
      break;
      case 2: //one_of no se usa 
      break;
      case 3: //Capacidad
        if(!(capacity(rooms[cons_soft[j].get_c1()], individuo))){
   //       cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
    //    else
      //    cout << "restriccion no violada " << endl;
      break; 
      case 4: //Same room
        if(!(same_room(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
  //        cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
    //    else
      //    cout << "restriccion no violada " << endl;
      break;
      case 5: //Not same room 
        if(!(not_same_room(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
  //        cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
    //    else
      //    cout << "restriccion no violada " << endl;
      break;
      case 6:  //Not sharing
        if(!(not_sharing(entities[cons_soft[j].get_c1()], individuo))){
  //        cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
    //    else
      //    cout << "restriccion no violada " << endl;
      break;
      case 7:  //Adjacency
        if(!(adjacency(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
   //       cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
    //    else
      //    cout << "restriccion no violada " << endl;
      break;
      case 8:  //Nearby  (same floor)
        if(!(nearby(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
  //        cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
    //    else
      //    cout << "restriccion no violada " << endl;
      break;
      case 9:  //AwayFrom  (not same floor)
        if(!(awayfrom(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
  //        cout << "restriccion violada" << endl;
          penalization_cons_soft += penalization_per_constraint[type];
        }
    //    else
      //    cout << "restriccion no violada " << endl;
      break;
      default:
        //cout << "sin restriccion dura" << endl;
      break;
    }
  }
  //cout << "penalizacion por restricciones violadas " << penalization_cons_soft << endl;

  penalization = penalization_space + penalization_cons_soft;
  //cout << "el valor final de la funcion de evaluacion es " << penalization << endl;
  return penalization;
}

/************************** Mejor individuo de la poblacion ****************************************/
Individual elite(vector<Individual> pob){
  Individual best;

  auto res = min_element( pob.begin(), pob.end(), []( Individual a,  Individual b ) { return a.get_apt() < b.get_apt(); } ); 
//  cout<< "y el mejor es: " << endl;
//  res[0].show();
  best = res[0];   
  return best;
}

/************************** Seleccion por ranking de los padres ****************************************/
vector<Individual> ranking(vector<Individual> pob, int cant_padres){
//  cout << "Hola, adios xd, ahora se seleccionan los padres" << endl;
  int i, r, j;
  float total, acumulado, rand_float;
  vector<float> ruleta;
  vector<Individual> sort_asc, padres;

  total = 0.0;
  for (i = 1; (unsigned)i <= pob.size(); i++) //debe partir de 1 para la suma de 1+2+...+n
  {
    total += i;
 //   cout << total << endl;
  }
//  cout << "suma total de los puestos de los individuos es: " << total << endl;

  sort_asc = pob;
  sort(sort_asc.begin(), sort_asc.end(), [] (Individual a, Individual b) { return a.get_apt() > b.get_apt(); }); 


  for (i = 1; (unsigned)i <= sort_asc.size(); i++) /* el que tiene menor aptitus tiene mayor posibilidad de ganar*/
  {
 //   sort_asc[i-1].show();
    ruleta.push_back(i/total);
//    cout << "porcentaje de la ruleta del individuo " << i << " es  (" << total << ") " << ruleta[i-1] << endl;
  }

  srand(time(0));
  acumulado = 1.0;
  r = (rand() % 100);
  rand_float = r/100.0;
  j = 0;
//  cout << "mi random floar " << rand_float << endl;
  for (i = sort_asc.size()-1; i >= 0 and j < cant_padres; i--)
  {
//    cout << "poscion " << i << endl;
  //  sort_asc[i].show();
    acumulado -= ruleta[i];
//    cout << "valor acumulado " << ruleta[i] << " es " << acumulado << endl;
    if(rand_float > acumulado){
 //     cout << rand_float << " vs " << acumulado << "para " << i << endl;
  //    cout << "PADRE " << j << ": ";
  //    sort_asc[i].show();
      padres.push_back(sort_asc[i]);
      j++;
      r = (rand() % 100);
      rand_float = r/100.0;
      i = sort_asc.size();
  //    cout << "posicion reset " << i << endl;
      acumulado = 1.0;
//      cout << "mi random float " << rand_float << endl;
    }
  }

/*  cout << "RETURN DE PADRES " << padres.size() << endl;
  for (i = 0; i < padres.size(); i++)
  {
    padres[i].show();
  }*/
  return padres;
}

/************************** Cruzamiento en un punto de dos individuos ****************************************/
vector<vector<int>> cross_one_point(vector<Individual> padres, float pc){
  cout << "hola, adios, cruzamiento 1313" << endl;
  int i, rand_partition, r, j;
  float rand_float;
  vector<vector<int>> hijos;
  vector<int> crear_men, father_men_1, father_men_2;

  if(padres.size() % 2 != 0){
//    cout << " Canitdad impar de padres" << endl;
    padres.pop_back();
  }
//  else
  //  cout << "cantidad par de padres bien " << endl;

  srand(time(0));

  for (i = 0; (unsigned)i < padres.size(); i=i+2)
  {
    r = (rand() % 100);
    rand_float = r/100.0;
  //  cout << "Numero random " << rand_float << endl; 
   // cout << "padres " << i << " " << i+1 << endl;
    if(rand_float <= pc){
      father_men_1 = padres[i].get_ind();
      father_men_2 = padres[i+1].get_ind();
     // cout << "tamaño del individuo " << father_men_1.size() << endl;
      rand_partition = 1 +  rand() % ((father_men_1.size()-1)); // entre 1 y tamaño del individuo-1
     // cout << "rand_partition donde cortar a los padres " << rand_partition << endl;

      for (j = 0; j < rand_partition; j++)
      {
        crear_men.push_back(father_men_1[j]);
      }
      for (j = rand_partition; (unsigned)j < father_men_2.size(); j++)
      {
        crear_men.push_back(father_men_2[j]);
      }

    /*  cout << "Hijo 1: ";
      for (j = 0; j < crear_men.size(); j++)
      {
        cout << crear_men[j] << " "; 
      }
      cout << endl;*/

      hijos.push_back(crear_men);
      crear_men.clear();

      for (j = 0; j < rand_partition; j++)
      {
        crear_men.push_back(father_men_2[j]);
      }
      for (j = rand_partition; (unsigned)j < father_men_1.size(); j++)
      {
        crear_men.push_back(father_men_1[j]);
      }

    /*  cout << "Hijo 2: ";
      for (j = 0; j < crear_men.size(); j++)
      {
        cout << crear_men[j] << " "; 
      }
      cout << endl;*/

      hijos.push_back(crear_men);
      crear_men.clear();
    }
    else{
      hijos.push_back(padres[i].get_ind());
      hijos.push_back(padres[i+1].get_ind());
    }
  }
/*  for (i = 0; i < hijos.size(); i++)
  {
    cout << "Hijo " << i << ": ";
    for (j = 0; j < hijos[i].size(); j++)
    {
      cout << hijos[i][j] << " ";
    }
    cout << endl;
  }*/

  return hijos;
}


/************************** Genera archivo de salida de un individuo ****************************************/
void output(Individual ind, string name){
  cout << "aqui 1 2 3 en el output" << endl;
  int i, j, type;
  float overused, wasted;
  vector<int> ids_violadas, entities_in_room;
  vector<float> rooms_bad_used;
  vector<int> rep = ind.get_ind();
  vector<vector<int>> individuo;
  
  for (i = 0; (unsigned)i < rep.size(); i++)
  {
    cout << rep[i] << " ";
  }
  cout << endl;

  ids_violadas.clear();

  ofstream salida;
  salida.open(name+".out");

  individuo.assign(params[0], vector<int> (params[1], 0)); //inicializo matriz con 0
  for (i = 0; (unsigned)i < rep.size(); i++)
  {
    individuo[i][rep[i]] = 1;
  }

  for (i = 0; (unsigned)i < cons_soft.size(); i++)
  {
    type = cons_soft[i].get_ctype();;
    switch(type){
      case 0: //Allocation C1->entidad C2->habitación  
        if(!(allocation(rooms[cons_soft[j].get_c2()], entities[cons_soft[j].get_c1()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      case 1: //Non allocation C1->entidad  C2->habitación
        if(!(non_allocation(rooms[cons_soft[j].get_c2()], entities[cons_soft[j].get_c1()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      case 2: //one_of no se usa 
      break;
      case 3: //Capacidad
        if(!(capacity(rooms[cons_soft[j].get_c1()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break; 
      case 4: //Same room
        if(!(same_room(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      case 5: //Not same room 
        if(!(not_same_room(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      case 6:  //Not sharing
        if(!(not_sharing(entities[cons_soft[j].get_c1()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      case 7:  //Adjacency
        if(!(adjacency(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      case 8:  //Nearby  (same floor)
        if(!(nearby(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      case 9:  //AwayFrom  (not same floor)
        if(!(awayfrom(entities[cons_soft[j].get_c1()], entities[cons_soft[j].get_c2()], individuo))){
          ids_violadas.push_back(cons_soft[i].get_cid());
        }
      break;
      default:
      break;
    }
  }
  salida << ids_violadas.size() << " ";
  for (i = 0; (unsigned)i < ids_violadas.size(); i++)
  {
    salida << ids_violadas[i] << " ";
  }
  salida << endl;

  rooms_bad_used.clear();
  rooms_bad_used = ind.set_space_rooms(rooms, entities);

  cout << "Espacio: "; 
  for (i = 0; (unsigned)i < rooms_bad_used.size(); i++)
  {
    cout << rooms_bad_used[i] << " ";
  }
  cout << endl;

  overused = wasted = 0;
  for (i = 0; (unsigned)i < rooms_bad_used.size(); i++)
  {
    if(rooms_bad_used[i] < 0)
      overused += rooms_bad_used[i]*-1;
    if(rooms_bad_used[i] > 0)
      wasted += rooms_bad_used[i];
  }
  salida << overused+wasted << " " << wasted << " " << overused << endl;
  salida << endl;

  for (i = 0; (unsigned)i < rooms_bad_used.size(); i++)
  {
    salida << i << " ";
    if(rooms_bad_used[i] > 0)
      salida << rooms_bad_used[i] << " 0 ";
    else if(rooms_bad_used[i] < 0)
      salida << "0 " << rooms_bad_used[i]*-1 << " ";
    else
      salida << "0 0 ";

    for (j = 0; (unsigned)j < rep.size(); j++)
    {
      if(rep[j] == i)
        entities_in_room.push_back(j);
    }
    salida << entities_in_room.size() << " ";
    for (j = 0; (unsigned)j < entities_in_room.size(); j++)
    {
      salida << entities_in_room[j] << " ";
    }
    salida << endl;
    entities_in_room.clear();
  }

 /* for (i = 0; i < rep.size(); i++)
  {
    salida << rep[i] << " ";
  }*/
  salida.close();
}

/************************** Muta a todos los individuos obtenidos del cruzamiento ****************************************/
vector<vector<int>> mutation(vector<vector<int>> crossover, float pm, vector<vector<Constraint>> ezpeciales){
  cout << "mutacioon" << endl;
  int i, j, k, r, random_room, type, room;
  float rand_float;
  vector<vector<int>> result;
  vector<int> rooms_tabu;
  result = crossover;

  for (i = 0; (unsigned)i < result.size(); i++)
  {
    cout << "individuo " << i << endl;
    for (k = 0; (unsigned)k < result[i].size(); k++)
    {
      cout << "entidad " << k << endl;
      r = (rand() % 100);
      rand_float = r/100.0;
      if(rand_float < pm) //mutar 
      {
        random_room = (rand() % rooms.size());
        for (j = 0; (unsigned)j < ezpeciales[k].size(); j++)
        {
          type = ezpeciales[k][j].get_ctype();
          ezpeciales[k][j].show();
          if(type == 0){
            random_room = ezpeciales[k][j].get_c2();
          }
          else if(type == 1){
            room = ezpeciales[k][j].get_c2();
            while(room == random_room){
              random_room = (rand() % rooms.size());
            }
          }
          else if (type == 6){
            cout << "hola " << endl;
            while((find(rooms_tabu.begin(), rooms_tabu.end(), random_room) != rooms_tabu.end())){
              random_room = (rand() % rooms.size());
              cout << "asd  " ;
            }
            rooms_tabu.push_back(random_room);
          }
        }
        while((find(rooms_tabu.begin(), rooms_tabu.end(), random_room) != rooms_tabu.end())){
          random_room = (rand() % rooms.size());
          cout << "asd  " ;
        }
        result[i][k] = random_room;
      }
      else{
        for (j = 0; (unsigned)j < ezpeciales[k].size(); j++)
        {
          type = ezpeciales[k][j].get_ctype();
          if (type == 6 and !(find(rooms_tabu.begin(), rooms_tabu.end(), result[i][k]) != rooms_tabu.end())){ // not sharing y la hab actual no esta en tabu ok, agregar a tabu
            rooms_tabu.push_back(result[i][k]);
          }
          else if (type == 6 and (find(rooms_tabu.begin(), rooms_tabu.end(), result[i][k]) != rooms_tabu.end())){ //not shring y la hab actual está en tabu asignar otra random
            while((find(rooms_tabu.begin(), rooms_tabu.end(), random_room) != rooms_tabu.end())){
              random_room = (rand() % rooms.size());
              cout << "asd  asd " ;
            }
            result[i][k] = random_room;
            rooms_tabu.push_back(result[i][k]);
          }
        }
      }
    }
    rooms_tabu.clear();
  }

  for (i = 0; (unsigned)i < result.size(); i++)
  {
    for (j = 0; (unsigned)j < result[i].size(); j++)
    {
      cout << result[i][j] << " ";
    }
    cout << endl;
  }
  return result;
}