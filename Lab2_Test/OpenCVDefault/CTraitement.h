#pragma once
#include "device_launch_parameters.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>     
#include <list>
using namespace std;
using namespace cv;

class CTraitement
{
public:
	CTraitement(void);
	~CTraitement(void);
	void DessineCroix(int x, int y, Mat matrice, Scalar PipetteColor);
	void LocaliserCercle(Mat matrice);
	void SeuillageAutomatique(Point PHautGauche, Point PBasDroit, Mat &mat);
	void DessineLigne(Point PHautGauche, Point PBasDroit, Mat mat, Scalar Couleur);
	void DessineTemplateGraphic(Mat mat,Point PHautGauche, Point PBasDroit);
	void AvoirContour(Mat matrice, Point Position);
	void TrouveCoinRectangle(Mat MatriceOriginal);

	Point premier_click;
	Point deuxieme_click;
	bool click2fois;
	int seuil;
	bool encadrer_visible;
	bool Contour_active;
	bool ContourActif_une_fois;
	int SeuilContour;
	
private:
	int Hauteur;
	int Largeur;
	Mat matrice_Graphics;
	Point point_milieu;
	Mat matrice_resultante;
	Point p_gauche,p_droit,p_haut,p_bas;
	int ContenuPixelCentre;
	list<Point>ListPixelContour;
};

