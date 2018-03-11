/*----- Sección de Declaraciones --------------*/
%{
#include <stdio.h> 
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include "../include/HTTPDownloader.hpp"

using namespace std;

int paso= 0;
string precio_amazon;
vector<string> nombres;
vector<string> precio;
vector<string> precio_sin_descuento;
vector<string> porcentaje_descuento;
multimap<double,string> porcentajes_ord;

%}

/*----- Sección de Reglas ----------------*/
%%
data-name=(.|\n)[^\"]+\"			{ 	string sub = yytext; 
							sub = sub.substr(11); 
							int pos = sub.find("\"");
							nombres.push_back(sub.substr(0,pos));
						}
data-price=(.|\n)[^\"]+\"			{ 	string sub = yytext; 
							sub = sub.substr(12); 
							int pos = sub.find("\"");
							precio.push_back(sub.substr(0,pos) + "€");
						}
span>(.|\n)[^>]+/span>€				{	string sub = yytext; 
							sub = sub.substr(5); 
							int pos = sub.find("<"); 
							precio_sin_descuento.push_back(sub.substr(0,pos)+ "€");
						}
tarjeta-articulo__descuento(.|\n)[^%]+%		{ 	string sub = yytext; 
							int pos = sub.find("%");
							sub = sub.substr(pos-3);
							string num = sub.substr(0,pos-1);
							porcentajes_ord.emplace(stod(num),nombres.back() + "] precio pccomponentes: " + precio.back());
							porcentaje_descuento.push_back(sub); 
						}
.|\n						{}
%%

/*----- Sección de Procedimientos --------*/
int main (int argc, char *argv[]) {
	/***********************************************************************
	* Segundo programa y busca en el HTML de pccomponentes ya descargado
	* las ofertas, los artículos en oferta, junto con su precio
	* el decuento y el precio anterior, pero esta vez sí, quitando la parte
	* que no nos resulta útil, y añadiando cierto formato, además 
	* los porcentajes y los nombres se guardan en un multimap, para que se 
	* ordene automáticamente, para poder hacer el fichero más interesante
	* ordenado por descuento y llama a busqueda_amazon pasandole el nombre
	* del artículo, el precio y el descuento.
	************************************************************************/
	ofstream parseo;
	parseo.open("../data/resultado_pccomponentes.txt");
	yyin = fopen ("../data/parseo.txt", "rt");
	if (yyin == NULL)
	{
		printf ("El fichero %s no se puede abrir\n", argv[1]);
		exit (-1);
	}
	yylex ();
	int i = 0;
	for(auto it : nombres){
		parseo << setw(3) << "Nombre: " << it  << setw(3) << " precio: " << precio[i] << setw(3) << " precio sin descuento: " << precio_sin_descuento[i] << setw(3) << " porcentaje de descuento: " << porcentaje_descuento[i] << internal << endl;
		++i;
	}
	parseo.close();
	map<double,string>::reverse_iterator rit;
	int tope=0;
	for (rit=porcentajes_ord.rbegin(); rit!=porcentajes_ord.rend(); ++rit){
		ostringstream strs;
		strs << rit->first;
		string descuento_prod = strs.str();
		string nombre_arch = rit->second;
		replace(nombre_arch.begin(), nombre_arch.end(), ' ', '+');
		string programa = "./busqueda_amazon " + nombre_arch + " " + descuento_prod;
		system(programa.c_str());
		++tope;
	}
	return 0;
}
