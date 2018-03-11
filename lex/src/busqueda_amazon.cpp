/*----- Sección de Declaraciones --------------*/
%{
#include <stdio.h> 
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "../include/HTTPDownloader.hpp"
#include <map>
#include <iomanip>
#include <algorithm>

using namespace std;

bool encontrado = false;
string precio_amazon;
string busqueda;
vector<string> nombres;
vector<string> precio;
vector<string> precio_sin_descuento;
vector<string> porcentaje_descuento;

multimap<double,string> porcentajes_ord;
%}

/*----- Sección de Reglas ----------------*/
%%

noResultsTitle\"		{ busqueda = yytext; }
a-text-bold\">EUR(.)[^<]+<	{ 
				if(!encontrado){
					string sub = yytext; 
					sub = sub.substr(17); 
					int pos = sub.find("<");
					precio_amazon = sub.substr(0,pos) + "€"; 
					encontrado = true;
				}
				}
.|\n				{}
%%

/*----- Sección de Procedimientos --------*/
int main (int argc, char *argv[]) {
	/***********************************************************************
	* Tercer programa, busca en el HTML de amazon el precio de un artículo
	* encontrado en pccomponentes, y si lo encuentra coge su precio y lo añade
	* al fichero resultado "resultadoOrdenado_pccomponentes, sino lo encuentra
	* llama a busqueda_amazonErr, pasandole el nombre del artículo, el precio y el descuento
	************************************************************************/
	HTTPDownloader downloader;
	ofstream orden;
	orden.open("../data/resultadoOrdenado_pccomponentes.txt", ios::app);
	string nombre = argv[1];
	int pos = nombre.find("]");
	string sub = nombre.substr(0,pos); 
	string content = downloader.download("https://www.amazon.es/s/ref=nb_sb_noss_2?__mk_es_ES=%C3%85M%C3%85%C5%BD%C3%95%C3%91&url=search-alias%3Daps&field-keywords="+sub);
	ofstream out("../data/amazon_busqueda.html");
	out << content;
	out.close();
	yyin = fopen("../data/amazon_busqueda.html", "rt");
	if (yyin == NULL){
		printf ("El fichero %s no se puede abrir\n", argv[1]);
		exit (-1);
	}
	yylex();
	string descuento_prod = argv[2];
	if(busqueda.size() > 1){
		orden.close();
		string sistema = "./busqueda_amazonErr " + nombre + " " + descuento_prod;
		system(sistema.c_str());
	}else{
		replace(nombre.begin(), nombre.end(), '+', ' ');
		replace(nombre.begin(), nombre.end(), ']', ' ');
		orden << descuento_prod << "% => " << nombre << ", precio amazon: " << precio_amazon << endl;
		orden.close();
	}
	return 0;
}
