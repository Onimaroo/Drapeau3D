#ifndef _LINK_
    #define _LINK_
    #include <PMat.h>

    typedef struct _lnk_
    {
        double k, l, z, s, amp, timer; // paramètres divers
        G3Xpoint vent;
        G3Xpoint frc;
        PMat *M1,*M2; // points M de connexion
        G3Xcolor color;
        void (*update)(struct _lnk_ *this); //mise à jour : calcul/distrib des forces
        void (*draw)(struct _lnk_ *this);
    } Link;

    // Connecteur : branche L entre M1 et M2
    void Connect(PMat *M1, Link *L, PMat *M2);

    // Constructeurs
    void Damped_Hook(Link *L, double k, double z); // Ressort + Frein
    void Force_Constante(Link* L, double frc);
    void drawPole();

// autres constructeurs.... autant qu’on veut

#endif