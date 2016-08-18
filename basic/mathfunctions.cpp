/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "functions.h"
#include "recreator.h"


FunctionCode<ArgType::Dbl> abs_dbl_code {recreateFunction, nullptr};
FunctionCode<ArgType::Int> abs_int_code {recreateFunction, nullptr};

MultiTypeFunctionCodes abs_codes {"ABS", abs_dbl_code, abs_int_code};
