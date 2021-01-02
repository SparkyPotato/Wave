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

#include "Parser/Parser.h"

namespace Wave {

Parser::Parser(const Lexer& lexer)
	: m_Tokens(lexer.GetTokens())
{}

void Parser::Parse()
{
	while (IsGood())
	{
		try
		{
			m_Expressions.emplace_back(Expr());
		}
		catch (...) { Synchronize(); }
	}
}

const std::vector<Wave::Diagnostic>& Parser::GetDiagnostics()
{
	return m_Diagnostics;
}

sp<Expression> Parser::Expr()
{
	return Equality();
}

sp<Expression> Parser::Equality()
{
	auto expr = Comparision();

	while (LookAhead(TokenType::EqualEqual) || LookAhead(TokenType::NotEqual))
	{
		Token op = Previous();
		auto right = Comparision();
		expr = std::make_shared<BinaryExpression>(expr, op, right);
	}

	return expr;
}

sp<Expression> Parser::Comparision()
{
	auto expr = Term();

	while (LookAhead(TokenType::Greater) || LookAhead(TokenType::GreaterEqual)
		|| LookAhead(TokenType::Lesser) || LookAhead(TokenType::LesserEqual))
	{
		Token op = Previous();
		auto right = Term();
		expr = std::make_shared<BinaryExpression>(expr, op, right);
	}

	return expr;
}

sp<Expression> Parser::Term()
{
	auto expr = Factor();

	while (LookAhead(TokenType::Plus) || LookAhead(TokenType::Minus))
	{
		Token op = Previous();
		auto right = Factor();
		expr = std::make_shared<BinaryExpression>(expr, op, right);
	}

	return expr;
}

sp<Expression> Parser::Factor()
{
	auto expr = Unary();

	while (LookAhead(TokenType::Star) || LookAhead(TokenType::Slash))
	{
		Token op = Previous();
		auto right = Unary();
		expr = std::make_shared<BinaryExpression>(expr, op, right);
	}

	return expr;
}

sp<Expression> Parser::Unary()
{
	if (LookAhead(TokenType::Not) || LookAhead(TokenType::Minus))
	{
		Token op = Previous();
		auto right = Unary();
		return std::make_shared<UnaryExpression>(op, right);
	}

	return Primary();
}

sp<Expression> Parser::Primary()
{
	if (LookAhead(TokenType::True) || LookAhead(TokenType::False)
		|| LookAhead(TokenType::Integer) || LookAhead(TokenType::Real) || LookAhead(TokenType::String))
	{ return std::make_shared<LiteralExpression>(Previous()); }
	else if (LookAhead(TokenType::LeftParenthesis))
	{
		auto expr = Expr();
		ErrorCheck(TokenType::RightParenthesis, "missing closing parenthesis ')'");
	}
	else if (!IsGood())
	{
		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			"expected expression after"
		);
		throw - 1;
	}

	m_Diagnostics.emplace_back(
		Peek().Marker,
		DiagnosticSeverity::Error,
		"expected expression before"
	);
	throw -1;
}

bool Parser::LookAhead(TokenType type)
{
	if (IsGood() && Peek().Type == type) 
	{ 
		Advance();
		return true;
	}

	return false;
}

const Token& Parser::Advance()
{
	if (IsGood()) { m_Tok++; }
	return Previous();
}

bool Parser::IsGood()
{
	return m_Tok < m_Tokens.size();
}

const Token& Parser::Peek()
{
	return m_Tokens[m_Tok];
}

const Token& Parser::Previous()
{
	return m_Tokens[m_Tok - 1];
}

const Token& Parser::ErrorCheck(TokenType type, const std::string& message)
{
	if (IsGood() && Peek().Type == type) { return Advance(); }

	m_Diagnostics.emplace_back(
		Peek().Marker,
		DiagnosticSeverity::Error,
		message
	);

	throw -1;
}

void Parser::Synchronize()
{
	Advance();

	while (IsGood())
	{
		if (Previous().Type == TokenType::Semicolon) { return; }

		switch (Peek().Type) 
		{
		case TokenType::Class:
		case TokenType::Function:
		case TokenType::For:
		case TokenType::If:
		case TokenType::While:
		case TokenType::Identifier:
		case TokenType::Return:
			return;
		default:
			break;
		}

		Advance();
	}
}

}
