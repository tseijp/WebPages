#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "img.h"

static unsigned char buf[HEIGHT][WIDTH][3];
static int filecnt = 0;
static char fname[100];

void img_clear(void) {
  int i, j;
  for(j = 0; j < HEIGHT; ++j) {
    for(i = 0; i < WIDTH; ++i) {
      buf[j][i][0] = buf[j][i][1] = buf[j][i][2] = 255;
    }
  }
}
void img_write(void) {
  sprintf(fname, "img/img%04d.ppm", ++filecnt);
  FILE *f = fopen(fname, "wb");
  if(f == NULL) { fprintf(stderr, "can't open %s\n", fname); exit(1); }
  fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
  fwrite(buf, sizeof(buf), 1, f);
  fclose(f);
}
void img_putpixel(struct color c, int x, int y) {
  if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) { return; }
  buf[HEIGHT-y-1][x][0] = c.r;
  buf[HEIGHT-y-1][x][1] = c.g;
  buf[HEIGHT-y-1][x][2] = c.b;
}
void img_fillcircle(struct color c, double x, double y, double r) {
  int imin = (int)(x - r - 1), imax = (int)(x + r + 1);
  int jmin = (int)(y - r - 1), jmax = (int)(y + r + 1);
  int i, j;
  for(j = jmin; j <= jmax; ++j) {
    for(i = imin; i <= imax; ++i) {
      if((x-i)*(x-i) + (y-j)*(y-j) <= r*r) { img_putpixel(c, i, j); }
    }
  }
}
void img_rect(struct color c, double x, double y, double r) {
  int imin = (int)(x - r - 1), imax = (int)(x + r + 1);
  int jmin = (int)(y - r - 1), jmax = (int)(y + r + 1);
  int i, j;
  for(j = jmin; j <= jmax; ++j) {
    for(i = imin; i <= imax; ++i) {
      img_putpixel(c, i, j);
    }
  }
}

//
static double oprod(double a, double b, double c, double d) {
  return a*d - b*c;
}

static int isinside(double x, double y, int n, double ax[], double ay[]) {
  int i;
  for(i = 0; i < n; ++i) {
    if(oprod(ax[i+1]-ax[i], ay[i+1]-ay[i], x-ax[i], y-ay[i]) < 0) { return 0; }
  }
  return 1;
}

static double amax(int n, double a[]) {
  double m = a[0];
  int i;
  for(i = 0; i < n; ++i) { if(m < a[i]) { m = a[i]; } }
  return m;
}

static double amin(int n, double a[]) {
  double m = a[0];
  int i;
  for(i = 0; i < n; ++i) { if(m > a[i]) { m = a[i]; } }
  return m;
}

void img_fillconvex(struct color c, int n, double ax[], double ay[]) {
  int xmax = (int)(amax(n, ax)+1), xmin = (int)(amin(n, ax)-1);
  int ymax = (int)(amax(n, ay)+1), ymin = (int)(amin(n, ay)-1);
  int i, j;
  for(i = xmin; i <= xmax; ++i) {
    for(j = ymin; j <= ymax; ++j) {
      if(isinside(i, j, n, ax, ay)) { img_putpixel(c, i, j); }
    }
  }
}

void img_filltriangle(struct color c, double x0, double y0, double x1, double y1, double x2, double y2) {
  double ax1[] = { x0, x1, x2, x0 }, ax2[] = { x0, x2, x1, x0 };
  double ay1[] = { y0, y1, y2, y0 }, ay2[] = { y0, y2, y1, y0 };
  img_fillconvex(c, 3, ax1, ay1);
  img_fillconvex(c, 3, ax2, ay2);
}

void img_fillline(struct color c, double x0, double y0,double x1, double y1, double w) {
  double dx = y1-y0, dy = x0-x1, n = 0.5*w / sqrt(dx*dx + dy*dy);
  dx *= n; dy *= n;
  double ax[] = { x0-dx, x0+dx, x1+dx, x1-dx, x0-dx };
  double ay[] = { y0-dy, y0+dy, y1+dy, y1-dy, y0-dy };
  img_fillconvex(c, 4, ax, ay);
}
