/*!=================================================================!*/
/*!= E.Incerti - eric.incerti@univ-eiffel.fr                       =!*/
/*!= Université Gustave Eiffel                                     =!*/
/*!= Code "squelette" pour prototypage avec libg3x.6e              =!*/
/*!=================================================================!*/

#include <g3x.h>

#include <PMat.h>
#include <Link.h>

/* tailles de la fenêtre graphique (en pixels)     */
static int WWIDTH = 1280, WHEIGHT = 1280;


/* -----------------------------------------------------------------------
 * ici, en général pas mal de variables GLOBALES
 * - les variables de données globales (points, vecteurs....)
 * - les FLAGS de dialogues
 * - les paramètres de dialogue
 * - ......
 * Pas trop le choix, puisque TOUT passe par des fonctions <void f(void)>
 * ----------------------------------------------------------------------- */

/*static double   xmin=-_RESOLUTION, ymin=-_RESOLUTION,xmax=+_RESOLUTION,ymax=+_RESOLUTION;*/

/*! VARIABLES DE SIMULATION !*/
static int      Fe;        /* frquence d'échantillonnage        */
static double   h;         /* pas d'échantillonnage 1./Fech      */
static int      Fa = 1;      /* on affiche 1 echantillon sur Faff  */
static double   m, k, z, g, timer_vent;   /* paramètres physiques               */

/*! systeme "Masses-Ressorts" : les particules et les liaisons !*/

static int massCount;
static PMat *TabM = NULL;
static int linkCount;
static Link *TabL = NULL;

/* Vent */
static G3Xclock* app_clock = NULL;
static G3Xpoint vent = {4800, 5700, 1150}; 


bool Modeleur(void)
{

	int linkCountLigne = 10; // 9 particules et 2 points fixes
	int massCount_pL= 15;
	int linkCount_tmp = 0;

	int coucheCount = 1;

	massCount = massCount_pL * linkCountLigne;

	massCount *= coucheCount; 

	if (!(TabM=(PMat*)calloc(massCount,sizeof(PMat)))) return false;

	int linkCount_pL = (massCount_pL - 1) + (massCount_pL - 1);
	linkCount = linkCount_pL * linkCountLigne;

	if(linkCountLigne > 1) {
		linkCount = linkCount + (massCount_pL) * (linkCountLigne - 1);

		if(coucheCount == 1){
			linkCount = linkCount + (massCount_pL - 1) * (linkCountLigne-1) * 2 ;
		}

		linkCount_tmp = linkCount;

		linkCount = linkCount + (massCount_pL - 2) * linkCountLigne + (linkCountLigne - 2) * (massCount_pL - 1);/* ajout de toute les liaisons de courbure en enlevant la colone des points fixes*/
	}

	if (!(TabL = (Link*) calloc(linkCount, sizeof(Link)))) { 
		return false;
	}

	Fe = 1000; h = 1. / Fe; m = 1.;

	k = 0.1 * (m * Fe * Fe); z = 0.01 * (m * Fe); g = -5. * Fe;      

	timer_vent = 0;
	app_clock = (G3Xclock*) calloc(0, sizeof(G3Xclock));

	double pfixe = 8;

	for(int idx_couche = 0; idx_couche < coucheCount; idx_couche++) {

		for (int j = 0 ; j < linkCountLigne ; j++) {

			PMat* M = TabM  + j * massCount_pL;
			G3Xpoint xyz = {-pfixe, j, idx_couche};
			M_builder(M++, 0, 1, 1, xyz);

			for (int i = 1; i < massCount_pL; i++) {
				float amp =  1 + i *(-0.5) / (massCount_pL-1); 
				G3Xpoint xyz = {-pfixe + i, j, idx_couche};
				M_builder(M++, 1, m * amp, amp, xyz);
			}

			Link* link_init = TabL + j * linkCount_pL;

			for (int i = 0; i < massCount_pL - 1; i++) {
				Damped_Hook(link_init++, k, z);
			}
			for (int i = 0; i < massCount_pL - 1; i++) {
				Force_Constante(link_init++, g); 
			}

			link_init = TabL + j * linkCount_pL;

			M = TabM + j * massCount_pL;

			while (link_init < TabL + j * linkCount_pL + massCount_pL - 1) {
				Connect(M, link_init++, M + 1);
				M++;
			}

			M = TabM + j * massCount_pL + 1; 

			while (link_init < TabL + (j + 1) * linkCount_pL) {
				Connect(M++, link_init++, NULL);
			}
		}


		for(int j = 0; j < linkCountLigne - 1; j++){
			int idx;

			idx = linkCountLigne * linkCount_pL + j*massCount_pL + j * (massCount_pL - 1) * 2;
			
			Link* link_first_row = TabL + idx;

			for (int i = 0; i < massCount_pL; i++) {
				Damped_Hook(link_first_row++, k, z);
			}

			link_first_row = TabL + idx;

			PMat* M = TabM + j * massCount_pL;

			while (link_first_row < TabL + idx + massCount_pL) {
				Connect(M, link_first_row++, M + massCount_pL); M++;
			}

			int idx_2;
			Link *link_second_row;

			if(coucheCount == 1) {
				idx_2 = idx + massCount_pL;
				link_second_row = TabL + idx_2;

				for (int i = 0; i < (massCount_pL - 1) * 2; i++) {
					Damped_Hook(link_second_row++, k, z);
				}

				link_second_row = TabL + idx_2;
				M = TabM + j * massCount_pL + 1 ;
				
				while (link_second_row < TabL + idx_2 + (massCount_pL - 1) * 2) {
					Connect(M - 1, link_second_row++, M + massCount_pL);
					Connect(M, link_second_row++, M - 1 + massCount_pL);
					M++;
				}
			}
		}

		for(int j = 0; j < linkCountLigne ; j++){

			if (j < linkCountLigne - 2){
				int idx_3 = linkCount_tmp + j *(2 * massCount_pL - 3);
				Link* link_third_row = TabL + idx_3;
				
				for (int i = 0; i < 2 * massCount_pL - 3; i++) {
					Damped_Hook(link_third_row++, k, z);
				}
				link_third_row = TabL + idx_3;

				PMat* M = TabM + j * massCount_pL + 1;

				while (link_third_row < TabL + idx_3 + 2 * massCount_pL - 3) {
					if(M->pos.x != +pfixe){
						Connect(M - 1, link_third_row++, M + 1);
					}
					Connect(M, link_third_row++, M + 2 * massCount_pL);
					M++;
				} 
			} 
			else {
				int idx_3 = linkCount_tmp + (linkCountLigne - 2) * (2 * massCount_pL - 3)  + (j - (linkCountLigne - 2)) * (massCount_pL - 2);
				Link* link_third_row = TabL + idx_3;
				
				for (int i = 0; i < massCount_pL - 2; i++) {
					Damped_Hook(link_third_row++, k, z);
				}

				link_third_row = TabL+ idx_3;

				PMat* M = TabM + j * massCount_pL;

				while (link_third_row < TabL + idx_3 + massCount_pL - 2) {
					link_third_row->color.r = 1.; 
					link_third_row->color.g = 0.; 
					link_third_row->color.b = 0.; 
					link_third_row->color.a = 1.;
					Connect(M, link_third_row++, M+2);
					M++;
				} 
			}
		}
	}

	return true;
}

/* 1°) la fonction d'initialisation : appelée 1 seule fois, au début       */
static void init(void)
{
	/*!  Ici, pas de dessin, pas de calcul dynamique, rien que des initialisations
	*   -> allocation(s) de mémoire, init. des paramètres globaux,
	*      construction de modèles....
	*      ouverture de fichiers, pré-chargement de données....
	*
	*   Tout ce qu'il y a ici pourrait être écrit directement dans le main()
	*   juste après l'appel à g3x_InitWindow()
	!*/
	Modeleur();
}

/* 2°) la fonction de contrôle : appelée 1 seule fois, juste APRES <init>  */
static void ctrl(void)
{
	/*! Interface de dialogue (partie statique) : création des boutons, scrollbars....
	*  Tout ce qu'il y a ici pourrait être directement écrit dans la fonction init(),
	*  mais c'est plus 'propre' et plus pratique de séparer.
	!*/

	g3x_SetScrollWidth(10); 
	g3x_CreateScrollv_i("h", &Fa, 1,20, 1, "h"); /*  fq affichage */
	g3x_CreateScrollv_d("k", &k, k * 0.01, k*5. ,1,"k");
	g3x_CreateScrollv_d("z", &z, z * 0.01, z*5. ,1,"z");
}

/* 3°) la fonction de contrôle : appelée en boucle, juste AVANT <draw>     */
static void evts(void)
{
  /*! Interface de dialogue (partie dynamique) : les touches clavier, la souris ....
   *  Tout ce qu'il y a ici pourrait être directement écrit dans la fonction draw(),
   *  mais c'est plus 'propre' et plus pratique de séparer.
  !*/
}

/* 4°) la fonction de dessin : appelée en boucle (indispensable)           */
static void draw(void)
{
	/*! C'est la fonction de dessin principale : elle ne réalise que de l'affichage
	*  sa mise à jour est automatique :
	*  - si un paramètre de contrôle est modifié
	*  - si la fonction <anim()> (calcul) est activée
	*  ATTENTION : surtout pas d'alloc. mémoire ici !!!
	!*/

	g3x_SetRefreshFreq(Fa);
	PMat *M = TabM;

	drawPole(M);

	for(M = TabM; M < TabM + massCount; M++) {
		M->draw(M);
	}

	g3x_GetTime(app_clock); 
	timer_vent = app_clock->ss;

	for(Link* L = TabL; L < TabL + linkCount; L++) {
		L->timer = timer_vent;
		if(L->color.r == 1) {
			L->draw(L);
		}

		L->vent = vent; 
		L->k = k *L->amp; 
		L->z = z *L->amp;
	}
}

/* 5°) la fonction d'animation : appelée en boucle draw/anim/...           */
static void anim(void)
{
	/*! C'est la fonction de 'calcul' qui va modifier les 'objets' affichés
	*  par la fonction de dessin (déplacement des objets, calculs divers...)
	*  Si elle n'est pas définie, c'est qu'il n'y a pas d'animation.
	*  ATTENTION : surtout pas d'alloc. mémoire ici !!!
	!*/

	for(Link* L = TabL; L < TabL + linkCount; L++) {
		L -> update(L);
	}
	for(PMat* M = TabM; M < TabM + massCount; M++) {
		M -> update(M, h);
	}
}

/* 6°) la fonction de sortie  (facultatif)                                 */
static void quit(void)
{
	/*! Ici, les opérations à réaliser en sortie du programme
	*  - libération de la mémoire éventuellement alloueé dans <init()>
	*  - fermeture de fichiers ....
	*  - bilan et messages...
	*  Au final cette fonction est exécutée par un appel à <atexit()>
	!*/
	if (TabM != NULL) { 
		free(TabM);
	} 
	if (TabL!=NULL) {
		free(TabL); 
	}
}

/***************************************************************************/
/* La fonction principale : NE CHANGE (presque) JAMAIS                     */
/***************************************************************************/
int main(int argc, char **argv)
{
	/* 1°) creation de la fenetre - titre et tailles (pixels) */
	g3x_InitWindow(*argv, WWIDTH, WHEIGHT);

	glClearColor(255,255,255,1);

	float fardepth = 2000;
	g3x_SetPerspective(1.,fardepth,1.);

	g3x_SetCameraSpheric(0.51 * PI,-0.55 * PI, - 1300); // Fixer la caméra initiale

	/* 2°) association des fonctions */
	g3x_SetInitFunction(init); /* fonction d'initialisation */
	g3x_SetCtrlFunction(ctrl); /* fonction de contrôle      */
	g3x_SetEvtsFunction(evts); /* fonction d'événements     */
	g3x_SetDrawFunction(draw); /* fonction de dessin        */
	g3x_SetAnimFunction(anim); /* fonction d'animation      */
	g3x_SetExitFunction(quit); /* fonction de sortie        */

	/* 3°) lancement de la boucle principale */
	return g3x_MainStart();
	/* RIEN APRES CA */
}
