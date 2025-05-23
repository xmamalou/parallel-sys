/*
    Parallel Systems Exercise Batch 1 -- Solutions to Batch 1 of Exercises for the Parallel
    Systems Course of the "Computer Engineering" Masters Programme of NKUA
    Copyright (C) 2025 Christoforos-Marios Mamaloukas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __EXERCISES_H__
#define __EXERCISES_H__

#include <stdint.h>
#include <stdbool.h>

#include "macros.h"

EXERCISE_DECL(pi_calc);
EXERCISE_DECL(shared_var);
EXERCISE_DECL(false_sharing);
EXERCISE_DECL(barriers);
EXERCISE_DECL(better_mul);
EXERCISE_DECL(game_of_life);
EXERCISE_DECL(gaussian_elimination);
EXERCISE_DECL(merge_sort);

#endif /* __EXERCISES_H__ */ 