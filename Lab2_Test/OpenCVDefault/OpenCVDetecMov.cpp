#include "stdafx.h"
#include "device_launch_parameters.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>     
#include "CTraitement.h"
#include <windows.h>

using namespace std;
using namespace cv;
#define ENTER  13
#define R 114
#define ESPACE 32
#define TAB 9
#define M 109
#define G 103
#define S 115
#define E 101
#define C 99
#define L 108

//Donnee image
Mat matrice;

//Position Souris
int mouse_X;
int mouse_Y;

//Camera
bool mode_camera = false;
bool mode_couleur = true;

//Classe Traitemet
CTraitement traitement;

//mets en memoire le texte lors des demandes de texte à l'utilisateur
string texteEntrer;

//Prendre une image
void PrendPhoto(Mat matrice, bool &CameraMode)
{
	imwrite("ImageEnCours.png",matrice);
	CameraMode = false;
}

//Gestion de la souris
void CallBackFunc(int event, int x, int y, int flags,void *userdata)
{
		mouse_X = x;
		mouse_Y = y;

		if(event == EVENT_LBUTTONDOWN)
		{
			traitement.premier_click = Point(x,y);
			traitement.click2fois = false;
		}
		else if(event == EVENT_LBUTTONUP)
		{
			traitement.deuxieme_click = Point(x,y);
			traitement.click2fois = true;
		}
}

void MenuDentreeSimple(string Question)
{
	cout << Question;
	getline(cin,texteEntrer);
}

void AfectationMenuDentree(string StatusVariable, string Question, int &Variable)
{
		cout << StatusVariable << Variable <<  '\n' << Question;
		getline(cin,texteEntrer);

		//string en decimale
		string::size_type base10;   
		Variable = stoi(texteEntrer,&base10);
		
		cout << "Seuil Mis a Jour " << Variable <<  '\n';
}

int main(int argc, char** argv)
{
	//Image
	int hauteur;
	int largeur;
	
	//camera
	VideoCapture camera(0);
	bool camera_trouvee = camera.isOpened();
	namedWindow("Lab2",1);

	//key Keyboard
	int key;

	bool Loop = true;
	while(Loop)
	{
		key = waitKey(10);
		
		//capte la souris
		setMouseCallback("Lab2",CallBackFunc,NULL);

		//Avoir la longueur et la largeur de la matrice(Image) en cours
		hauteur = matrice.rows;
		largeur = matrice.cols;
		
		/****** MODE Camera *******/
		if(mode_camera)
		{
			if(camera_trouvee)
			{
				camera >> matrice;

				if(key == ENTER)
					PrendPhoto(matrice,mode_camera);
			}
		}
		/******			 *******/
		
		/****** MODE Image *******/
		else
		{
			matrice = imread("ImageEnCours.png",mode_couleur);
			if(key == R)
			{
				imwrite("ImageEnCours.png",matrice);
			}
		}
		/******			 *******/

		/****** MODE Menu *******/
		if(key == M)
		{
			//6) MODE MENU CONSOLE
			MenuDentreeSimple("Nom de l'image a charger? ");
			matrice = imread(texteEntrer, mode_couleur);
			imwrite("ImageEnCours.png",matrice);
			mode_camera = false;
		}
		/******			 *******/

		//5) EFFACER L'IMAGE
		if(key == ESPACE)
		{
			matrice = NULL;
			imwrite("ImageEnCours.png",matrice);
		}

		//9) PASSER DU MODE Image AU MODE Camera
		if(key == TAB)
		{
			if(mode_camera)mode_camera = false;
			else mode_camera = true;
		}

		//Changer le mode (couleur ou noir et blanc)
		if(key == G)
		{
			if(mode_couleur)mode_couleur = false;
			else mode_couleur = true;
		}

		//Classe Traitement
		if(!mode_camera) 
		{
			traitement.LocaliserCercle(matrice);
			traitement.SeuillageAutomatique(traitement.premier_click,traitement.deuxieme_click,matrice);
		}

		//Menu Seuil de lencadrer
		if(key == S)
		{
			AfectationMenuDentree("Seuil Figure Actuel ","Niveau de seuil ? ",traitement.seuil);
		}
		else if(key == L)
		{
			AfectationMenuDentree("Seuil Contour Actuel ","Niveau de seuil ? ",traitement.SeuilContour);
		}

		//rends visible lencadrer
		if(key == E)
		{
			traitement.encadrer_visible = !traitement.encadrer_visible;
		}

		//Contour active
		if(key == C)
		{
			traitement.Contour_active = !traitement.Contour_active;
		}

		//Affiche Image
		imshow("Lab2",matrice);
	}

	//Liberation des ressources la caméra
	camera.release();
	waitKey(0);
	return 0;
}



