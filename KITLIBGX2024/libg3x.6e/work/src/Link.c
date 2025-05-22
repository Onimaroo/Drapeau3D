#include <Link.h>


static G3Xpoint dist3D_vec(PMat* M1,PMat* M2){ 
    double Xc,Yc,Zc;
    Xc = M2->pos.x - M1->pos.x;
    Yc = M2->pos.y - M1->pos.y;
    Zc = M2->pos.z - M1->pos.z;
    G3Xpoint dist;
    dist.x = Xc; 
    dist.y = Yc; 
    dist.z = Zc;
    return dist; 
}

// Liste des algorithmes
static void update_Force_Constante(Link* L)
{
    // Gravité

    PMat* M = (L->M1 ? L->M1 : L->M2);
    M->frc.y += L->frc.y;

    // Vent

    M->frc.x += L->vent.x * cos(L->timer);
    M->frc.y += L->vent.y * cos(L->timer);
    M->frc.z += L->vent.z * cos(L->timer);

}

static void update_Damped_Hook(Link* L)
{ 
    G3Xpoint distance = dist3D_vec(L->M1,L->M2);
    double d = sqrt(distance.x*distance.x + distance.y*distance.y + distance.z*distance.z );
    double amp = L->k * (1 - L->l / d); 
    G3Xpoint f;

    f.x = amp * distance.x + L->z * (L->M2->vit.x - L->M1->vit.x);
    f.y = amp * distance.y + L->z * (L->M2->vit.y - L->M1->vit.y);
    f.z = amp * distance.z + L->z * (L->M2->vit.z - L->M1->vit.z);

    L->M1->frc.x += f.x; 
    L->M2->frc.x -= f.x;

    L->M1->frc.y += f.y;
    L->M2->frc.y -= f.y;

    L->M1->frc.z += f.z;
    L->M2->frc.z -= f.z;

}  

extern void drawPole(PMat* M) {
    GLUquadricObj *quadric = gluNewQuadric();

    glPushMatrix();

    //draw the cylinder body
    glTranslatef(M->pos.x, M->pos.y - 40, M->pos.z);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    glRotatef(-90, 1, 0, 0.0);
    gluCylinder(quadric, 0.25, 0.25, 50, 100, 100);

    glPopMatrix();

    gluDeleteQuadric(quadric);
    glLineWidth(1.);
}

// Basé sur cette fonction: http://lifeofaprogrammergeek.blogspot.com/2008/07/rendering-cylinder-between-two-points.html

extern void drawFlag(Link* L)
{
    g3x_Material(L->color, 0.3, 0.3, 0.3, 0.3, 1.);
    GLUquadricObj *quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);

    float vx = L->M2->pos.x - L->M1->pos.x;
    float vy = L->M2->pos.y - L->M1->pos.y;
    float vz = L->M2->pos.z - L->M1->pos.z;

    //handle the degenerate case of z1 == z2 with an approximation
    if(vz == 0)
        vz = .0001;

    float v = sqrt(vx * vx + vy * vy + vz * vz);
    float ax = 57.2957795 * acos(vz / v);
    if (vz < 0.0)
        ax = -ax;
    float rx = -vy * vz;
    float ry = vx * vz;
    glPushMatrix();

    //draw the cylinder body
    glTranslatef(L->M1->pos.x, L->M1->pos.y, L->M1->pos.z);
    glRotatef(ax, rx, ry, 0.0);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluCylinder(quadric, 0.05, 0.03, v, 10, 1);

    //draw the first cap
    gluQuadricOrientation(quadric, GLU_INSIDE);
    gluDisk(quadric, 0.0, 0.03, 10, 1);
    glTranslatef(0, 0, v);

    //draw the second cap
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluDisk(quadric, 0.0, 0.03, 10, 1);
    glPopMatrix();

    gluDeleteQuadric(quadric);
    glLineWidth(1.);
}

extern void Force_Constante(Link* L, double frc_const)
{

    L->frc.y = frc_const;
    L->frc.x = L->frc.z = 0;

    L->k = 0.;
    L->z = 0.; 
    L->s = 0.;
    L->l = 0.;

    L->update  = &update_Force_Constante;

    L->draw = NULL;
}

extern void Damped_Hook(Link *L, double k, double z) {
    L->k = k ;
    L->z = z ;

    L->update = &update_Damped_Hook;

    L->color.r = 1.; 
    L->color.g = 1.; 
    L->color.b = 1.; 
    
    L->draw = &drawFlag;
}

// Connecteur : branche L entre M1 et M2
extern void Connect(PMat *M1, Link *L, PMat *M2) {
    L->M1 = M1;
    L->M2 = M2;

    if(M1 != NULL && M2 != NULL){
        float amp = (M1->amp + M2->amp) / 2;
        L->k = L->k * amp;
        L->z = L->z * amp;
        L->amp = amp;
    } 

    else if (M1 == NULL || M2 == NULL) {
        return;
    }

    L->l = g3x_Dist(L->M1->pos, L->M2->pos);
}