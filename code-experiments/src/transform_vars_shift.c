/**
 * @file transform_vars_shift.c
 * @brief Implementation of shifting all decision values by an offset.
 */

#include <assert.h>

#include "coco.h"
#include "coco_problem.c"

/**
 * @brief Data type for transform_vars_shift.
 */
typedef struct {
  double *offset;
  double *shifted_x;
  coco_problem_free_function_t old_free_problem;
} transform_vars_shift_data_t;

/**
 * @brief Evaluates the transformation.
 */
static void transform_vars_shift_evaluate(coco_problem_t *problem, const double *x, double *y) {
  size_t i;
  transform_vars_shift_data_t *data;
  coco_problem_t *inner_problem;

  data = coco_problem_transformed_get_data(problem);
  inner_problem = coco_problem_transformed_get_inner_problem(problem);

  for (i = 0; i < problem->number_of_variables; ++i) {
    data->shifted_x[i] = x[i] - data->offset[i];
  }
  coco_evaluate_function(inner_problem, data->shifted_x, y);
  assert(y[0] + 1e-13 >= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_shift_free(void *thing) {
  transform_vars_shift_data_t *data = thing;
  coco_free_memory(data->shifted_x);
  coco_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static coco_problem_t *transform_vars_shift(coco_problem_t *inner_problem,
                                              const double *offset,
                                              const int shift_bounds) {
  transform_vars_shift_data_t *data;
  coco_problem_t *problem;
  size_t i;
  if (shift_bounds)
    coco_error("shift_bounds not implemented.");

  data = coco_allocate_memory(sizeof(*data));
  data->offset = coco_duplicate_vector(offset, inner_problem->number_of_variables);
  data->shifted_x = coco_allocate_vector(inner_problem->number_of_variables);

  problem = coco_problem_transformed_allocate(inner_problem, data, transform_vars_shift_free);
  problem->evaluate_function = transform_vars_shift_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] += data->offset[i];
  }
  return problem;
}
