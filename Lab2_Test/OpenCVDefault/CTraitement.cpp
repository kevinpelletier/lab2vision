#include "StdAfx.h"
#include "CTraitement.h"

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif
#define COULEURINTERIEUR 225
#define COULEURCONTOUR 2

CTraitement::CTraitement(void)
{
	seuil = 5;
	encadrer_visible = false;
	Contour_active = false;
	SeuilContour = 100;
}


CTraitement::~CTraitement(void)
{

}

void CTraitement::DessineLigne(Point PHautGauche, Point PBasDroit, Mat mat, Scalar Couleur)
{
	if(click2fois) line(mat,PHautGauche,Point(PBasDroit.x,PHautGauche.y),Couleur);
}

void CTraitement::DessineTemplateGraphic(Mat mat,Point PHautGauche, Point PBasDroit)
{
	//decalage du graphique
	Point Decalage = Point(50,100);

	//propriete Fenetre dapplication
	int FenetreX = 640;
	int FenetreY = 480;

	//Grosseur du graphique
	float Agrandissement = 0.75f;
	int GraphiqueX = FenetreX * Agrandissement;
	int GraphiqueY = FenetreY * Agrandissement;
	
	//Affichage du texte(graduation et mediane)
	char Texte[200];

	//Ligne abcisse et ordonnee
	line(mat,Point(0,GraphiqueY) + Decalage,Point(GraphiqueX,GraphiqueY) + Decalage,Scalar(0,0,0),2);
	line(mat,Point(0,GraphiqueY) + Decalage,Point(0,0)               + Decalage,Scalar(0,0,0),2);

	int nbPallier = 5;
	int Ecart = 50;
	Point PositionGraduation;

	//Dessin des graduation
	for(int i = 0; i < nbPallier; i++)
	{
		//Part du point le plus bas Y et decale de (i+1) * Ecart vers le haut
		PositionGraduation = Point(0,GraphiqueY - (Ecart * (i + 1)));
		PositionGraduation += Decalage;

		//Ligne horizontal de graduation
		line(mat, PositionGraduation,Point(PositionGraduation.x + 10,PositionGraduation.y),Scalar(0,0,0),2);
		
		//Ecriture de la graduation
		sprintf(Texte, "%i", (i+1) * 50);
		string TG = Texte;
		putText(mat,TG ,Point(PositionGraduation.x - 40,PositionGraduation.y),FONT_HERSHEY_COMPLEX,0.5,Scalar(0,0,0,0),1);
	}

	int DistanceX = abs(PHautGauche.x - PBasDroit.x);
	float DonneeX,DonneeXAvant;
	int Max = -100000;
	int Min = 99999999;
	for(int i = 0; i < DistanceX; i++)
	{
		//trouve la donnee actuelle
		DonneeX = matrice_resultante.data[(PHautGauche.y * matrice_resultante.rows) +  (PHautGauche.x + i)];
		Point PositionPixel(i,GraphiqueY - DonneeX);
		PositionPixel += Decalage;

		//trouve l'ancienne donnee actuelle
		DonneeXAvant = matrice_resultante.data[(PHautGauche.y * matrice_resultante.rows) +  (PHautGauche.x + i - 1)];
		Point PositionPixelAvant(i, GraphiqueY - DonneeXAvant);
		PositionPixelAvant += Decalage;

		//Calcul Min Max
		if(DonneeX > Max) 
			Max = DonneeX;
		if(DonneeX < Min) 
			Min = DonneeX;

		//relie le pixel avec celui davant
		line(mat, PositionPixel,PositionPixelAvant,Scalar(0,0,0),1);
	}

	//Calcul Mediane
	if(Min != Max)
	{
		int Mediane = (Min + (abs(Min - Max)/2));
		Point PointMilieuDebut = Point(0, GraphiqueY - Mediane) + Decalage;
		Point PointMilieuFin = Point(GraphiqueX, GraphiqueY - Mediane) + Decalage;
		sprintf(Texte, "%i", Mediane);
		string MedianeString = Texte;
		putText(mat,MedianeString ,PointMilieuFin,FONT_HERSHEY_COMPLEX,0.5,Scalar(0,0,0,0),2);
		line(mat,PointMilieuDebut, PointMilieuFin,Scalar(0,0,0,0),1);
	}
	imshow("seuillage automatique",mat);
}

void CTraitement::SeuillageAutomatique(Point PHautGauche, Point PBasDroit, Mat &mat)
{
	int NumeroLigne = PHautGauche.y;
	DessineLigne(PHautGauche,PBasDroit,mat, Scalar(255,0,0));

	matrice_Graphics = imread("Graphic.png",0);
	if(click2fois)DessineTemplateGraphic(matrice_Graphics,PHautGauche,PBasDroit);
	click2fois = false;
}

void CTraitement::DessineCroix(int x, int y, Mat matrice, Scalar PipetteColor)
{
	int CrossSize = 20;

	//Ligne horizontal
	Point P1 (x - CrossSize/2, y);
	Point P2 (x + CrossSize/2, y);

	//Ligne Verticale
	Point P3 (x, y - CrossSize/2);
	Point P4 (x, y + CrossSize/2);

	line(matrice,P1,P2,PipetteColor);
	line(matrice,P3,P4,PipetteColor);
}

int ValeurPixel(Mat matrice,Point Position)
{
	return matrice.data[(Position.y * matrice.cols) + (Position.x)];
}

void ModifierPixel(Mat matrice, Point Position, int Couleur)
{
	matrice.data[(Position.y * matrice.cols) + Position.x] = Couleur;
}

bool PixelDansLaForme(int &Couleur, list<Point> &List, Point Position, Mat matrice)
{
	//FONCTIONNEL SANS SEUIL
	Couleur =  ValeurPixel(matrice,Position);

	//si couleur est noir
	if(Couleur == 0)
	{
		if(find(List.begin(), List.end(), Position) == List.end())
		{
			List.push_back(Position);
		}
		return true;
	}
	else if(Couleur == 255) //si la couleur est blanc
	{
		//ModifierPixel(matrice,Position,COULEURCONTOUR);
		return false;
	}
}

bool PixelDansLaForme(int &Couleur, list<Point> &List, Point Position, Mat matrice, int ContenuCentre, int seuil)
{
	//FONCTIONNEL AVEC SEUIL
	Couleur =  ValeurPixel(matrice,Position);

	int DistancePointSeuil;
	DistancePointSeuil = Couleur - ContenuCentre;
	if(DistancePointSeuil < 0)
	{
		if(find(List.begin(), List.end(), Position) == List.end())
		{
			List.push_back(Position);
		}
		return true;
	}
	else if (DistancePointSeuil > 0)
	{
		//ModifierPixel(matrice,Position,COULEURCONTOUR);
		return false;
	}
}

void CTraitement::TrouveCoinRectangle(Mat MatriceOriginal)
{
	int MinX = 9999999;
	int MinY = 9999999;
	int MaxX = -9999999;
	int MaxY = -9999999;

	//Min max Contour
	Point P1,P2,P3,P4;
	for (list<Point>::iterator i = ListPixelContour.begin(); i != ListPixelContour.end(); i++)
	{
		Point p = *i;
		if(p.x < MinX) MinX = p.x;
		if(p.y < MinY) MinY = p.y;
		if(p.x > MaxX) MaxX = p.x;
		if(p.y > MaxY) MaxY = p.y;
	}
	
	int Y1 = 9999999999999,Y2 = -99999999999; //relatif a XMin
	int Y3 = 9999999999999,Y4 = -99999999999; //relatif a XMax
	int X1 = 9999999999999,X2 = -99999999999; //relatif a YMin
	int X3 = 9999999999999,X4 = -99999999999; //relatif a YMax
	for (list<Point>::iterator i = ListPixelContour.begin(); i != ListPixelContour.end(); i++)
	{
		Point p = *i;
		if(p.x == MinX) 
		{
			if(p.y < Y1) Y1 = p.y;
			if(p.y > Y2) Y2 = p.y;
		}

		if(p.x == MaxX) 
		{
			if(p.y < Y3) Y3 = p.y;
			if(p.y > Y4) Y4 = p.y;
		}

		if(p.y == MinY) 
		{
			if(p.x < X1) X1 = p.x;
			if(p.x > X2) X2 = p.x;
		}

		if(p.y == MaxY) 
		{
			if(p.x < X3) X3 = p.x;
			if(p.x > X4) X4 = p.x;
		}
	}

	Point Extremite1 = Point(MinX,Y1);
	Point Extremite2 = Point(X4,MaxY);

	DessineCroix(MinX,Y1,matrice_resultante,Scalar(255,255,255));
	DessineCroix(MinX,Y2,matrice_resultante,Scalar(255,255,255));
	DessineCroix(MaxX,Y3,matrice_resultante,Scalar(255,255,255));
	DessineCroix(MaxX,Y4,matrice_resultante,Scalar(255,255,255));

	DessineCroix(X1,MinY,matrice_resultante,Scalar(255,255,255));
	DessineCroix(X2,MinY,matrice_resultante,Scalar(255,255,255));
	DessineCroix(X3,MaxY,matrice_resultante,Scalar(255,255,255));
	DessineCroix(X4,MaxY,matrice_resultante,Scalar(255,255,255));

	Point Teta = Extremite1 - Extremite2;
	
	//Mets le texte de langle
	int angle = (180/M_PI)* atan2((double)-Teta.y,(double)Teta.x);
	angle = abs(angle);

	char TextCharAngle[200];
	sprintf(TextCharAngle,"Angle %i",angle);
	string SAngle = TextCharAngle;
	putText(matrice_resultante,SAngle ,Point(20,20),FONT_HERSHEY_COMPLEX,0.5,Scalar(0,0,0,0),1);
}

void CTraitement::AvoirContour(Mat matrice, Point Position)
{
	list<Point>ListPixelForme;
	ListPixelForme.push_back(Position);
	Point PixelEnCours,PixelAdjacent;
	int ContenuAdjacent;

	do
	{
		//Point en cours dans la liste
		PixelEnCours = ListPixelForme.front();
		ModifierPixel(matrice,PixelEnCours,SeuilContour);
		ContenuPixelCentre = ValeurPixel(matrice,Position);

		//Pixel Droit
		PixelAdjacent = Point(PixelEnCours.x + 1,PixelEnCours.y + 0);
		if(!PixelDansLaForme(ContenuAdjacent,ListPixelForme,PixelAdjacent,matrice,ContenuPixelCentre,SeuilContour))
		{
			if(find(ListPixelContour.begin(), ListPixelContour.end(), PixelAdjacent) == ListPixelContour.end())
			{
				ListPixelContour.push_back(PixelEnCours);
			}
		}
		
		//Pixel Gauche
		PixelAdjacent = Point(PixelEnCours.x - 1,PixelEnCours.y + 0);
		if(!PixelDansLaForme(ContenuAdjacent,ListPixelForme,PixelAdjacent,matrice,ContenuPixelCentre,SeuilContour))
		{
			if(find(ListPixelContour.begin(), ListPixelContour.end(), PixelAdjacent) == ListPixelContour.end())
			{
				ListPixelContour.push_back(PixelEnCours);
			}
		}

		//Pixel Haut
		PixelAdjacent = Point(PixelEnCours.x + 0,PixelEnCours.y + 1);
		if(!PixelDansLaForme(ContenuAdjacent,ListPixelForme,PixelAdjacent,matrice,ContenuPixelCentre,SeuilContour))
		{
			if(find(ListPixelContour.begin(), ListPixelContour.end(), PixelAdjacent) == ListPixelContour.end())
			{
				ListPixelContour.push_back(PixelEnCours);
			}
		}

		//Pixel Bas
		PixelAdjacent = Point(PixelEnCours.x + 0,PixelEnCours.y - 1);
		if(!PixelDansLaForme(ContenuAdjacent,ListPixelForme,PixelAdjacent,matrice,ContenuPixelCentre,SeuilContour))
		{
			if(find(ListPixelContour.begin(), ListPixelContour.end(), PixelAdjacent) == ListPixelContour.end())
			{
				ListPixelContour.push_back(PixelEnCours);
			}
		}

		ListPixelForme.pop_front();
	}
	while(ListPixelForme.size() > 0);

	//for dans une liste c++
	//Dessin Contour
	for (list<Point>::iterator i = ListPixelContour.begin(); i != ListPixelContour.end(); i++)
	{
		Point  Pixel = *i;
		ModifierPixel(matrice,Pixel,2);
	}
}

void CTraitement::LocaliserCercle(Mat matrice)
{
	//Image
	Hauteur = matrice.rows;
	Largeur = matrice.cols;	

	//Met matrice(en couleur) dans MatriceResultante(en noir et blanc)
	cvtColor(matrice,matrice_resultante,CV_RGB2GRAY);

	//(declaration de tableau a taille variable *Pointeur*) AIDE MEMOIRE
	//unsigned int* TotalParColonne = new unsigned int[Largeur];
	//unsigned int* TotalParLigne = new unsigned int[Hauteur];

	//Taille fixe
	unsigned int TotalParColonne[640];
	unsigned int TotalParLigne[480];
	unsigned int SommePixel = 0;

	//initialisation 
	for(int i = 0; i < sizeof(TotalParColonne)/sizeof(int); i++)
		TotalParColonne[i] = 0;

	for(int i = 0; i < sizeof(TotalParLigne)/sizeof(int); i++)
		TotalParLigne[i] = 0;

	//fais la somme des pixels par Ligne
	for (int i = 0; i < Hauteur; i++)
	{
		for(int j = 0; j < Largeur; j++)
		{
			SommePixel += matrice_resultante.data[(i * Largeur) + j];
		}

		TotalParLigne[i] = SommePixel;
		SommePixel = 0;
	}

	//fais la somme des pixels par Colonne
	for (int i = 0; i < Largeur; i++)
	{
		for(int j = 0; j < Hauteur; j++)
		{
			SommePixel += matrice_resultante.data[(j * Largeur) + i];
		}

		TotalParColonne[i] = SommePixel;
		SommePixel = 0;
	}

	int Index = 0;
	bool Trouve = false;
	int TotalEnCours;
	int AncienTotalEnCours;
	
	/*Comparaison des lignes et colonnes entre elles*/

	//Colonne
		//Gauche a droite
	while (!Trouve && Index < Hauteur)
	{
		Index++;
		TotalEnCours = TotalParColonne[Index];
		AncienTotalEnCours = TotalParColonne[Index - 1];
		
		//Distance
		if(abs(TotalEnCours - AncienTotalEnCours) >  seuil)
		{
			Trouve = true;
			p_gauche.x = Index;
		}
	}

	Trouve = false;
	Index = Largeur;
	TotalEnCours = 0;
	AncienTotalEnCours = 0;

		//Droite a gauche
	while (!Trouve && Index > 0)
	{
		Index--;
		TotalEnCours = TotalParColonne[Index];
		AncienTotalEnCours = TotalParColonne[Index - 1];

		if(abs(TotalEnCours - AncienTotalEnCours) >  seuil)
		{
			Trouve = true;
			p_droit.x = Index;
		}
	}

	Trouve = false;
	Index = 0;
	TotalEnCours = 0;
	AncienTotalEnCours = 0;

	//Ligne
		//Haut a Bas
	while (!Trouve && Index < Largeur)
	{
		Index++;
		TotalEnCours = TotalParLigne[Index];
		AncienTotalEnCours = TotalParLigne[Index - 1];

		if(abs(TotalEnCours - AncienTotalEnCours) >  seuil)
		{
 			Trouve = true;
			p_haut.y = Index;
		}
	}

	Trouve = false;
	Index = Hauteur;
	TotalEnCours = 0;
	AncienTotalEnCours = 0;

		//Bas a Haut
	while (!Trouve && Index > 0)
	{
		Index--;
		TotalEnCours = TotalParLigne[Index];
		AncienTotalEnCours = TotalParLigne[Index - 1];

		if(abs(TotalEnCours - AncienTotalEnCours) >  seuil)
		{
			Trouve = true;
			p_bas.y = Index;
		}
	}

	//Dessine les lignes et colonnes des coin 
	if(encadrer_visible)
	{
		//Largeur
		for(int i = 0; i < Hauteur; i++)
		{
			for(int j = 0; j < Largeur; j++)
			{
				if((i == p_bas.y && j > p_gauche.x  && j < p_droit.x ) || 
				   (i == p_haut.y && j > p_gauche.x  && j < p_droit.x)) matrice_resultante.data[j + (i * Largeur)] = 0;	
			}
		}

		//Hauteur
		for(int i = 0; i < Largeur; i++)
		{
			for(int j = 0; j < Hauteur; j++)
			{
				if((i == p_gauche.x && j > p_haut.y && j < p_bas.y) || 
				   (i == p_droit.x && j > p_haut.y && j < p_bas.y)) matrice_resultante.data[i + (j * Largeur)] = 0;	
			}
		}
	}

	//Trouver les dimensions du cercle trouver
	int LargeurCercle = abs(p_gauche.x - p_droit.x );
	int HauteurCercle = abs(p_bas.y - p_haut.y);
	Scalar Couleur(255,255,255);

	point_milieu.x = p_gauche.x + LargeurCercle/2;
	point_milieu.y =  p_bas.y - HauteurCercle/2;

	//Dessine la croix et laffiche
	DessineCroix(point_milieu.x,point_milieu.y,matrice_resultante,Couleur);
	
	//ne prends pas en compte la croix dessiner
	Mat MatriceOriginalNoirEtBlanc;
	cvtColor(matrice,MatriceOriginalNoirEtBlanc,CV_RGB2GRAY);

	//localise les coins du rectangle
	TrouveCoinRectangle(MatriceOriginalNoirEtBlanc);

	//Contour
	if(Contour_active && !ContourActif_une_fois)
	{
		ContourActif_une_fois = true;
		AvoirContour(MatriceOriginalNoirEtBlanc, point_milieu);
		imshow("Contour",MatriceOriginalNoirEtBlanc);
	}
	else if(!Contour_active)
	{
		ContourActif_une_fois = false;
	}

	imshow("Resultante",matrice_resultante);
}
