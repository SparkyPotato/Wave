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

#include "Grammar.h"

namespace Wave {

/// Parse a list of tokens to form an AST.
class WAVEC_API Parser
{
public:
	/// Construct a parser.
	///
	/// \param lexer Lexer which has run.
	Parser(const Lexer& lexer);

	/// Parse to form an AST.
	void Parse();

	/// Get diagnostics.
	///
	/// \return std::vector of diagnostics.
	const std::vector<Diagnostic>& GetDiagnostics();

private:
	/// Parse Identifier. Expects cursor to be at the first token of the identifier.
	///
	/// \return Parsed identifier.
	Identifier ParseIdentifier();

	/// Parse module import. Expects cursor to be after the import keyword.
	///
	/// \return The module import.
	ModuleImport ParseImport();

	/// Parse global definition. Expects cursor to be at first token of definition.
	///
	/// \return The global definition.
	GlobalDefinition ParseGlobalDefinition();

	/// Parse a definition. Expects cursor to be at first token of definition.
	///
	/// \return The definition.
	up<Definition> ParseDefinition();

	/// Parse a function definition. Expects cursor to be after func keyword.
	///
	/// \return The function definition.
	up<FunctionDefinition> ParseFunctionDefinition();

	/// Parse a variable definition. Expects cursor to be at after var, const, or static.
	///
	/// \return The variable definition.
	up<VarDefinition> ParseVarDefinition();

	/// Parse a class definition. Expects cursor to be after class keyword.
	///
	/// \return The class definition.
	up<ClassDefinition> ParseClassDefinition();

	/// Parse a class variable. Expects cursor to be after var keywords.
	///
	/// \return The class variable.
	up<ClassVar> ParseClassVar();

	/// Parse a class function. Expects cursor to be at the first token.
	///
	/// \return The class function.
	up<ClassFunc> ParseClassFunc();

	/// Parse a getter or a setter. Expects cursor to be at the identifier.
	///
	/// \return The getter or setter.
	up<ClassFunc> ParseGetterOrSetter();

	/// Parse a constructor. Expects cursor to be after the construct.
	///
	/// \return The constructor.
	up<Constructor> ParseConstructor();

	/// Parse a type. Expects cursor to be at the type keyword.
	///
	/// \return The type.
	up<Type> ParseType();

	/// Parse a typeof expression. Expects cursor to be after typeof keyword.
	///
	/// \return The type.
	up<TypeOf> ParseTypeOf();

	/// Parse a function type. Expects cursor to be after the func keyword.
	///
	/// \return The function type;
	up<FuncType> ParseFuncType();

	/// Parse an expression. Expects cursor to be at the first token of the expression.
	///
	/// \return The parsed expression.
	up<Expression> ParseExpression();

	/// Parse an anonymous function. Expects cursor to be on the first token of the function.
	///
	/// \return Function parsed.
	up<Function> ParseFunction();

	/// Parse a function parameter. Expects cursor to be on the first token of the parameter.
	///
	/// \return The parsed parameter.
	Parameter ParseParam();

	/// Parse a block of statements. Expects cursor to be on the first token of the block.
	///
	/// \return The parsed block.
	up<Block> ParseBlock();

	/// Parse a statement. Expects cursor to be on the first token of the statement.
	///
	/// \return The parsed statement.
	up<Statement> ParseStatement();

	/// Get token and advance cursor. Does no bounds-checking.
	///
	/// \return The token at the cursor before advancing.
	const Token& Advance();

	/// Peek at the token and advance if it matches type.
	///
	/// \param type Type to match.
	/// 
	/// \return If the type matched.
	bool Check(TokenType type);

	/// Peek at the current token without advancing the cursor. Does no bounds-checking.
	///
	/// \return The current token.
	const Token& Peek();

	/// Get the previous token. Does no bounds-checking.
	///
	/// \return The previous token.
	const Token& Previous();

	/// Check if the token stream has more tokens.
	///
	/// \return If it is safe to call Advance().
	bool IsGood();

	/// Ensure the current token is of type, and advance. 
	/// Advances even if the check fails.
	/// 
	/// \param type Type to check for.
	/// \param message Error message.
	/// 
	/// \return The current token.
	/// 
	/// \throw int if the check fails.
	const Token& Ensure(TokenType type, const std::string& message);

	const std::vector<Token>& m_Tokens;
	uint64_t m_Tok = 0;
	std::vector<Diagnostic> m_Diagnostics;
	up<Module> m_Module;
};

}
