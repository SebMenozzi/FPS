#include <SDL2/SDL.h>
#include "heightmap.h"

Heightmap::Heightmap(const char* file)
{
  this->heightmap = NULL;
  this->height = 0u;
  this->width = 0u;
  this->atenuation = 10.0;

  this->texturesContainer.addTexture("herbe.bmp");

  this->heightmap = SDL_LoadBMP(file);

  if ((NULL != this->heightmap)) {

    this->width = this->heightmap->w;
    this->height = this->heightmap->h;

    std::vector<float16> tmp;

  	for(int y = 0; y < this->height; ++y) {
  		tmp.clear();
  		for(int x = 0; x < this->width; ++x) {
  			tmp.push_back(((float16) (getPixel(this->heightmap, x, y) & 0xff)));
  		}
  		this->heights.push_back(tmp);
  	}

  } else {
    std::cout << "Failure to load the heightmap!" << std::endl;
  }
}

Heightmap::~Heightmap()
{
  SDL_FreeSurface(this->heightmap);

  delete this->heightmap;

  this->texturesContainer.deleteTexture("herbe.bmp");
}

void Heightmap::draw()
{
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, this->texturesContainer.getTexture("herbe.bmp").texture);

  if (NULL != this->heightmap) {
    for(int y = 0; y < this->height - 1; ++y) {

      glBegin(GL_TRIANGLE_STRIP);

      for(int x = 0; x < this->width - 1; ++x) {

        glTexCoord2d(x, y);
        glVertex3f(x, y, this->heights[x][y] / this->atenuation);

        //glVertex3f(x, y, ((float16) (getPixel(this->heightmap, x, y) & 0xff)) / this->atenuation); // ça ne fonctionne pas avec

        glTexCoord2d(x, y + 1);
        glVertex3f(x, y + 1, this->heights[x][y + 1] / this->atenuation);
        //glVertex3f(x, y + 1, ((float16) (getPixel(this->heightmap, x, y + 1) & 0xff)) / this->atenuation); // ça ne fonctionne pas avec
      }
      glEnd();
    }
  }
}

float16 Heightmap::getHeight(float16 x, float16 y)
{
  // algo de Seb

  float grid_square_size = this->height / ((float) this->heights.size()); // 1.0

  int x_grid = (int) floor(x / grid_square_size);
  int y_grid = (int) floor(y / grid_square_size);

  if (x_grid >= this->width || y_grid >= this->height || x_grid < 0 || y_grid < 0) {
    return 0;
  }

  float16 xCoord = fmod(x, grid_square_size) / grid_square_size;
  float16 yCoord = fmod(y, grid_square_size) / grid_square_size;

  //printf("x1: %f x2: %f\n", ((float16)(getPixel(this->heightmap, x, y) & 0xff)), this->hauteurs[x][y]);
  //printf("x1: %f x2: %f\n", x_grid, xCoord);

  if (xCoord <= (1 - yCoord))
  {
    Point3Float p1 = { 0, 0, ((float16) (getPixel(this->heightmap, x_grid, y_grid) & 0xff)) / this->atenuation };
    Point3Float p2 = { 1, 0, ((float16) (getPixel(this->heightmap, x_grid + 1, y_grid) & 0xff)) / this->atenuation };
    Point3Float p3 = { 0, 1, ((float16) (getPixel(this->heightmap, x_grid, y_grid + 1) & 0xff)) / this->atenuation };
    Point2Float pos = { xCoord, yCoord };

    return barycenter(p1, p2, p3, pos);
  }
  else
  {
    Point3Float p1 = { 1, 0, ((float16) (getPixel(this->heightmap, x_grid + 1, y_grid) & 0xff)) / this->atenuation };
    Point3Float p2 = { 1, 1, ((float16) (getPixel(this->heightmap, x_grid + 1, y_grid + 1) & 0xff)) / this->atenuation };
    Point3Float p3 = { 0, 1, ((float16) (getPixel(this->heightmap, x_grid, y_grid + 1) & 0xff)) / this->atenuation };
    Point2Float pos = { xCoord, yCoord };

    return barycenter(p1, p2, p3, pos);
  }

  /*
  // algo de David

  if (x >= this->width || y >= this->height || x < 0 || y < 0)
  {
    return 0;
  }

  //printf("x1: %f x2: %f\n", this->heights[x][y], ((float16) (getPixel(this->heightmap, x, y) & 0xff)));

  float16 a = (((float16)(getPixel(this->heightmap, x, y) & 0xff)) / this->atenuation);
  float16 b = (((float16)(getPixel(this->heightmap, x+1, y) & 0xff)) / this->atenuation);
  float16 c = (((float16)(getPixel(this->heightmap, x, y+1) & 0xff)) / this->atenuation);
  float16 d = (((float16)(getPixel(this->heightmap, x+1, y+1) & 0xff)) / this->atenuation);
  float16 e = a + (b - a) * (x - (uint32) x);
  float16 f = c + (d - c) * (x - (uint32) x);
  float16 g = e + (f - e) * (y - (uint32) y);
  return g;
  */
}
