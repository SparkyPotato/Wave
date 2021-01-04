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
	def.Exported = Check(TokenType::Export);
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
	auto& tok = Previous();
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
		if (Check(TokenType::Public)) { Ensure(TokenType::Colon, "expected colon ':'");  level = Level::Public; }
		else if (Check(TokenType::Protected)) { Ensure(TokenType::Colon, "expected colon ':'"); level = Level::Protected; }
		else if (Check(TokenType::Private)) { Ensure(TokenType::Colon, "expected colon ':'") ;level = Level::Private; }

		if (Check(TokenType::Variable))
		{
			switch (level)
			{
			case Level::Public: def->Public.emplace_back(ParseClassVar()); break;
			case Level::Protected: def->Protected.emplace_back(ParseClassVar()); break;
			case Level::Private: def->Private.emplace_back(ParseClassVar()); break;
			}
		}
		else if (Check(TokenType::Static) || Check(TokenType::Const))
		{
			if (Check(TokenType::Function) || Check(TokenType::Abstract))
			{
				m_Tok--;
				switch (level)
				{
				case Level::Public: def->Public.emplace_back(ParseClassFunc()); break;
				case Level::Protected: def->Protected.emplace_back(ParseClassFunc()); break;
				case Level::Private: def->Private.emplace_back(ParseClassFunc()); break;
				}
			}
			else
			{
				switch (level)
				{
				case Level::Public: def->Public.emplace_back(ParseClassVar()); break;
				case Level::Protected: def->Protected.emplace_back(ParseClassVar()); break;
				case Level::Private: def->Private.emplace_back(ParseClassVar()); break;
				}
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
	return def;
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
	case TokenType::Identifier: m_Tok--; return ParseGetterOrSetter();
	case TokenType::Static:
	case TokenType::Const: 
		if (Check(TokenType::Function)) { return ParseMethod(); }
		else if (Check(TokenType::Abstract)) { return ParseAbstract(); }
		else
		{
			m_Diagnostics.emplace_back(
				Previous().Marker,
				DiagnosticSeverity::Error,
				"expected class function"
			);
			throw -1;
		}
	case TokenType::Function: return ParseMethod();
	case TokenType::Abstract: return ParseAbstract();
	default:
		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			"expected class function"
		);
		throw -1;
	}
}

up<Method> Parser::ParseMethod()
{
	auto method = std::make_unique<Method>();
	m_Tok--;
	auto& f = Peek();
	
	if (f.Type == TokenType::Const) { method->IsConst = true; }
	else if (f.Type == TokenType::Static) { method->IsStatic = true; }

	Ensure(TokenType::Function, "expected function definition");
	method->Def = ParseFunctionDefinition();

	return method;
}

up<Abstract> Parser::ParseAbstract()
{
	auto abstract = std::make_unique<Abstract>();
	abstract->IsConst = Previous().Type == TokenType::Const;
	abstract->Ident = Ensure(TokenType::Identifier, "expected identifier");
	Ensure(TokenType::LeftParenthesis, "expected opening parenthesis, '('");
	if (!Check(TokenType::RightParenthesis))
	{
		do
		{
			abstract->Params.emplace_back(ParseParam());
		} while (Check(TokenType::Comma));
		Ensure(TokenType::RightParenthesis, "expected closing parenthesis ')'");
	}

	if (Check(TokenType::Colon))
	{
		abstract->ReturnType = ParseType();
	}
	
	Ensure(TokenType::Semicolon, "expected semicolon ';'");
	return abstract;
}

up<ClassFunc> Parser::ParseGetterOrSetter()
{
	auto& ident = Advance();
	if (Check(TokenType::Colon))
	{
		auto getter = std::make_unique<Getter>();
		getter->Ident = ident;
		getter->GetType = ParseType();
		getter->ExecBlock = ParseBlock();
		
		return getter;
	}
	else if (Check(TokenType::LeftParenthesis))
	{
		auto setter = std::make_unique<Setter>();
		setter->Ident = ident;
		setter->SetParam = ParseParam();
		Ensure(TokenType::RightParenthesis, "expected closing parenthesis ')'");

		return setter;
	}
	else
	{
		m_Diagnostics.emplace_back(
			ident.Marker,
			DiagnosticSeverity::Error,
			"expected getter or setter"
		);
		throw -1;
	}
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

	up<Type> type;

	switch (tok.Type)
	{
	case TokenType::IntegerType: type = std::make_unique<IntType>(); break;
	case TokenType::RealType: type = std::make_unique<RealType>(); break;
	case TokenType::StringType: type = std::make_unique<StringType>(); break;
	case TokenType::BoolType: type = std::make_unique<BoolType>(); break;
	case TokenType::Function: type = ParseFuncType(); break;
	case TokenType::TypeOf: type = ParseTypeOf(); break;
	case TokenType::Identifier:
	{
		auto c = std::make_unique<ClassType>();
		m_Tok--;
		c->Ident = ParseIdentifier();
		type = std::move(c);
		break;
	}
	default:
		m_Diagnostics.emplace_back(
			tok.Marker,
			DiagnosticSeverity::Error,
			"expected type"
		);
		throw -1;
	}

	if (Check(TokenType::LeftIndex))
	{
		auto arr = std::make_unique<ArrayType>();
		
		if (!Check(TokenType::RightIndex)) 
		{ 
			arr->Size = ParseExpression(); 
			Ensure(TokenType::RightIndex, "expected closing bracket ']'");
		}

		arr->HoldType = std::move(type);
		return arr;
	}

	return type;
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
	return ParseAssignment();
}

up<Expression> Parser::ParseAssignment()
{
	auto expr = ParseEquality();

	if (Check(TokenType::Equal))
	{
		auto value = ParseAssignment();
		auto assign = std::make_unique<Assignment>();
		assign->Value = std::move(value);
		auto var = dynamic_cast<VarAccess*>(expr.get());
		if (var)
		{
			assign->Var = var->Var;
			return assign;
		}

		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			"invalid assignment, can only assign to variables."
		);
	}

	return expr;
}

up<Expression> Parser::ParseOr()
{
	auto expr = ParseAnd();

	while (Check(TokenType::Or))
	{
		auto& op = Previous();
		auto right = ParseAnd();
		auto logic = std::make_unique<Logical>();

		logic->Left = std::move(expr);
		logic->Operator = op;
		logic->Right = std::move(right);
		expr = std::move(logic);
	}

	return expr;
}

up<Expression> Parser::ParseAnd()
{
	auto expr = ParseEquality();

	while (Check(TokenType::And))
	{
		auto& op = Previous();
		auto right = ParseEquality();
		auto logic = std::make_unique<Logical>();

		logic->Left = std::move(expr);
		logic->Operator = op;
		logic->Right = std::move(right);
		expr = std::move(logic);
	}

	return expr;
}

up<Expression> Parser::ParseEquality()
{
	auto expr = ParseComparision();

	while (Check(TokenType::NotEqual) || Check(TokenType::EqualEqual))
	{
		auto& op = Previous();
		auto right = ParseComparision();
		auto temp = std::make_unique<Binary>();

		temp->Left = std::move(expr);
		temp->Operator = op;
		temp->Right = std::move(right);
		expr = std::move(temp);
	}

	return expr;
}

up<Expression> Parser::ParseComparision()
{
	auto expr = ParseTerm();

	while (Check(TokenType::Greater) || Check(TokenType::GreaterEqual)
		|| Check(TokenType::Lesser) || Check(TokenType::LesserEqual))
	{
		auto& op = Previous();
		auto right = ParseTerm();
		auto temp = std::make_unique<Binary>();

		temp->Left = std::move(expr);
		temp->Operator = op;
		temp->Right = std::move(right);
		expr = std::move(temp);
	}

	return expr;
}

up<Expression> Parser::ParseTerm()
{
	auto expr = ParseFactor();

	while (Check(TokenType::Minus) || Check(TokenType::Plus))
	{
		auto& op = Previous();
		auto right = ParseFactor();
		auto temp = std::make_unique<Binary>();

		temp->Left = std::move(expr);
		temp->Operator = op;
		temp->Right = std::move(right);
		expr = std::move(temp);
	}

	return expr;
}

up<Expression> Parser::ParseFactor()
{
	auto expr = ParseUnary();

	while (Check(TokenType::Slash) || Check(TokenType::Star))
	{
		auto& op = Previous();
		auto right = ParseUnary();
		auto temp = std::make_unique<Binary>();

		temp->Left = std::move(expr);
		temp->Operator = op;
		temp->Right = std::move(right);
		expr = std::move(temp);
	}

	return expr;
}

up<Expression> Parser::ParseUnary()
{
	if (Check(TokenType::Not) || Check(TokenType::Minus))
	{
		auto& op = Previous();
		auto right = ParseUnary();
		auto temp = std::make_unique<Binary>();

		temp->Operator = op;
		temp->Right = std::move(right);
		return temp;
	}

	return ParseCall();
}

up<Expression> Parser::ParseCall()
{
	auto callee = ParsePrimary();

	if (Check(TokenType::LeftParenthesis))
	{
		auto call = std::make_unique<Call>();
		call->Callee = std::move(callee);

		if (!Check(TokenType::RightParenthesis))
		{
			do
			{
				call->Args.emplace_back(ParseExpression());
			} while (Check(TokenType::Comma));

			Ensure(TokenType::RightParenthesis, "expected closing parenthesis ')'");
		}

		return call;
	}

	return callee;
}

up<Expression> Parser::ParsePrimary()
{
	if (Check(TokenType::True) || Check(TokenType::False)
		|| Check(TokenType::Integer) || Check(TokenType::Real)
		|| Check(TokenType::String))
	{
		auto literal = std::make_unique<Literal>();
		literal->Value = Previous();
		return literal;
	}
	else if (Check(TokenType::Identifier))
	{
		m_Tok--;

		auto var = ParseIdentifier();
		if (Check(TokenType::LeftIndex))
		{
			auto acc = std::make_unique<ArrayIndex>();
			acc->Var = var;
			acc->Index = ParseExpression();
			Ensure(TokenType::RightIndex, "expected closing bracket ']'");

			return acc;
		}

		auto acc = std::make_unique<VarAccess>();
		acc->Var = var;
		return acc;
	}
	else if (Check(TokenType::Copy))
	{
		auto& copy = Previous();

		Identifier var;
		try { var = ParseIdentifier(); }
		catch (...)
		{
			m_Diagnostics.emplace_back(
				copy.Marker,
				DiagnosticSeverity::Note,
				"can only only copy variables"
			);
			m_Diagnostics.emplace_back(
				copy.Marker,
				DiagnosticSeverity::Note,
				"consider removing 'copy'"
			);
			throw;
		}

		if (Check(TokenType::LeftIndex))
		{
			auto acc = std::make_unique<ArrayIndex>();
			acc->Var = var;
			acc->IsCopy = true;
			acc->Index = ParseExpression();
			Ensure(TokenType::RightIndex, "expected closing bracket ']'");

			return acc;
		}

		auto acc = std::make_unique<VarAccess>();
		acc->Var = var;
		acc->IsCopy = true;
		return acc;
	}
	else if (Check(TokenType::LeftParenthesis))
	{
		if (IsFunction())
		{
			return ParseFunction();
		}
		else
		{
			auto expr = ParseExpression();
			Ensure(TokenType::RightParenthesis, "expected closing parenthesis ')'");
			return expr;
		}
	}

	m_Diagnostics.emplace_back(
		Previous().Marker,
		DiagnosticSeverity::Error,
		"expected expression"
	);
	throw -1;
}

bool Parser::IsFunction()
{
	auto tok = m_Tok;
	if (!Check(TokenType::Identifier) && !Check(TokenType::RightParenthesis)) { return false; }
	while (!Check(TokenType::RightParenthesis)) { Advance(); }
	if (Check(TokenType::Colon)) { m_Tok = tok; return true; }
	else if (Check(TokenType::LeftBrace)) { m_Tok = tok - 1; return true; }
	else { m_Tok = tok; return false; }
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
		func->IsReturnConst = Check(TokenType::Const);
		func->ReturnType = ParseType();
	}

	func->ExecBlock = ParseBlock();

	return func;
}

Parameter Parser::ParseParam()
{
	Parameter param;
	param.Ident = Ensure(TokenType::Identifier, "expected identifier");
	if (Check(TokenType::Colon))
	{
		param.IsConst = Check(TokenType::Const);
		if (!Check(TokenType::Comma) && !Check(TokenType::RightParenthesis)) { param.DataType = ParseType(); }
		else { m_Tok--; param.DataType = std::make_unique<GenericType>(); }
	}
	else
	{
		param.DataType = std::make_unique<GenericType>();
	}
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
	if (IsDefinition()) { return ParseDefinition(); }
	else if (Check(TokenType::While)) { return ParseWhile(); }
	else if (Check(TokenType::For)) { return ParseFor(); }
	else if (Check(TokenType::Return)) 
	{
		auto ret = std::make_unique<Return>();
		if (!Check(TokenType::Semicolon)) 
		{ 
			ret->Value = ParseExpression(); 
			Ensure(TokenType::Semicolon, "expected semicolon ';'");
		}
		return ret;
	}
	else if (Check(TokenType::Break)) 
	{ 
		return std::make_unique<Break>();
		Ensure(TokenType::Semicolon, "expected semicolon ';'");
	}
	else if (Check(TokenType::Continue)) 
	{ 
		return std::make_unique<Continue>();
		Ensure(TokenType::Semicolon, "expected semicolon ';'");
	}
	else if (Check(TokenType::LeftBrace)) { m_Tok--; return ParseBlock(); }
	else if (Check(TokenType::If)) { return ParseIf(); }
	else if (Check(TokenType::Try)) { return ParseTry(); }
	else if (Check(TokenType::Throw)) 
	{
		auto thr = std::make_unique<Throw>();
		if (!Check(TokenType::Semicolon)) 
		{ 
			thr->Value = ParseExpression(); 
			Ensure(TokenType::Semicolon, "expected semicolon ';'");
		}
		return thr;
	}
	else
	{
		auto expr = std::make_unique<ExpressionStatement>();
		expr->Expr = ParseExpression();
		Ensure(TokenType::Semicolon, "expected semicolon ';'");
		return expr;
	}
}


up<While> Parser::ParseWhile()
{
	auto loop = std::make_unique<While>();
	loop->Condition = ParseExpression();
	loop->ExecBlock = ParseBlock();
	return loop;
}

up<For> Parser::ParseFor()
{
	auto loop = std::make_unique<For>();
	if (IsDefinition()) { loop->Initializer = ParseDefinition(); }
	else { loop->Initializer = ParseExpression(); }
	Ensure(TokenType::Semicolon, "expected semicolon ';'");
	loop->Condition = ParseExpression();
	Ensure(TokenType::Semicolon, "expected semicolon ';'");
	loop->Increment = ParseExpression();
	loop->ExecBlock = ParseBlock();
	return loop;
}

up<If> Parser::ParseIf()
{
	auto ifs = std::make_unique<If>();
	ifs->Condition = ParseExpression();
	ifs->True = ParseBlock();

	while (Check(TokenType::Else) && Check(TokenType::If))
	{
		auto& elseif = ifs->ElseIfs.emplace_back();
		elseif.Condition = ParseExpression();
		elseif.True = ParseBlock();
	}

	if (Check(TokenType::Else))
	{
		ifs->Else = ParseBlock();
	}

	return ifs;
}

up<Try> Parser::ParseTry()
{
	auto tryy = std::make_unique<Try>();
	tryy->ExecBlock = ParseBlock();
	while (Check(TokenType::Catch))
	{
		auto& catchh = tryy->Catches.emplace_back();
		Ensure(TokenType::LeftParenthesis, "expected, opening parenthesis '('");
		catchh.Param = ParseParam();
		Ensure(TokenType::RightParenthesis, "expected, opening parenthesis ')'");
		catchh.ExecBlock = ParseBlock();
	}

	if (tryy->Catches.size() == 0)
	{
		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			"expected catch block"
		);
	}

	return tryy;
}

bool Parser::IsDefinition()
{
	auto& tok = Advance();
	m_Tok--;

	switch (tok.Type)
	{
	case TokenType::Function: 
	case TokenType::Class: 
	case TokenType::Variable:
	case TokenType::Const:
	case TokenType::Static: return true;
	default: return false;
	}
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

	Advance();
	if (Previous().Type != type)
	{
		m_Diagnostics.emplace_back(
			Previous().Marker,
			DiagnosticSeverity::Error,
			message
		);
		throw -1;
	}

	return Previous();
}

}
