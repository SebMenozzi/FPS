#include "objet3DStatique.h"
#include <fstream>

Objet3DStatique::Objet3DStatique(std::string nomFichier)
{
  // Initialisation des attributs
  this->x = 0.0;
  this->y = 0.0;
  this->z = 0.0;
  this->horizontalAngle = 0.0;

  bool8 creationListeAffichage = FALSE;
  std::string textureCourante;

  // Ouverture du fichier
  std::fstream fichier(nomFichier.c_str(), std::fstream::in);

  // Ouverture realisee avec succes
  if (fichier.is_open())
  {
    std::string ligne;

    // Lecture de chaques lignes
    while(getline(fichier, ligne))
    {
      // Chargement du nom
      if (0 == ligne.compare(0, 4, "nom "))
      {
        this->nom.assign(ligne.begin() + 4, ligne.end());
      }

      // Chargement d'une texture
      else if (0 == ligne.compare(0, 6, "image "))
      {
        // Lecture du nom de fichier de la texture
        std::string nomFichierTexture;
        nomFichierTexture.assign(ligne.begin() + 6, ligne.end());

        // Si la texture n'est pas deja creee
        if(this->textures.find(nomFichierTexture) == this->textures.end())
        {
            this->conteneurTextures.ajouter(nomFichierTexture);
            this->textures.insert(nomFichierTexture);
        }

        // Cette texture devient la texture courante
        textureCourante = nomFichierTexture;
      }

      // Debut de coordonnees
      else if (0 == ligne.compare(0, 1, "{"))
      {
        // Detection du debut de la creation de la liste d'affichage
        if (FALSE == creationListeAffichage)
        {
          // Generation et commencement de la liste d'affichage
          this->listeAffichage = glGenLists(1);
          glNewList(this->listeAffichage, GL_COMPILE);

          glEnable(GL_TEXTURE_2D);

          creationListeAffichage = TRUE;
        }

        // Selection de la derniere texture rencontree texture
        glBindTexture(GL_TEXTURE_2D, this->conteneurTextures.texture(textureCourante).texture);

        glBegin(GL_POLYGON);

        // Description des sommets du polygone
        while(getline(fichier, ligne))
        {
          sint32 x2D, y2D, x3D, y3D, z3D;

          // Fin de coordonnees
          if (0 == ligne.compare(0, 1, "}") )
          {
            glEnd();
            break;
          }

          // Commentaire / ligne vide
          else if (0 == ligne.compare(0, 1, "#") || ligne.empty())
          {

          }

          // Coordonnees 2D / 3D
          else if (5 == sscanf(ligne.c_str(), "%ld %ld %ld %ld %ld", &x2D, &y2D, &x3D, &y3D, &z3D))
          {
              glTexCoord2f((float16)x2D / this->conteneurTextures.texture(textureCourante).largeur, (float16)y2D / this->conteneurTextures.texture(textureCourante).hauteur); glVertex3d(x3D, y3D, z3D);
          }

          else
          {
              printf("ATTENTION : Erreur lors du chargement du fichier M3S\n");

              fichier.close();
              return;
          }
        }
      }
      // Correction RST (Rotate, Scale, Translate)
      else if (0 == ligne.compare(0, 4, "rst "))
      {
        std::string rst;
        GLfloat rAngle, rX, rY, rZ, sX, sY, sZ, tX, tY, tZ;

        rst.assign(ligne.begin() + 4, ligne.end());

        if (10 == sscanf(rst.c_str(), "%f %f %f %f %f %f %f %f %f %f",
                         &rAngle, &rX, &rY, &rZ, // glRotatef
                         &sX, &sY, &sZ, // glScalef
                         &tX, &tY, &tZ  // glTranslatef
                        )
           )
        {
          // Generation de la liste d'affichage pour la RST
          this->listeRST = glGenLists(1);
          glNewList(this->listeRST, GL_COMPILE);

          // D�finition de la RST
          glRotatef(rAngle, rX, rY, rZ);
          glScalef(sX, sY, sZ);
          glTranslatef(tX, tY, tZ);

          glEndList();
        }
      }

      // Commentaire et ligne vide
      else if (0 == ligne.compare(0, 1, "#") || ligne.empty())
      {
        // TODO: ...
      }
    }

    // A la fin, la liste d'affichage est termine
    glEndList();

    // Le fichier peut alors etre ferme
    fichier.close();
  }
  else
  {
    printf("Erreur lors de l'ouverture du fichier %s\n", nomFichier.c_str());
  }
}

Objet3DStatique::~Objet3DStatique()
{
  // Liberation des textures
  for (Textures::iterator element = this->textures.begin(); element != textures.end(); element++)
  {
    this->conteneurTextures.supprimer(*element);
  }

  glDeleteLists(this->listeAffichage, 1);
  glDeleteLists(this->listeRST, 1);
}

void Objet3DStatique::dessiner()
{
  // On memorise le repere courant avant d'effectuer la RST
  glPushMatrix();
    // Positionne l'objet en lieu de dessin
    // Atention, la position X 2D devient la position Y 3D et inversement
    glTranslatef(this->y, this->x, 0.0);
    glRotated(this->horizontalAngle, 0.0, 0.0, 1.0);

    // Correction / Adaptation de l'objet a la scene
    glCallList(this->listeRST);

    // Dessin de l'objet
    glCallList(this->listeAffichage);

  // Restoration du repere d'origine
  glPopMatrix();
}
