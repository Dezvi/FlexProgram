/*----- Sección de Declaraciones --------------*/
%{
#include <stdio.h> 
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "../include/HTTPDownloader.hpp"

using namespace std;

vector<string> nombres;
%}
/*----- Sección de Reglas ----------------*/
%%
tarjeta-articulo__nombre(.|\n)[^%]+%		{ nombres.push_back(yytext); }
.|\n						{}
%%

/*----- Sección de Procedimientos --------*/
int main (int argc, char *argv[]) {
	/***********************************************************************
	* Primer programa y el que ejecuta el resto, busca en el HTML de pccomponentes
	* en concreto en las ofertas, los artículos en oferta, junto con su precio
	* el decuento y el precio anterior, para ello primero descarga el HTML 
	* crea un archivo con él, lee ese archivo, y el resultado macheado por las
	* expresiones regulares se guarda en otro archivo para volverse a analizar
	* ya que hay "basura" o elementos que no nos interesan y que también ha macheado.
	************************************************************************/
	HTTPDownloader downloader;
	std::string content = downloader.download("https://pccomponentes.com/ofertas-especiales");
	std::ofstream out("../data/pccomponentes.html");
	std::ofstream parseo("../data/parseo.txt");
	out << content;
	out.close();
	yyin = fopen ("../data/pccomponentes.html", "rt");
	if (yyin == NULL)
	{
		printf ("El fichero %s no se puede abrir\n", argv[1]);
		exit (-1);
	}
	yylex ();
	for(auto it : nombres){
		parseo << it;
	}
	parseo.close();
	system("./parseo");
	return 0;
}
