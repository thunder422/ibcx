/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "operators.h"


OperatorCode<OpType::Int> not_code {nullptr, nullptr};

NotOperatorCodes not_codes {Precedence::Not, "NOT", not_code};
