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
#include <istream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "Global.h"
#include "CompileContext.h"
#include "Diagnostic.h"

namespace Wave {

template<typename T>
using up = std::unique_ptr<T>;

/// Type of a lexer token.
enum class TokenType
{
	// Single character tokens
	LeftParenthesis, RightParenthesis, 
	LeftBrace, RightBrace,
	LeftIndex, RightIndex,
	Comma, Period, Minus, Plus, Colon, Semicolon, Slash, Star,
	
	// Single or double character tokens
	Not, NotEqual,
	Equal, EqualEqual,
	Greater, GreaterEqual,
	Lesser, LesserEqual,

	// Literals
	Identifier, String, Integer, Real,

	// Keywords
	And, Or, 
	If, Else,
	True, False,
	For, While,
	Try, Catch, Throw,
	Class, Construct, Abstract,
	Static, Const, Copy,
	Public, Private, Protected,
	Self, Super,
	Function, Return,
	Variable,
	Type, TypeOf,
	IntegerType, RealType, StringType, BoolType,
	Module, Import, As, Export,

	Null
};

/// Lexer token.
struct WAVEC_API Token
{
	Token()
		: Marker(""), Type(TokenType::Null)
	{}

	/// Construct a token.
	///
	/// \param marker Marker of the token.
	/// \param type Type of the token.
	Token(FileMarker marker, TokenType type)
		: Marker(marker), Type(type)
	{}

	/// Construct a token.
	///
	/// \param marker Marker of the token.
	/// \param type Type of the token.
	/// \param value Value of the token.
	Token(FileMarker marker, TokenType type, const std::variant<std::string, int64_t, double>& value)
		: Marker(marker), Type(type), Value(value)
	{}

	/// Type of the token.
	TokenType Type;

	/// Marker of the entire token.
	FileMarker Marker;

	/// String value of the token.
	std::variant<std::string, int64_t, double> Value;
};

/// Wave lexer.
class WAVEC_API Lexer
{
public:
	/// Initialize a lexer from an input stream.
	///
	/// \param context Compile context to use for lexing.
	/// \param filePath The path of the file.
	/// \param stream std::istream to read from.
	Lexer(CompileContext& context, const std::filesystem::path& filePath, std::istream& stream);

	/// Run the lexical analyzer.
	void Lex();

	/// Print out all tokens to standard output.
	void PrettyPrint();

	/// Get the diagnostics from lexical analysis.
	///
	/// \return std::vector containing all diagnostics.
	const std::vector<Diagnostic>& GetDiagnostics();

	/// Get the lexical tokens.
	///
	/// \return std::vector of the tokens.
	const std::vector<Token>& GetTokens() const;

private:
	/// Get the next character in the input stream.
	/// Update the length of the FileMarker.
	/// 
	/// \return The character.
	char GetChar();

	/// Look ahead at the next characters.
	/// Updates the FileMarker if the character was found.
	/// 
	/// \param c String to match with.
	/// 
	/// \return If the character was found
	bool LookAhead(char c);

	/// Peeks at the next character.
	///
	/// \return The next character.
	char Peek();

	/// Push a token into the token list.
	/// Uses the FileMarker to push the token, and resets the marker's state.
	/// 
	/// \param type Type of the token to push.
	void PushToken(TokenType type);

	/// Push a valued token into the token list.
	/// Resets the marker's state.
	/// 
	/// \param type Token type.
	/// \param value Value to push.
	void PushToken(TokenType type, const std::variant<std::string, int64_t, double>& value);

	/// Push a string literal into the token list.
	void StringLiteral();

	/// Push a number literal into the token list.
	/// 
	/// \param c The current character.
	void NumberLiteral(char c);

	/// Push an identifier into the token list.
	///
	/// \param c The current character.
	void Identifier(char c);

	CompileContext& m_Context;
	std::istream& m_Stream;
	std::vector<Diagnostic> m_Diagnostics;
	FileMarker m_Marker;
	std::vector<Token> m_Tokens;
};

}
