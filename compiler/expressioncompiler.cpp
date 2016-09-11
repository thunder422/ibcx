/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "compileerror.h"
#include "compiler.h"
#include "constnumcompiler.h"
#include "expressioncompiler.h"
#include "functions.h"
#include "operators.h"
#include "programcode.h"
#include "programunit.h"
#include "table.h"


void NoConvert(Compiler &compiler, DataType data_type);

class ExpressionCompilerImpl : public ExpressionCompiler {
public:
    ExpressionCompilerImpl(Compiler &compiler);

    DataType compileExpression(DataType expected_data_type) override;
    DataType compileExpression() override;

private:
    using CompileSubExprFunction = DataType (ExpressionCompilerImpl::*)();
    using GetCodesFunction = OperatorCodes *(*)(Compiler &, Precedence);
    using ConvertFunction = void (*)(Compiler &compiler, DataType data_type);

    struct SubExpression {
        unsigned column;
        unsigned length;
        DataType data_type;
    };

    DataType compileImplication();
    DataType compileEquivalence();
    DataType compileOr();
    DataType compileExclusiveOr();
    DataType compileAnd();
    DataType compileNot();
    OperatorCodes *getNotOperatorCodes();
    DataType compileNotOperand(OperatorCodes *codes);
    DataType compileEquality();
    DataType compileRelation();
    DataType compileSummation();
    DataType compileModulo();
    DataType compileIntegerDivision();
    DataType compileProduct();
    DataType compileExponential();
    DataType compileNegation();
    DataType compileOperand();
    DataType compileFunction();
    std::vector<DataType> compileFunctionArguments(FunctionCodes *codes);
    DataType addFunctionCode(FunctionCodes *codes, std::vector<DataType> argument_data_types) const;
    DataType compileParentheses(DataType expected_data_type = {});
    DataType compileNumConstant();
    DataType compileNumOperator(Precedence precedence,
        DataType (ExpressionCompilerImpl::*compile_operand)(),
        OperatorCodes *(*get_codes)(Compiler &, Precedence),
        void (*convert)(Compiler &compiler, DataType data_type) = NoConvert);
    DataType compileNumStrOperator(Precedence precedence,
        CompileSubExprFunction compile_sub_expression, GetCodesFunction get_codes);
    ExpressionCompilerImpl::SubExpression
        compileSubExpression(CompileSubExprFunction compile_sub_expression);
    DataType compileNumExpression(CompileSubExprFunction compile_sub_expression);
    DataType addOperatorCode(OperatorCodes *codes, DataType lhs_data_type, DataType rhs_data_type)
        const;

    Compiler &compiler;
};

// ----------------------------------------

std::unique_ptr<ExpressionCompiler> ExpressionCompiler::create(Compiler &compiler)
{
    return std::unique_ptr<ExpressionCompiler> {new ExpressionCompilerImpl {compiler}};
}

// ----------------------------------------

OperatorCodes *SymbolGetCodes(Compiler &compiler, Precedence precedence)
{
    return compiler.getSymbolOperatorCodes(precedence);
}

OperatorCodes *ComparisonGetCodes(Compiler &compiler, Precedence precedence)
{
    return compiler.getComparisonOperatorCodes(precedence);
}

OperatorCodes *WordGetCodes(Compiler &compiler, Precedence precedence)
{
    return compiler.getWordOperatorCodes(precedence);
}

void NoConvert(Compiler &compiler, DataType data_type)
{
    (void)compiler;
    (void)data_type;
}

void ConvertToDouble(Compiler &compiler, DataType data_type)
{
    compiler.convertToDouble(data_type);
}

void ConvertToInteger(Compiler &compiler, DataType data_type)
{
    compiler.convertToInteger(data_type);
}

// ----------------------------------------

ExpressionCompilerImpl::ExpressionCompilerImpl(Compiler &compiler) :
    compiler {compiler}
{
}

DataType ExpressionCompilerImpl::compileExpression(DataType expected_data_type)
{
    auto sub_expression = compileSubExpression(&ExpressionCompilerImpl::compileImplication);
    if (sub_expression.data_type) {
        if (sub_expression.data_type.isNotNumeric()) {
            throw ExpNumExprError {sub_expression.column, sub_expression.length};
        } else if (expected_data_type.isDouble()) {
            ConvertToDouble(compiler, sub_expression.data_type);
        } else if (expected_data_type.isInteger()) {
            ConvertToInteger(compiler, sub_expression.data_type);
        }
    } else {
        throw ExpNumExprError {compiler.getColumn()};
    }
    return sub_expression.data_type;
}

DataType ExpressionCompilerImpl::compileExpression()
{
    return compileImplication();
}

DataType ExpressionCompilerImpl::compileImplication()
{
    return compileNumOperator(Precedence::Imp, &ExpressionCompilerImpl::compileEquivalence,
        WordGetCodes, ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileEquivalence()
{
    return compileNumOperator(Precedence::Eqv, &ExpressionCompilerImpl::compileOr, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileOr()
{
    return compileNumOperator(Precedence::Or, &ExpressionCompilerImpl::compileExclusiveOr,
        WordGetCodes, ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileExclusiveOr()
{
    return compileNumOperator(Precedence::Xor, &ExpressionCompilerImpl::compileAnd, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileAnd()
{
    return compileNumOperator(Precedence::And, &ExpressionCompilerImpl::compileNot, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileNot()
{
    if (auto codes = getNotOperatorCodes()) {
        return compileNotOperand(codes);
    } else {
        return compileEquality();
    }
}

OperatorCodes *ExpressionCompilerImpl::getNotOperatorCodes()
{
    return compiler.getWordOperatorCodes(Precedence::Not);
}

DataType ExpressionCompilerImpl::compileNotOperand(OperatorCodes *codes)
{
    auto data_type = compileNumExpression(&ExpressionCompilerImpl::compileNot);
    compiler.convertToInteger(data_type);
    return addOperatorCode(codes, {}, {});
}

DataType ExpressionCompilerImpl::compileEquality()
{
    return compileNumStrOperator(Precedence::Equality, &ExpressionCompilerImpl::compileRelation,
        ComparisonGetCodes);
}

DataType ExpressionCompilerImpl::compileRelation()
{
    return compileNumStrOperator(Precedence::Relation, &ExpressionCompilerImpl::compileSummation,
        ComparisonGetCodes);
}

DataType ExpressionCompilerImpl::compileSummation()
{
    return compileNumStrOperator(Precedence::Summation, &ExpressionCompilerImpl::compileModulo,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileModulo()
{
    return compileNumOperator(Precedence::Modulo, &ExpressionCompilerImpl::compileIntegerDivision,
        WordGetCodes);
}

DataType ExpressionCompilerImpl::compileIntegerDivision()
{
    return compileNumOperator(Precedence::IntDivide, &ExpressionCompilerImpl::compileProduct,
        SymbolGetCodes, ConvertToDouble);
}

DataType ExpressionCompilerImpl::compileProduct()
{
    return compileNumOperator(Precedence::Product, &ExpressionCompilerImpl::compileExponential,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileExponential()
{
    return compileNumOperator(Precedence::Exponential, &ExpressionCompilerImpl::compileOperand,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileNegation()
{
    compiler.skipWhiteSpace();
    auto data_type = compileNumExpression(&ExpressionCompilerImpl::compileExponential);
    auto codes = Table::operatorCodes(Precedence::Negate);
    compiler.addInstruction(codes->select(data_type).code);
    return data_type;
}

DataType ExpressionCompilerImpl::compileOperand()
{
    if (compiler.peekNextChar() == '(') {
        return compileParentheses();
    }
    if (auto codes = getNotOperatorCodes()) {
        return compileNotOperand(codes);
    }
    auto data_type = compileFunction();
    if (data_type) {
        return data_type;
    }
    data_type = compileNumConstant();
    if (data_type) {
        return data_type;
    }
    return compiler.compileStringConstant();
}

DataType ExpressionCompilerImpl::compileFunction()
{
    if (auto codes = compiler.getNumFunctionCodes()) {
        auto argument_data_types = compileFunctionArguments(codes);
        return addFunctionCode(codes, argument_data_types);
    }
    return {};
}

std::vector<DataType> ExpressionCompilerImpl::compileFunctionArguments(FunctionCodes *codes)
{
    std::vector<DataType> argument_data_types;
    if (compiler.peekNextChar() == '(') {
        auto data_type = compileParentheses(codes->argumentDataType());
        argument_data_types.push_back(data_type);
    } else if (!codes->argumentOptional()) {
        throw CompileError {"expected opening parentheses", compiler.getColumn()};
    }
    return argument_data_types;
}

DataType ExpressionCompilerImpl::addFunctionCode(FunctionCodes *codes,
    std::vector<DataType> argument_data_types) const
{
    auto info = codes->select(argument_data_types);
    compiler.addInstruction(info.code);
    return info.result_data_type;
}

DataType ExpressionCompilerImpl::compileParentheses(DataType expected_data_type)
{
    compiler.getNextChar();
    compiler.skipWhiteSpace();
    auto data_type = compileExpression(expected_data_type);
    if (compiler.peekNextChar() != ')') {
        throw CompileError {"expected closing parentheses", compiler.getColumn()};
    }
    compiler.getNextChar();
    return data_type;
}

DataType ExpressionCompilerImpl::compileNumConstant()
{
    auto constant_number = ConstNumCompiler::create(compiler);
    auto data_type = constant_number->compile();
    if (!data_type && constant_number->negateOperator()) {
        return compileNegation();
    }
    if (auto unparsed_char = constant_number->unparsedChar()) {
        compiler.parseKeyword(unparsed_char);
    }
    return data_type;
}

DataType ExpressionCompilerImpl::compileNumOperator(Precedence precedence,
    CompileSubExprFunction compile_sub_expression, GetCodesFunction get_codes,
    ConvertFunction convert)
{
    auto lhs = compileSubExpression(compile_sub_expression);
    if (lhs.data_type) {
        while (auto codes = get_codes(compiler, precedence)) {
            if (lhs.data_type.isNotNumeric()) {
                throw ExpNumExprError {lhs.column, lhs.length};
            }
            convert(compiler, lhs.data_type);
            auto rhs_data_type = compileNumExpression(compile_sub_expression);
            convert(compiler, rhs_data_type);
            lhs.data_type = addOperatorCode(codes, lhs.data_type, rhs_data_type);
        }
    }
    return lhs.data_type;
}

DataType ExpressionCompilerImpl::compileNumStrOperator(Precedence precedence,
    CompileSubExprFunction compile_sub_expression, GetCodesFunction get_codes)
{
    auto lhs = compileSubExpression(compile_sub_expression);
    if (lhs.data_type) {
        while (auto codes = get_codes(compiler, precedence)) {
            auto rhs = compileSubExpression(compile_sub_expression);
            try {
                lhs.data_type = addOperatorCode(codes, lhs.data_type, rhs.data_type);
            }
            catch (const ExpNumLeftOperandError &) {
                throw ExpNumExprError {lhs.column, lhs.length};
            }
            catch (const ExpNumRightOperandError &) {
                throw ExpNumExprError {rhs.column, rhs.length};
            }
            catch (const ExpStrOperandError &) {
                throw ExpStrExprError {rhs.column, rhs.length};
            }
        }
    }
    return lhs.data_type;
}

ExpressionCompilerImpl::SubExpression
ExpressionCompilerImpl::compileSubExpression(CompileSubExprFunction compile_sub_expression)
{
    SubExpression sub_expression;
    sub_expression.column = compiler.getColumn();
    sub_expression.data_type = (this->*compile_sub_expression)();
    sub_expression.length = compiler.getColumn() - sub_expression.column;
    return sub_expression;
}

DataType ExpressionCompilerImpl::compileNumExpression(CompileSubExprFunction compile_sub_expression)
{
    auto operand_column = compiler.getColumn();
    auto data_type = (this->*compile_sub_expression)();
    if (data_type.isNotNumeric()) {
        auto operand_length = data_type ? compiler.getColumn() - operand_column : 1;
        throw ExpNumExprError {operand_column, operand_length};
    }
    return data_type;
}

DataType ExpressionCompilerImpl::addOperatorCode(OperatorCodes *codes, DataType lhs_data_type,
    DataType rhs_data_type) const
{
    auto info = codes->select(lhs_data_type, rhs_data_type);
    compiler.addInstruction(info.code);
    return info.result_data_type;
}
