#ifndef _PMAT_
    #define _PMAT_
    #include <g3x.h>

    typedef struct _ptm_ {
        double m; // masse
        double amp;
        G3Xpoint pos; // position
        G3Xpoint vit;
        G3Xpoint frc;
        G3Xcolor color;
        void (*update)(struct _ptm_ *this, double h); // mise à jour : intégrateur (h: pas de calcul)
        void (*draw)(struct _ptm_ *this); // fonction de dessin
    } PMat;

    // Constructeur
    void M_builder(PMat *M, int type, double m, double amp, G3Xpoint P0);
#endif