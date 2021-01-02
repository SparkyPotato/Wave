// Copyright 2021 SparkyPotato
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <any>
#include <memory>

#include "WaveCompiler/Global.h"
#include "WaveCompiler/Lexer.h"

namespace Wave {

template<typename T>
using sp = std::shared_ptr<T>;

struct BinaryExpression;
struct GroupExpression;
struct LiteralExpression;
struct UnaryExpression;

/// Expression visitor.
class WAVEC_API ExpressionVisitor
{
public:
	/// Visit a binary expression.
	///
	/// \param expression Expression to visit.
	/// \param context Context argument passed.
	virtual void Visit(BinaryExpression& expression, std::any& context) = 0;

	/// Visit a group expression.
	///
	/// \param expression Expression to visit.
	/// \param context Context argument passed.
	virtual void Visit(GroupExpression& expression, std::any& context) = 0;

	/// Visit a literal expression.
	///
	/// \param expression Expression to visit.
	/// \param context Context argument passed.
	virtual void Visit(LiteralExpression& expression, std::any& context) = 0;

	/// Visit a unary expression.
	///
	/// \param expression Expression to visit.
	/// \param context Context argument passed.
	virtual void Visit(UnaryExpression& expression, std::any& context) = 0;
};

/// Expression.
/// Contains an expression in the AST.
struct WAVEC_API Expression
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass.
	virtual void Accept(ExpressionVisitor& visitor, std::any& context) = 0;
};

/// Binary expression which contains two expressions
/// as operands, and one operator.
struct WAVEC_API BinaryExpression : Expression
{
	/// Constructor.
	///
	/// \param left Left expression.
	/// \param op Binary operator.
	/// \param right Right expression.
	BinaryExpression(const sp<Expression>& left, const Token& op, const sp<Expression>& right)
		: Left(left), Right(right), Operator(op)
	{}

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass.
	virtual void Accept(ExpressionVisitor& visitor, std::any& context) override
	{
		return visitor.Visit(*this, context);
	}

	/// Left side of the expression.
	sp<Expression> Left;

	/// Binary operator of the expression.
	Token Operator;

	/// Right side of the expression.
	sp<Expression> Right;
};

/// Group expression which contains a single expression,
/// as if it were surrounded by parentheses.
struct WAVEC_API GroupExpression : Expression
{
	/// Constructor.
	///
	/// \param expr Grouped expression.
	GroupExpression(const sp<Expression> expr)
		: Expr(expr)
	{}

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass.
	virtual void Accept(ExpressionVisitor& visitor, std::any& context) override
	{
		return visitor.Visit(*this, context);
	}

	/// Expression that is grouped.
	sp<Expression> Expr;
};

/// Expression evaluating into a literal.
struct WAVEC_API LiteralExpression : Expression
{
	/// Constructor.
	///
	/// \param literal Literal.
	LiteralExpression(const Token& literal)
		: Literal(literal)
	{}

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass.
	virtual void Accept(ExpressionVisitor& visitor, std::any& context) override
	{
		return visitor.Visit(*this, context);
	}

	/// Literal token.
	Token Literal;
};

/// Unary expression, with one expression operand and one operator.
struct WAVEC_API UnaryExpression : Expression
{
	/// Constructor.
	///
	/// \param op Binary operator.
	/// \param right Right expression.
	UnaryExpression(const Token& op, const sp<Expression>& right)
		: Operator(op), Right(right)
	{}

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass.
	virtual void Accept(ExpressionVisitor& visitor, std::any& context) override
	{
		return visitor.Visit(*this, context);
	}

	/// Unary operator.
	Token Operator;

	/// Right side of the expression.
	sp<Expression> Right;
};

}
