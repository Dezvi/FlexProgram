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
vector<string> nombres;
vector<string> precio;
vector<string> precio_sin_descuento;
vector<string> porcentaje_descuento;

multimap<double,string> porcentajes_ord;
%}

/*----- Sección de Reglas ----------------*/
%%

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
	* Cuarto y último programa, busca en el HTML de amazon el precio de un artículo
	* encontrado en pccomponentes, y si lo encuentra coge su precio y lo añade
	* al fichero resultado "resultadoOrdenado_pccomponentes, realmente hace
	* lo mismo que el anterior pero con una variante, como el nombre del artículo
	* no se encontró reducimos su nombre a las dos primeras palabras para ver si
	* así lo encuentra.
	************************************************************************/
	HTTPDownloader downloader;
	ofstream orden;
	orden.open("../data/resultadoOrdenado_pccomponentes.txt", ios::app);
	int posicion=0;
	string reemplaced = argv[1];
	posicion = reemplaced.find("+");
	string first_word = reemplaced.substr(0,posicion);
	reemplaced = reemplaced.substr(posicion+1);
	posicion = reemplaced.find("+");
	string second_word = reemplaced.substr(0,posicion);
	reemplaced = first_word+"+"+second_word;
	string content = downloader.download("https://www.amazon.es/s/ref=nb_sb_noss_2?__mk_es_ES=%C3%85M%C3%85%C5%BD%C3%95%C3%91&url=search-alias%3Daps&field-keywords="+reemplaced);
	ofstream out("amazon_busqueda.html");
	out << content;
	out.close();
	yyin = fopen("../data/amazon_busqueda.html", "rt");
	if (yyin == NULL)
	{
		printf ("El fichero %s no se puede abrir\n", argv[1]);
		exit (-1);
	}
	yylex();
	string compr;
	compr = reemplaced;
	reemplaced = argv[1];
	replace(reemplaced.begin(), reemplaced.end(), '+', ' ');
	replace(reemplaced.begin(), reemplaced.end(), ']', ' ');
	string descuento_prod = argv[2];
	orden << descuento_prod << "% => " << reemplaced << " precio amazon: " << precio_amazon << endl;
	orden.close();
	return 0;
}
