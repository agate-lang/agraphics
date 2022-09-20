// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Julien Bernard
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cairo.h>

#include "agate.h"
#include "support.h"

#include "config.h"

/*
 * Tools
 */

static inline bool equals(const char *lhs, const char *rhs) {
  return strcmp(lhs, rhs) == 0;
}

static inline double min2(double x, double y) {
  return x < y ? x : y;
}

static inline double min3(double x, double y, double z) {
  return min2(x, min2(y, z));
}

static inline double max2(double x, double y) {
  return x > y ? x : y;
}

static inline double max3(double x, double y, double z) {
  return max2(x, max2(y, z));
}

/*
 * Vector2
 */

struct Vector2 {
  double x;
  double y;
};

// class

static ptrdiff_t agVector2Allocate(AgateVM *vm, const char *unit_name, const char *class_name) {
  return sizeof(struct Vector2);
}

// methods

static void agVector2New(AgateVM *vm) {
  struct Vector2 *vector = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  vector->x = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  vector->y = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
}

static void agVector2XGetter(AgateVM *vm) {
  struct Vector2 *vector = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), vector->x);
}

static void agVector2XSetter(AgateVM *vm) {
  struct Vector2 *vector = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  vector->x = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), vector->x);
}

static void agVector2YGetter(AgateVM *vm) {
  struct Vector2 *vector = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), vector->y);
}

static void agVector2YSetter(AgateVM *vm) {
  struct Vector2 *vector = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  vector->y = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), vector->y);
}

/*
 * Color
 */

struct Color {
  double r;
  double g;
  double b;
  double a;
};

// class

static ptrdiff_t agColorAllocate(AgateVM *vm, const char *unit_name, const char *class_name) {
  return sizeof(struct Color);
}

// methods

static void agColorNew(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  color->r = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  color->g = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  color->b = agateSlotGetFloat(vm, agateSlotForArg(vm, 3));
  color->a = agateSlotGetFloat(vm, agateSlotForArg(vm, 4));
}

static void agColorRGetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->r);
}

static void agColorRSetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  color->r = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->r);
}

static void agColorGGetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->g);
}

static void agColorGSetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  color->g = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->g);
}

static void agColorBGetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->b);
}

static void agColorBSetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  color->b = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->b);
}

static void agColorAGetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->a);
}

static void agColorASetter(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  color->a = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  agateSlotSetFloat(vm, agateSlotForReturn(vm), color->a);
}

struct HSV {
  double h;
  double s;
  double v;
  double a;
};

static void agConvertRgbToHsv(struct HSV *hsv, const struct Color *color) {
  const double r = color->r;
  const double g = color->g;
  const double b = color->b;

  const double min = min3(color->r, color->g, color->b);
  const double max = max3(color->r, color->g, color->b);

  if ((max - min) > DBL_EPSILON) {
    if (max == r) {
      hsv->h = fmod(60 * (g - b) / (max - min) + 360, 360);
    } else if (max == g) {
      hsv->h = 60 * (b - r) / (max - min) + 120;
    } else if (max == b) {
      hsv->h = 60 * (r - g) / (max - min) + 240;
    } else {
      assert(false);
    }
  } else {
    hsv->h = 0;
  }

  hsv->s = (max < DBL_EPSILON ? 0 : (1 - min / max));
  hsv->v = max;
  hsv->a = color->a;
}

static void agConvertHsvToRgb(struct Color *color, const struct HSV *hsv) {
  const double h = hsv->h / 60;
  const double s = hsv->s;
  const double v = hsv->v;

  const int i = ((int) h) % 6;
  assert(0 <= i && i < 6);

  const double f = h - (double) i;
  const double x = v * (1 - s);
  const double y = v * (1 - (f * s));
  const double z = v * (1 - (1 - f) * s);

  switch (i) {
    case 0: color->r = v; color->g = z; color->b = x; break;
    case 1: color->r = y; color->g = v; color->b = x; break;
    case 2: color->r = x; color->g = v; color->b = z; break;
    case 3: color->r = x; color->g = y; color->b = v; break;
    case 4: color->r = z; color->g = x; color->b = v; break;
    case 5: color->r = v; color->g = x; color->b = y; break;
    default: assert(false); break;
  }

  color->a = hsv->a;
}

static void agColorDarker(AgateVM *vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double percent = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));

  struct HSV hsv;
  agConvertRgbToHsv(&hsv, color);
  hsv.v -= hsv.v * percent;
  agConvertHsvToRgb(color, &hsv);
}

static void agColorLighter(AgateVM* vm) {
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double percent = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));

  struct HSV hsv;
  agConvertRgbToHsv(&hsv, color);
  hsv.v += hsv.v * percent;

  if (hsv.v > 1) {
    hsv.s -= (hsv.v - 1);

    if (hsv.s < 0) {
      hsv.s = 0;
    }

    hsv.v = 1;
  }

  agConvertHsvToRgb(color, &hsv);
}

/*
 * Surface
 */

struct Surface {
  cairo_surface_t *ptr;
};

// class

static ptrdiff_t agSurfaceAllocate(AgateVM *vm, const char *unit_name, const char *class_name) {
  return sizeof(struct Surface);
}

void agSurfaceDestroy(AgateVM *vm, const char *unit_name, const char *class_name, void *data) {
  struct Surface *surface = data;

  if (surface->ptr != NULL) {
    cairo_surface_destroy(surface->ptr);
  }

  surface->ptr = NULL;
}

// methods

static void agSurfaceNew(AgateVM *vm) {
  struct Surface *surface = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Vector2 *size = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  surface->ptr = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size->x, size->y);
  assert(surface->ptr);
}

static void agSurfaceNewFromPng(AgateVM *vm) {
  struct Surface *surface = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  const char *filename = agateSlotGetString(vm, agateSlotForArg(vm, 1));
  surface->ptr = cairo_image_surface_create_from_png(filename);
  assert(surface->ptr);
}

static void agSurfaceExport(AgateVM *vm) {
  struct Surface *surface = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  const char *filename = agateSlotGetString(vm, agateSlotForArg(vm, 1));
  cairo_status_t status = cairo_surface_write_to_png(surface->ptr, filename);

  if (status != CAIRO_STATUS_SUCCESS) {
    fprintf(stderr, "Error: %s\n", cairo_status_to_string(status));
  }
}

/*
 * Pattern
 */

struct Pattern {
  cairo_pattern_t *ptr;
};

// class

static ptrdiff_t agPatternAllocate(AgateVM *vm, const char *unit_name, const char *class_name) {
  return sizeof(struct Pattern);
}

void agPatternDestroy(AgateVM *vm, const char *unit_name, const char *class_name, void *data) {
  struct Pattern *pattern = data;

  if (pattern->ptr != NULL) {
    cairo_pattern_destroy(pattern->ptr);
  }

  pattern->ptr = NULL;
}

// methods

static void agSolidPatternNew(AgateVM *vm) {
  struct Pattern *pattern = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  pattern->ptr = cairo_pattern_create_rgba(color->r, color->g, color->b, color->a);
}

static void agSurfacePatternNew(AgateVM *vm) {
  struct Pattern *pattern = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Surface *surface = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  pattern->ptr = cairo_pattern_create_for_surface(surface->ptr);
}

static void agGradientPatternAddColor(AgateVM *vm) {
  struct Pattern *pattern = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double offset = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 2));
  cairo_pattern_add_color_stop_rgba(pattern->ptr, offset, color->r, color->g, color->b, color->a);
}

static void agLinearGradientPatternNew(AgateVM *vm) {
  struct Pattern *pattern = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Vector2 *p0 = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  struct Vector2 *p1 = agateSlotGetForeign(vm, agateSlotForArg(vm, 2));
  pattern->ptr = cairo_pattern_create_linear(p0->x, p0->y, p1->x, p1->y);
}

static void agRadialGradientPatternNew(AgateVM *vm) {
  struct Pattern *pattern = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Vector2 *c0 = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  double r0 = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  struct Vector2 *c1 = agateSlotGetForeign(vm, agateSlotForArg(vm, 3));
  double r1 = agateSlotGetFloat(vm, agateSlotForArg(vm, 4));
  pattern->ptr = cairo_pattern_create_radial(c0->x, c0->y, r0, c1->x, c1->y, r1);
}

/*
 * Context
 */

struct Context {
  cairo_t *ptr;
};

// class

static ptrdiff_t agContextAllocate(AgateVM *vm, const char *unit_name, const char *class_name) {
  return sizeof(struct Context);
}

void agContextDestroy(AgateVM *vm, const char *unit_name, const char *class_name, void *data) {
  struct Context *context = data;

  if (context->ptr != NULL) {
    cairo_destroy(context->ptr);
  }

  context->ptr = NULL;
}

// methods

static void agContextNew(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Surface *surface = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  context->ptr = cairo_create(surface->ptr);
}

static void agContextSave(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  cairo_save(context->ptr);
}

static void agContextRestore(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  cairo_restore(context->ptr);
}

static void agContextTranslate(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double tx = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  double ty = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  cairo_translate(context->ptr, tx, ty);
}

static void agContextScale(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double sx = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  double sy = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  cairo_scale(context->ptr, sx, sy);
}

static void agContextRotate(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double angle = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  cairo_rotate(context->ptr, angle);
}

// source

static void agContextSetSourceColor(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Color *color = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  cairo_set_source_rgba(context->ptr, color->r, color->g, color->b, color->a);
}

static void agContextSetSourceSurface(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Surface *surface = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  double x = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  double y = agateSlotGetFloat(vm, agateSlotForArg(vm, 3));
  cairo_set_source_surface(context->ptr, surface->ptr, x, y);
}

static void agContextSetSourcePattern(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  struct Pattern *pattern = agateSlotGetForeign(vm, agateSlotForArg(vm, 1));
  cairo_set_source(context->ptr, pattern->ptr);
}

// style

static void agContextSetAntialias(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  int64_t raw = agateSlotGetInt(vm, agateSlotForArg(vm, 1));
  cairo_set_antialias(context->ptr, (cairo_antialias_t) raw);
}

static void agContextSetFillRule(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  int64_t raw = agateSlotGetInt(vm, agateSlotForArg(vm, 1));
  cairo_set_fill_rule(context->ptr, (cairo_fill_rule_t) raw);
}

static void agContextSetLineCap(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  int64_t raw = agateSlotGetInt(vm, agateSlotForArg(vm, 1));
  cairo_set_line_cap(context->ptr, (cairo_line_cap_t) raw);
}

static void agContextSetLineJoin(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  int64_t raw = agateSlotGetInt(vm, agateSlotForArg(vm, 1));
  cairo_set_line_join(context->ptr, (cairo_line_join_t) raw);
}

static void agContextSetLineWidth(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double width = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  cairo_set_line_width(context->ptr, width);
}

static void agContextSetMiterLimit(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double limit = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  cairo_set_miter_limit(context->ptr, limit);
}

static void agContextSetOperator(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  int64_t raw = agateSlotGetInt(vm, agateSlotForArg(vm, 1));
  cairo_set_operator(context->ptr, (cairo_operator_t) raw);
}

// draw

static void agContextClip(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  bool preserve = agateSlotGetBool(vm, agateSlotForArg(vm, 1));

  if (preserve) {
    cairo_clip_preserve(context->ptr);
  } else {
    cairo_clip(context->ptr);
  }
}

static void agContextFill(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  bool preserve = agateSlotGetBool(vm, agateSlotForArg(vm, 1));

  if (preserve) {
    cairo_fill_preserve(context->ptr);
  } else {
    cairo_fill(context->ptr);
  }
}

static void agContextStroke(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  bool preserve = agateSlotGetBool(vm, agateSlotForArg(vm, 1));

  if (preserve) {
    cairo_stroke_preserve(context->ptr);
  } else {
    cairo_stroke(context->ptr);
  }
}

static void agContextPaint(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  cairo_paint(context->ptr);
}

// path

static void agContextMoveTo(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double x = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  double y = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  cairo_move_to(context->ptr, x, y);
}

static void agContextLineTo(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double x = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  double y = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  cairo_line_to(context->ptr, x, y);
}

static void agContextCurveTo(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double x1 = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  double y1 = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  double x2 = agateSlotGetFloat(vm, agateSlotForArg(vm, 3));
  double y2 = agateSlotGetFloat(vm, agateSlotForArg(vm, 4));
  double x3 = agateSlotGetFloat(vm, agateSlotForArg(vm, 5));
  double y3 = agateSlotGetFloat(vm, agateSlotForArg(vm, 6));
  cairo_curve_to(context->ptr, x1, y1, x2, y2, x3, y3);
}

static void agContextClosePath(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  cairo_close_path(context->ptr);
}

static void agContextArc(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double xc = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  double yc = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  double radius = agateSlotGetFloat(vm, agateSlotForArg(vm, 3));
  double angle1 = agateSlotGetFloat(vm, agateSlotForArg(vm, 4));
  double angle2 = agateSlotGetFloat(vm, agateSlotForArg(vm, 5));
  cairo_arc(context->ptr, xc, yc, radius, angle1, angle2);
}

static void agContextArcNegative(AgateVM *vm) {
  struct Context *context = agateSlotGetForeign(vm, agateSlotForArg(vm, 0));
  double xc = agateSlotGetFloat(vm, agateSlotForArg(vm, 1));
  double yc = agateSlotGetFloat(vm, agateSlotForArg(vm, 2));
  double radius = agateSlotGetFloat(vm, agateSlotForArg(vm, 3));
  double angle1 = agateSlotGetFloat(vm, agateSlotForArg(vm, 4));
  double angle2 = agateSlotGetFloat(vm, agateSlotForArg(vm, 5));
  cairo_arc_negative(context->ptr, xc, yc, radius, angle1, angle2);
}


/*
 * Agate configuration
 */

// static void agNotImplemented(AgateVM *vm) {
// }

static AgateForeignClassHandler agClassHandler(AgateVM *vm, const char *unit_name, const char *class_name) {
  assert(equals(unit_name, "agraphics"));
  AgateForeignClassHandler handler = { NULL, NULL };

  if (equals(class_name, "Vector2")) {
    handler.allocate = agVector2Allocate;
    return handler;
  }

  if (equals(class_name, "Color")) {
    handler.allocate = agColorAllocate;
    return handler;
  }

  if (equals(class_name, "Surface")) {
    handler.allocate = agSurfaceAllocate;
    handler.destroy = agSurfaceDestroy;
    return handler;
  }

  if (equals(class_name, "Context")) {
    handler.allocate = agContextAllocate;
    handler.destroy = agContextDestroy;
    return handler;
  }

  if (equals(class_name, "SolidPattern") || equals(class_name, "SurfacePattern") || equals(class_name, "LinearGradientPattern") || equals(class_name, "RadialGradientPattern")) {
    handler.allocate = agPatternAllocate;
    handler.destroy = agPatternDestroy;
    return handler;
  }

  return handler;
}

static AgateForeignMethodFunc agMethodHandler(AgateVM *vm, const char *unit_name, const char *class_name, AgateForeignMethodKind kind, const char *signature) {
  assert(equals(unit_name, "agraphics"));

  if (equals(class_name, "Vector2")) {
    if (equals(signature, "init new(_,_)")) { return agVector2New; }
    if (equals(signature, "x")) { return agVector2XGetter; }
    if (equals(signature, "x=(_)")) { return agVector2XSetter; }
    if (equals(signature, "y")) { return agVector2YGetter; }
    if (equals(signature, "y=(_)")) { return agVector2YSetter; }
  }

  if (equals(class_name, "Color")) {
    if (equals(signature, "init new(_,_,_,_)")) { return agColorNew; }
    if (equals(signature, "r")) { return agColorRGetter; }
    if (equals(signature, "r=(_)")) { return agColorRSetter; }
    if (equals(signature, "g")) { return agColorGGetter; }
    if (equals(signature, "g=(_)")) { return agColorGSetter; }
    if (equals(signature, "b")) { return agColorBGetter; }
    if (equals(signature, "b=(_)")) { return agColorBSetter; }
    if (equals(signature, "a")) { return agColorAGetter; }
    if (equals(signature, "a=(_)")) { return agColorASetter; }
    if (equals(signature, "darker(_)")) { return agColorDarker; }
    if (equals(signature, "lighter(_)")) { return agColorLighter; }
  }

  if (equals(class_name, "Surface")) {
    if (equals(signature, "init new(_)")) { return agSurfaceNew; }
    if (equals(signature, "init new_from_png(_)")) { return agSurfaceNewFromPng; }
    if (equals(signature, "export(_)")) { return agSurfaceExport; }
  }

  if (equals(class_name, "SolidPattern")) {
    if (equals(signature, "init new(_)")) { return agSolidPatternNew; }
  }

  if (equals(class_name, "SurfacePattern")) {
    if (equals(signature, "init new(_)")) { return agSurfacePatternNew; }
  }

  if (equals(class_name, "GradientPattern")) {
    if (equals(signature, "add_color_stop(_,_)")) { return agGradientPatternAddColor; }
  }

  if (equals(class_name, "LinearGradientPattern")) {
    if (equals(signature, "init new(_,_)")) { return agLinearGradientPatternNew; }
  }

  if (equals(class_name, "RadialGradientPattern")) {
    if (equals(signature, "init new(_,_,_,_)")) { return agRadialGradientPatternNew; }
  }

  if (equals(class_name, "Context")) {
    if (equals(signature, "init new(_)")) { return agContextNew; }
    if (equals(signature, "save()")) { return agContextSave; }
    if (equals(signature, "restore()")) { return agContextRestore; }
    if (equals(signature, "translate(_,_)")) { return agContextTranslate; }
    if (equals(signature, "scale(_,_)")) { return agContextScale; }
    if (equals(signature, "rotate(_)")) { return agContextRotate; }
    if (equals(signature, "set_source_color(_)")) { return agContextSetSourceColor; }
    if (equals(signature, "set_source_surface(_,_,_)")) { return agContextSetSourceSurface; }
    if (equals(signature, "set_source_pattern(_)")) { return agContextSetSourcePattern; }
    if (equals(signature, "set_antialias(_)")) { return agContextSetAntialias; }
    if (equals(signature, "set_fill_rule(_)")) { return agContextSetFillRule; }
    if (equals(signature, "set_line_cap(_)")) { return agContextSetLineCap; }
    if (equals(signature, "set_line_join(_)")) { return agContextSetLineJoin; }
    if (equals(signature, "set_line_width(_)")) { return agContextSetLineWidth; }
    if (equals(signature, "set_miter_limit(_)")) { return agContextSetMiterLimit; }
    if (equals(signature, "set_operator(_)")) { return agContextSetOperator; }
    if (equals(signature, "clip(_)")) { return agContextClip; }
    if (equals(signature, "fill(_)")) { return agContextFill; }
    if (equals(signature, "stroke(_)")) { return agContextStroke; }
    if (equals(signature, "paint()")) { return agContextPaint; }
    if (equals(signature, "move_to(_,_)")) { return agContextMoveTo; }
    if (equals(signature, "line_to(_,_)")) { return agContextLineTo; }
    if (equals(signature, "curve_to(_,_,_,_,_,_)")) { return agContextCurveTo; }
    if (equals(signature, "close_path()")) { return agContextClosePath; }
    if (equals(signature, "arc(_,_,_,_,_)")) { return agContextArc; }
    if (equals(signature, "arc_negative(_,_,_,_,_)")) { return agContextArcNegative; }
  }

  return NULL;
}

static void usage(void) {
  printf("Usage: agraphics <unit>\n");
}

static void print(AgateVM *vm, const char* text) {
  fputs(text, stdout);
}

static void write(AgateVM *vm, uint8_t byte) {
  fputc(byte, stdout);
}

static void error(AgateVM *vm, AgateErrorKind kind, const char *unit_name, int line, const char *message) {
  switch (kind) {
    case AGATE_ERROR_COMPILE:
      fprintf(stderr, "%s:%d: error: %s\n", unit_name, line, message);
      break;
    case AGATE_ERROR_RUNTIME:
      fprintf(stderr, "error: %s\n", message);
      break;
    case AGATE_ERROR_STACKTRACE:
      fprintf(stderr, "%s:%d: in %s\n", unit_name, line, message);
      break;
  }
}

static void input(AgateVM *vm, char *buffer, size_t size) {
  fgets(buffer, size, stdin);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage();
    return EXIT_FAILURE;
  }

  AgateConfig config;
  agateConfigInitialize(&config);

  config.unit_handler = agateExUnitHandler;
  config.foreign_class_handler = agateExForeignClassHandler;
  config.foreign_method_handler = agateExForeignMethodHandler;

  config.print = print;
  config.write = write;
  config.error = error;
  config.input = input;

  AgateVM *vm = agateExNewVM(&config);

  agateExUnitAddIncludePath(vm, AGRAPHICS_UNIT_DIRECTORY);
  agateExUnitAddIncludePath(vm, ".");

  agateExForeignClassAddHandler(vm, agClassHandler, "agraphics");
  agateExForeignMethodAddHandler(vm, agMethodHandler, "agraphics");

  const char *source = agateExUnitLoad(vm, argv[1]);

  if (source != NULL) {
    AgateStatus status = agateCallString(vm, argv[1], source);
    agateExUnitRelease(vm, source);

    if (status != AGATE_STATUS_OK) {
      fprintf(stderr, "Error in the agraphics unit '%s'.\n", argv[1]);
    }
  } else {
    fprintf(stderr, "Could not find agraphics unit '%s'.\n", argv[1]);
  }

  agateExDeleteVM(vm);

  return EXIT_SUCCESS;
}
