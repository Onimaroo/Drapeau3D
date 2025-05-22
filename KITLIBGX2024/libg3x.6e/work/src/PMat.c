#include <PMat.h>


// Les "moteurs" : mise à jour de l’état
// intégrateur Leapfrog
static void update_leapfrog(PMat *M, double h) {

    // intégration 1 : vitesse m.F(n) = (V(n+1)-V(n))/h -EXplicite
    M->vit.x += h * M->frc.x / M->m;  
    M->vit.y += h * M->frc.y / M->m;
    M->vit.z += h * M->frc.z / M->m;

    // intégration 2 : position V(n+1) = (X(n+1)-X(n))/h -IMplicite
    M->pos.x += h * M->vit.x;
    M->pos.y += h * M->vit.y;
    M->pos.z += h * M->vit.z;

    // on vide le buffer de force
    M->frc.x = 0.;
    M->frc.y = 0.;
    M->frc.z = 0.;  
}

// intégrateur Euler Explicite
// juste pour l’exemple : méthode très instable -> à éviter
// simple échange des 2 premières lignes par rapport à Leapfrog -> ça change tout
static void update_euler_exp(PMat *M, double h)
{
    // intégration 1 : position V(n) = (X(n+1)-X(n-1))/h -EXplicite
    M->pos.x += h * M->vit.x;    
    M->pos.y += h * M->vit.y;
    M->pos.z += h * M->vit.z;

    // intégration 2 : vitesse m.F(n) = (V(n+1)-V(n))/h -EXplicite
    M->vit.x += h * M->frc.x / M->m;
    M->vit.y += h * M->frc.y / M->m;
    M->vit.z += h * M->frc.z / M->m;

    // on vide le buffer de force
    M->frc.x = 0.;
    M->frc.y = 0.;
    M->frc.z = 0.;  
}

// mise à jour point fixe : ne fait rien
static void update_fixe(PMat *M, double h)
{
    // on vide le buffer de force (par sécurité)
    M->frc.x = 0.;
    M->frc.y = 0.;
    M->frc.z = 0.;  

    // position et vitesse restent inchangées
    M->vit.x = 0.;
    M->vit.y = 0.;
    M->vit.z = 0.;
}

static void drawSphere(PMat *M)
{ 
    glPushMatrix();
    glTranslatef(M->pos.x, M->pos.y, M->pos.z);
    glutSolidSphere(0.15, 40, 40);
    glPopMatrix();
}

// ...
extern void M_builder(PMat *M, int type, double m, double amp, G3Xpoint P0) {
    M->m = m; // masse
    M->amp = amp;
    M->pos = P0; // position initiale
    // Vitesse initiale
    M->vit.x = 0;
    M->vit.y = 0;
    M->vit.z = 0;
    // JAMAIS de force à la création
    M->frc.x = 0;
    M->frc.y = 0;
    M->frc.z = 0;
    
    switch (type) // choix de la fonction de mise à jour
    {
        case 0 : M->update = update_fixe; break;// Point Fixe
        case 1 : M->update = update_leapfrog; break;// Particule Leapfrog
        case 2 : M->update = update_euler_exp; break;// Particule Euler Exp.
    }
    M->draw  = &drawSphere;
}