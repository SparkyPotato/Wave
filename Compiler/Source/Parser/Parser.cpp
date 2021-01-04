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
	if (m_Tokens.size() == 1)
	{
		m_Diagnostics.emplace_back(
			m_Tokens[0].Marker,
			DiagnosticSeverity::Error,
			"file is empty"
		);
		return;
	}

	m_Module = std::make_unique<Module>();

	try
	{
		// Module definitions.
		Ensure(TokenType::Module, "expected module definition");
		m_Module->Def = ParseIdentifier();
		Ensure(TokenType::Semicolon, "expected semicolon ';'");

		while (Check(TokenType::Import))
		{
			m_Module->Imports.emplace_back(ParseImport());
		}

		while (IsGood())
		{
			m_Module->Definitions.emplace_back(ParseGlobalDefinition());
		}
	}
	catch (...) {}
}

const std::vector<Wave::Diagnostic>& Parser::GetDiagnostics()
{
	return m_Diagnostics;
}

Identifier Parser::ParseIdentifier()
{
	Identifier ident;
	do
	{
		 ident.Path.emplace_back(Ensure(TokenType::Identifier, "expected identifier"));
	} while (Check(TokenType::Period));

	return ident;
}

ModuleImport Parser::ParseImport()
{
	ModuleImport import;
	import.Imported = ParseIdentifier();

	if (Check(TokenType::As))
	{
		import.As = ParseIdentifier();
	}
	else
	{
		import.As = import.Imported;
	}

	Ensure(TokenType::Semicolon, "expected semicolon ';'");
	return import;
}

GlobalDefinition Parser::ParseGlobalDefinition()
{
	GlobalDefinition def;
	if (Check(TokenType::Export)) { def.Exported = true; }
	def.Def = ParseDefinition();

	return def;
}

up<Definition> Parser::ParseDefinition()
{
	auto& tok = Advance();

	switch (tok.Type)
	{
	case TokenType::Function: return ParseFunctionDefinition();
	case TokenType::Class: return ParseClassDefinition();
	case TokenType::Variable:
	case TokenType::Const:
	case TokenType::Static: return ParseVarDefinition();
	default:
		m_Diagnostics.emplace_back(
			tok.Marker,
			DiagnosticSeverity::Error,
			"expected definition (var, func, or class)"
		);
		throw -1;
	}
}

up<FunctionDefinition> Parser::ParseFunctionDefinition()
{
	auto def = std::make_unique<FunctionDefinition>();
	def->Ident = Ensure(TokenType::Identifier, "expected identifier");
	def->Func = ParseFunction();

	return def;
}

up<VarDefinition> Parser::ParseVarDefinition()
{
	auto def = std::make_unique<VarDefinition>();
	def->VarType = Previous();
	def->Ident = Ensure(TokenType::Identifier, "expected identifier");

	bool hasType = false;
	if (Check(TokenType::Colon))
	{
		hasType = true;
		def->DataType = ParseType();
	}

	if (Check(TokenType::Equal))
	{
		def->Value = ParseExpression();
	}
	else if (!hasType)
	{
		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			"type can only be omitted if variable is initialized"
		);
	}

	Ensure(TokenType::Semicolon, "expected semicolon ';'");
	return def;
}

up<ClassDefinition> Parser::ParseClassDefinition()
{
	auto def = std::make_unique<ClassDefinition>();
	def->Ident = Ensure(TokenType::Identifier, "expected identifier");

	if (Check(TokenType::Colon))
	{
		do
		{
			def->Bases.emplace_back(ParseIdentifier());
		} while (Check(TokenType::Comma));
	}

	Ensure(TokenType::LeftBrace, "expected block");

	enum class Level { Public, Protected, Private };
	auto level = Level::Public;
	while (!Check(TokenType::RightBrace))
	{
		if (Check(TokenType::Public)) { level = Level::Public; }
		else if (Check(TokenType::Protected)) { level = Level::Protected; }
		else if (Check(TokenType::Private)) { level = Level::Private; }

		if (Check(TokenType::Variable) || Check(TokenType::Const) || Check(TokenType::Static))
		{
			switch (level)
			{
			case Level::Public: def->Public.emplace_back(ParseClassVar()); break;
			case Level::Protected: def->Protected.emplace_back(ParseClassVar()); break;
			case Level::Private: def->Private.emplace_back(ParseClassVar()); break;
			}
		}
		else
		{
			switch (level)
			{
			case Level::Public: def->Public.emplace_back(ParseClassFunc()); break;
			case Level::Protected: def->Protected.emplace_back(ParseClassFunc()); break;
			case Level::Private: def->Private.emplace_back(ParseClassFunc()); break;
			}
		}
	}

	Ensure(TokenType::Semicolon, "expected semicolon ';'");
}

up<ClassVar> Parser::ParseClassVar()
{
	auto var = std::make_unique<ClassVar>();
	var->Def = ParseVarDefinition();
	return var;
}

up<ClassFunc> Parser::ParseClassFunc()
{
	auto& tok = Advance();
	switch (tok.Type)
	{
	case TokenType::Construct: return ParseConstructor();
	case TokenType::Identifier: return ParseGetterOrSetter();
	default:
		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			"expected class function"
		);
		throw -1;
	}
}

up<ClassFunc> Parser::ParseGetterOrSetter()
{

}

up<Constructor> Parser::ParseConstructor()
{
	auto construct = std::make_unique<Constructor>();
	Ensure(TokenType::LeftParenthesis, "expected opening parenthesis '('");
	if (!Check(TokenType::RightParenthesis))
	{
		do
		{
			construct->Params.emplace_back(ParseParam());
		} while (Check(TokenType::Comma));
		Ensure(TokenType::RightParenthesis, "expected closing parenthesis ')'");
	}
	construct->ExecBlock = ParseBlock();

	return construct;
}

up<Type> Parser::ParseType()
{
	auto& tok = Advance();
	switch (tok.Type)
	{
	case TokenType::IntegerType: return std::make_unique<IntType>();
	case TokenType::RealType: return std::make_unique<RealType>();
	case TokenType::StringType: return std::make_unique<StringType>();
	case TokenType::BoolType: return std::make_unique<BoolType>();
	case TokenType::Function: return ParseFuncType();
	case TokenType::TypeOf: return ParseTypeOf();
	case TokenType::Identifier:
	{
		auto c = std::make_unique<ClassType>();
		m_Tok--;
		c->Ident = ParseIdentifier();
		return c;
	}
	default:
		m_Diagnostics.emplace_back(
			tok.Marker,
			DiagnosticSeverity::Error,
			"expected type"
		);
		throw -1;
	}
}

up<TypeOf> Parser::ParseTypeOf()
{
	auto type = std::make_unique<TypeOf>();
	type->Expr = ParseExpression();
	return type;
}

up<FuncType> Parser::ParseFuncType()
{
	auto type = std::make_unique<FuncType>();

	Ensure(TokenType::LeftParenthesis, "expected opening parenthesis '('");
	if (!Check(TokenType::RightParenthesis))
	{
		do
		{
			type->ParamTypes.emplace_back(ParseType());
		} while (Check(TokenType::Comma));
		Ensure(TokenType::RightParenthesis, "expected closing parenthesis ')'");
	}

	if (Check(TokenType::Colon))
	{
		type->ReturnType = ParseType();
	}

	return type;
}

up<Expression> Parser::ParseExpression()
{

}

up<Function> Parser::ParseFunction()
{
	auto func = std::make_unique<Function>();

	Ensure(TokenType::LeftParenthesis, "expected opening parenthesis '('");
	if (!Check(TokenType::RightParenthesis))
	{
		do
		{
			func->Params.emplace_back(ParseParam());
		} while (Check(TokenType::Comma));
		Ensure(TokenType::RightParenthesis, "expected closing parenthesis ')'");
	}

	if (Check(TokenType::Colon))
	{
		func->ReturnType = ParseType();
	}

	func->ExecBlock = ParseBlock();

	return func;
}

Parameter Parser::ParseParam()
{
	Parameter param;
	param.Ident = Ensure(TokenType::Identifier, "expected identifier");
	param.DataType = ParseType();
	return param;
}


up<Block> Parser::ParseBlock()
{
	auto block = std::make_unique<Block>();
	Ensure(TokenType::LeftBrace, "expected block");
	do
	{
		block->Statements.emplace_back(ParseStatement());
	} while (!Check(TokenType::RightBrace));
	return block;
}

up<Statement> Parser::ParseStatement()
{

}


const Token& Parser::Advance()
{
	m_Tok++;
	return Previous();
}

bool Parser::Check(TokenType type)
{
	if (IsGood() && Peek().Type == type) 
	{
		Advance();
		return true; 
	}

	return false;
}

const Token& Parser::Peek()
{
	return m_Tokens[m_Tok];
}

const Token& Parser::Previous()
{
	return m_Tokens[m_Tok - 1];
}

bool Parser::IsGood()
{
	return m_Tok < m_Tokens.size() - 1;
}

const Token& Parser::Ensure(TokenType type, const std::string& message)
{
	if (!IsGood())
	{
		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			message
		);
		throw -1;
	}

	auto& tok = Advance();
	if (tok.Type != type)
	{
		m_Diagnostics.emplace_back(
			tok.Marker,
			DiagnosticSeverity::Error,
			message
		);
		throw -1;
	}

	return tok;
}

}
