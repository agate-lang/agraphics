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

#define AG_VECTOR2_TAG  0x1000
#define AG_MATRIX_TAG   0x1001
#define AG_COLOR_TAG    0x1002
#define AG_SURFACE_TAG  0x1003
#define AG_PATTERN_TAG  0x1004
#define AG_CONTEXT_TAG  0x1004

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

static uint64_t agVector2Tag(AgateVM *vm, const char *unit_name, const char *class_name) {
  return AG_VECTOR2_TAG;
}

// methods

static void agVector2New(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_VECTOR2_TAG);
  struct Vector2 *vector = agateSlotGetForeign(vm, 0);
  vector->x = agateSlotGetFloat(vm, 1);
  vector->y = agateSlotGetFloat(vm, 2);
}

static void agVector2XGetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_VECTOR2_TAG);
  struct Vector2 *vector = agateSlotGetForeign(vm, 0);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, vector->x);
}

static void agVector2XSetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_VECTOR2_TAG);
  struct Vector2 *vector = agateSlotGetForeign(vm, 0);
  vector->x = agateSlotGetFloat(vm, 1);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, vector->x);
}

static void agVector2YGetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_VECTOR2_TAG);
  struct Vector2 *vector = agateSlotGetForeign(vm, 0);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, vector->y);
}

static void agVector2YSetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_VECTOR2_TAG);
  struct Vector2 *vector = agateSlotGetForeign(vm, 0);
  vector->y = agateSlotGetFloat(vm, 1);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, vector->y);
}

/*
 * Matrix
 */

// class

static ptrdiff_t agMatrixAllocate(AgateVM *vm, const char *unit_name, const char *class_name) {
  return sizeof(cairo_matrix_t);
}

static uint64_t agMatrixTag(AgateVM *vm, const char *unit_name, const char *class_name) {
  return AG_MATRIX_TAG;
}

// methods

static void agMatrixNew(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  cairo_matrix_init_identity(matrix);
}

static void agMatrixNewTranslate(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  double tx = agateSlotGetFloat(vm, 1);
  double ty = agateSlotGetFloat(vm, 2);
  cairo_matrix_init_translate(matrix, tx, ty);
}

static void agMatrixNewScale(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  double sx = agateSlotGetFloat(vm, 1);
  double sy = agateSlotGetFloat(vm, 2);
  cairo_matrix_init_scale(matrix, sx, sy);
}

static void agMatrixNewRotate(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  double angle = agateSlotGetFloat(vm, 1);
  cairo_matrix_init_rotate(matrix, angle);
}

static void agMatrixTranslate(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  double tx = agateSlotGetFloat(vm, 1);
  double ty = agateSlotGetFloat(vm, 2);
  cairo_matrix_translate(matrix, tx, ty);
}

static void agMatrixScale(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  double sx = agateSlotGetFloat(vm, 1);
  double sy = agateSlotGetFloat(vm, 2);
  cairo_matrix_scale(matrix, sx, sy);
}

static void agMatrixRotate(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  double angle = agateSlotGetFloat(vm, 1);
  cairo_matrix_rotate(matrix, angle);
}

static void agMatrixInvert(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);

  if (cairo_matrix_invert(matrix) != CAIRO_STATUS_SUCCESS) {
    ptrdiff_t string_slot = agateSlotAllocate(vm);
    agateSlotSetString(vm, string_slot, "Unable to invert the matrix");
    agateAbort(vm, string_slot);
  }
}

static void agMatrixMultiply(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_MATRIX_TAG);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_MATRIX_TAG);
  cairo_matrix_t *other = agateSlotGetForeign(vm, 1);

  ptrdiff_t class_slot = agateSlotAllocate(vm);
  agateGetVariable(vm, "agraphics", "Matrix", class_slot);

  ptrdiff_t result_slot = agateSlotAllocate(vm);
  cairo_matrix_t *result = agateSlotSetForeign(vm, result_slot, class_slot);
  cairo_matrix_multiply(result, matrix, other);

  agateSlotCopy(vm, AGATE_RETURN_SLOT, result_slot);
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

static uint64_t agColorTag(AgateVM *vm, const char *unit_name, const char *class_name) {
  return AG_COLOR_TAG;
}

// methods

static void agColorNew(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  color->r = agateSlotGetFloat(vm, 1);
  color->g = agateSlotGetFloat(vm, 2);
  color->b = agateSlotGetFloat(vm, 3);
  color->a = agateSlotGetFloat(vm, 4);
}

static void agColorRGetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->r);
}

static void agColorRSetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  color->r = agateSlotGetFloat(vm, 1);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->r);
}

static void agColorGGetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->g);
}

static void agColorGSetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  color->g = agateSlotGetFloat(vm, 1);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->g);
}

static void agColorBGetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->b);
}

static void agColorBSetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  color->b = agateSlotGetFloat(vm, 1);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->b);
}

static void agColorAGetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->a);
}

static void agColorASetter(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  color->a = agateSlotGetFloat(vm, 1);
  agateSlotSetFloat(vm, AGATE_RETURN_SLOT, color->a);
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
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  double percent = agateSlotGetFloat(vm, 1);

  struct HSV hsv;
  agConvertRgbToHsv(&hsv, color);
  hsv.v -= hsv.v * percent;
  agConvertHsvToRgb(color, &hsv);
}

static void agColorLighter(AgateVM* vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 0);
  double percent = agateSlotGetFloat(vm, 1);

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

static uint64_t agSurfaceTag(AgateVM *vm, const char *unit_name, const char *class_name) {
  return AG_SURFACE_TAG;
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
  assert(agateSlotGetForeignTag(vm, 0) == AG_SURFACE_TAG);
  struct Surface *surface = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_VECTOR2_TAG);
  struct Vector2 *size = agateSlotGetForeign(vm, 1);
  surface->ptr = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size->x, size->y);
  assert(surface->ptr);
}

static void agSurfaceNewFromPng(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_SURFACE_TAG);
  struct Surface *surface = agateSlotGetForeign(vm, 0);
  const char *filename = agateSlotGetString(vm, 1);
  surface->ptr = cairo_image_surface_create_from_png(filename);
  assert(surface->ptr);
}

static void agSurfaceExport(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_SURFACE_TAG);
  struct Surface *surface = agateSlotGetForeign(vm, 0);
  const char *filename = agateSlotGetString(vm, 1);
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

static uint64_t agPatternTag(AgateVM *vm, const char *unit_name, const char *class_name) {
  return AG_PATTERN_TAG;
}

void agPatternDestroy(AgateVM *vm, const char *unit_name, const char *class_name, void *data) {
  struct Pattern *pattern = data;

  if (pattern->ptr != NULL) {
    cairo_pattern_destroy(pattern->ptr);
  }

  pattern->ptr = NULL;
}

// methods

static void agPatternSetMatrix(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_PATTERN_TAG);
  struct Pattern *pattern = agateSlotGetForeign(vm, 0);
  cairo_matrix_t *matrix = agateSlotGetForeign(vm, 1);
  cairo_pattern_set_matrix(pattern->ptr, matrix);
}

static void agSolidPatternNew(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_PATTERN_TAG);
  struct Pattern *pattern = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 1);
  pattern->ptr = cairo_pattern_create_rgba(color->r, color->g, color->b, color->a);
}

static void agSurfacePatternNew(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_PATTERN_TAG);
  struct Pattern *pattern = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_SURFACE_TAG);
  struct Surface *surface = agateSlotGetForeign(vm, 1);
  pattern->ptr = cairo_pattern_create_for_surface(surface->ptr);
}

static void agGradientPatternAddColor(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_PATTERN_TAG);
  struct Pattern *pattern = agateSlotGetForeign(vm, 0);
  double offset = agateSlotGetFloat(vm, 1);
  assert(agateSlotGetForeignTag(vm, 2) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 2);
  cairo_pattern_add_color_stop_rgba(pattern->ptr, offset, color->r, color->g, color->b, color->a);
}

static void agLinearGradientPatternNew(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_PATTERN_TAG);
  struct Pattern *pattern = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_VECTOR2_TAG);
  struct Vector2 *p0 = agateSlotGetForeign(vm, 1);
  assert(agateSlotGetForeignTag(vm, 2) == AG_VECTOR2_TAG);
  struct Vector2 *p1 = agateSlotGetForeign(vm, 2);
  pattern->ptr = cairo_pattern_create_linear(p0->x, p0->y, p1->x, p1->y);
}

static void agRadialGradientPatternNew(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_PATTERN_TAG);
  struct Pattern *pattern = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_VECTOR2_TAG);
  struct Vector2 *c0 = agateSlotGetForeign(vm, 1);
  double r0 = agateSlotGetFloat(vm, 2);
  assert(agateSlotGetForeignTag(vm, 3) == AG_VECTOR2_TAG);
  struct Vector2 *c1 = agateSlotGetForeign(vm, 3);
  double r1 = agateSlotGetFloat(vm, 4);
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

static uint64_t agContextTag(AgateVM *vm, const char *unit_name, const char *class_name) {
  return AG_CONTEXT_TAG;
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
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_SURFACE_TAG);
  struct Surface *surface = agateSlotGetForeign(vm, 1);
  context->ptr = cairo_create(surface->ptr);
}

static void agContextSave(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  cairo_save(context->ptr);
}

static void agContextRestore(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  cairo_restore(context->ptr);
}

// group

static void agContextPushGroup(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  cairo_push_group(context->ptr);
}

static void agContextPopGroupToSource(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  cairo_pop_group_to_source(context->ptr);
}

// matrix

static void agContextTranslate(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double tx = agateSlotGetFloat(vm, 1);
  double ty = agateSlotGetFloat(vm, 2);
  cairo_translate(context->ptr, tx, ty);
}

static void agContextScale(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double sx = agateSlotGetFloat(vm, 1);
  double sy = agateSlotGetFloat(vm, 2);
  cairo_scale(context->ptr, sx, sy);
}

static void agContextRotate(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double angle = agateSlotGetFloat(vm, 1);
  cairo_rotate(context->ptr, angle);
}

// source

static void agContextSetSourceColor(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_COLOR_TAG);
  struct Color *color = agateSlotGetForeign(vm, 1);
  cairo_set_source_rgba(context->ptr, color->r, color->g, color->b, color->a);
}

static void agContextSetSourceSurface(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_SURFACE_TAG);
  struct Surface *surface = agateSlotGetForeign(vm, 1);
  double x = agateSlotGetFloat(vm, 2);
  double y = agateSlotGetFloat(vm, 3);
  cairo_set_source_surface(context->ptr, surface->ptr, x, y);
}

static void agContextSetSourcePattern(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  assert(agateSlotGetForeignTag(vm, 1) == AG_PATTERN_TAG);
  struct Pattern *pattern = agateSlotGetForeign(vm, 1);
  cairo_set_source(context->ptr, pattern->ptr);
}

// style

static void agContextSetAntialias(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  int64_t raw = agateSlotGetInt(vm, 1);
  cairo_set_antialias(context->ptr, (cairo_antialias_t) raw);
}

static void agContextSetFillRule(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  int64_t raw = agateSlotGetInt(vm, 1);
  cairo_set_fill_rule(context->ptr, (cairo_fill_rule_t) raw);
}

static void agContextSetLineCap(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  int64_t raw = agateSlotGetInt(vm, 1);
  cairo_set_line_cap(context->ptr, (cairo_line_cap_t) raw);
}

static void agContextSetLineJoin(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  int64_t raw = agateSlotGetInt(vm, 1);
  cairo_set_line_join(context->ptr, (cairo_line_join_t) raw);
}

static void agContextSetLineWidth(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double width = agateSlotGetFloat(vm, 1);
  cairo_set_line_width(context->ptr, width);
}

static void agContextSetMiterLimit(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double limit = agateSlotGetFloat(vm, 1);
  cairo_set_miter_limit(context->ptr, limit);
}

static void agContextSetOperator(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  int64_t raw = agateSlotGetInt(vm, 1);
  cairo_set_operator(context->ptr, (cairo_operator_t) raw);
}

// draw

static void agContextClip(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  bool preserve = agateSlotGetBool(vm, 1);

  if (preserve) {
    cairo_clip_preserve(context->ptr);
  } else {
    cairo_clip(context->ptr);
  }
}

static void agContextFill(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  bool preserve = agateSlotGetBool(vm, 1);

  if (preserve) {
    cairo_fill_preserve(context->ptr);
  } else {
    cairo_fill(context->ptr);
  }
}

static void agContextStroke(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  bool preserve = agateSlotGetBool(vm, 1);

  if (preserve) {
    cairo_stroke_preserve(context->ptr);
  } else {
    cairo_stroke(context->ptr);
  }
}

static void agContextPaint(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  cairo_paint(context->ptr);
}

static void agContextPaintWithAlpha(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double alpha = agateSlotGetFloat(vm, 1);
  cairo_paint_with_alpha(context->ptr, alpha);
}

// path

static void agContextMoveTo(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double x = agateSlotGetFloat(vm, 1);
  double y = agateSlotGetFloat(vm, 2);
  cairo_move_to(context->ptr, x, y);
}

static void agContextLineTo(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double x = agateSlotGetFloat(vm, 1);
  double y = agateSlotGetFloat(vm, 2);
  cairo_line_to(context->ptr, x, y);
}

static void agContextCurveTo(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double x1 = agateSlotGetFloat(vm, 1);
  double y1 = agateSlotGetFloat(vm, 2);
  double x2 = agateSlotGetFloat(vm, 3);
  double y2 = agateSlotGetFloat(vm, 4);
  double x3 = agateSlotGetFloat(vm, 5);
  double y3 = agateSlotGetFloat(vm, 6);
  cairo_curve_to(context->ptr, x1, y1, x2, y2, x3, y3);
}

static void agContextClosePath(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  cairo_close_path(context->ptr);
}

static void agContextRectangle(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double x = agateSlotGetFloat(vm, 1);
  double y = agateSlotGetFloat(vm, 2);
  double width = agateSlotGetFloat(vm, 3);
  double height = agateSlotGetFloat(vm, 4);
  cairo_rectangle(context->ptr, x, y, width, height);
}

static void agContextArc(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double xc = agateSlotGetFloat(vm, 1);
  double yc = agateSlotGetFloat(vm, 2);
  double radius = agateSlotGetFloat(vm, 3);
  double angle1 = agateSlotGetFloat(vm, 4);
  double angle2 = agateSlotGetFloat(vm, 5);
  cairo_arc(context->ptr, xc, yc, radius, angle1, angle2);
}

static void agContextArcNegative(AgateVM *vm) {
  assert(agateSlotGetForeignTag(vm, 0) == AG_CONTEXT_TAG);
  struct Context *context = agateSlotGetForeign(vm, 0);
  double xc = agateSlotGetFloat(vm, 1);
  double yc = agateSlotGetFloat(vm, 2);
  double radius = agateSlotGetFloat(vm, 3);
  double angle1 = agateSlotGetFloat(vm, 4);
  double angle2 = agateSlotGetFloat(vm, 5);
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
    handler.tag = agVector2Tag;
    return handler;
  }

  if (equals(class_name, "Matrix")) {
    handler.allocate = agMatrixAllocate;
    handler.tag = agMatrixTag;
    return handler;
  }

  if (equals(class_name, "Color")) {
    handler.allocate = agColorAllocate;
    handler.tag = agColorTag;
    return handler;
  }

  if (equals(class_name, "Surface")) {
    handler.allocate = agSurfaceAllocate;
    handler.tag = agSurfaceTag;
    handler.destroy = agSurfaceDestroy;
    return handler;
  }

  if (equals(class_name, "Context")) {
    handler.allocate = agContextAllocate;
    handler.tag = agContextTag;
    handler.destroy = agContextDestroy;
    return handler;
  }

  if (equals(class_name, "SolidPattern") || equals(class_name, "SurfacePattern") || equals(class_name, "LinearGradientPattern") || equals(class_name, "RadialGradientPattern")) {
    handler.allocate = agPatternAllocate;
    handler.tag = agPatternTag;
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

  if (equals(class_name, "Matrix")) {
    if (equals(signature, "init new()")) { return agMatrixNew; }
    if (equals(signature, "init new_translate(_,_)")) { return agMatrixNewTranslate; }
    if (equals(signature, "init new_scale(_,_)")) { return agMatrixNewScale; }
    if (equals(signature, "init new_rotate(_)")) { return agMatrixNewRotate; }
    if (equals(signature, "translate(_,_)")) { return agMatrixTranslate; }
    if (equals(signature, "scale(_,_)")) { return agMatrixScale; }
    if (equals(signature, "rotate(_)")) { return agMatrixRotate; }
    if (equals(signature, "invert()")) { return agMatrixInvert; }
    if (equals(signature, "*(_)")) { return agMatrixMultiply; }
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

  if (equals(class_name, "Pattern")) {
    if (equals(signature, "set_matrix(_)")) { return agPatternSetMatrix; }
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
    if (equals(signature, "push_group()")) { return agContextPushGroup; }
    if (equals(signature, "pop_group_to_source()")) { return agContextPopGroupToSource; }
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
    if (equals(signature, "paint_with_alpha(_)")) { return agContextPaintWithAlpha; }
    if (equals(signature, "move_to(_,_)")) { return agContextMoveTo; }
    if (equals(signature, "line_to(_,_)")) { return agContextLineTo; }
    if (equals(signature, "curve_to(_,_,_,_,_,_)")) { return agContextCurveTo; }
    if (equals(signature, "close_path()")) { return agContextClosePath; }
    if (equals(signature, "rectangle(_,_,_,_)")) { return agContextRectangle; }
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
