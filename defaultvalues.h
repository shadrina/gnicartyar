#ifndef DEFAULTVALUES_H
#define DEFAULTVALUES_H

#include <cmath>
#include "geometry.h"

static const int WIDTH = 1024;
static const int HEIGHT = 720;

static constexpr double FOV = M_PI / 2.3;
static constexpr double DEPTH = 3;
static constexpr double WF_MODE_DEPTH = 3;

static constexpr double OFFSET = 1e-3;

static const Point3D BACKGROUND({.1, .8, .8});
static const Point3D SCENE_COLOR({.2, .2, .2});

static constexpr double ZN = -10 + OFFSET;
static constexpr double ZF = -30 - OFFSET;

static constexpr double SCENE_Y_BOTTOM = -4. - OFFSET;
static constexpr double SCENE_HEIGHT = 12. + 2 * OFFSET;
static constexpr double SCENE_X_LEFT = -10. - OFFSET;
static constexpr double SCENE_WIDTH = 20. + 2 * OFFSET;


#endif // DEFAULTVALUES_H
