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

#include "Lexer.h"

#include <iostream>
#include <map>
#include <sstream>

namespace Wave {

Lexer::Lexer(CompileContext& context, const std::filesystem::path& filePath, std::istream& stream)
	: m_Context(context), m_Marker(filePath), m_Stream(stream)
{}

bool IsAlphabet(char c)
{
	return (c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z') ||
		c == '_';
}

void Lexer::Lex()
{
	while (m_Stream.good())
	{
		char c = GetChar();

		switch (c)
		{
		// Single character tokens
		case '(': PushToken(TokenType::LeftParenthesis); break;
		case ')': PushToken(TokenType::RightParenthesis); break;
		case '{': PushToken(TokenType::LeftBrace); break;
		case '}': PushToken(TokenType::RightBrace); break;
		case '[': PushToken(TokenType::LeftIndex); break;
		case ']': PushToken(TokenType::RightIndex); break;
		case ',': PushToken(TokenType::Comma); break;
		case '.': PushToken(TokenType::Period); break;
		case '-':
			if (LookAhead('=')) { PushToken(TokenType::MinusEqual); }
			else { PushToken(TokenType::Minus); }
			break;
		case '+': 
			if (LookAhead('=')) { PushToken(TokenType::PlusEqual); }
			else { PushToken(TokenType::Plus); }
			break;
		case ':': PushToken(TokenType::Colon); break;
		case ';': PushToken(TokenType::Semicolon); break;
		case '*': 
			if (LookAhead('=')) { PushToken(TokenType::StarEqual); }
			else { PushToken(TokenType::Star); }
			break;
		case '%':
			if (LookAhead('=')) { PushToken(TokenType::PercentageEqual); }
			else { PushToken(TokenType::Percentage); }
			break;
		// Double character tokens
		case '=':
			PushToken(LookAhead('=') ? TokenType::EqualEqual : TokenType::Equal);
			break;
		case '!':
			PushToken(LookAhead('=') ? TokenType::NotEqual : TokenType::Not);
			break;
		case '>':
			PushToken(LookAhead('=') ? TokenType::GreaterEqual : TokenType::Greater);
			break;
		case '<':
			PushToken(LookAhead('=') ? TokenType::LesserEqual : TokenType::Lesser);
			break;
		// Comments are special
		case '/':
			if (LookAhead('/'))
			{
				char c;
				do { c = GetChar(); } while (c != '\n' && m_Stream.good());

				m_Marker.Pos += m_Marker.Length;
				m_Marker.Length = 0;
			}
			else if (LookAhead('*'))
			{
				FileMarker marker = m_Marker;

				bool star, slash;
				do 
				{
					star = GetChar() == '*';
					slash = LookAhead('/');
				} while (!(star && slash) && m_Stream.good());

				// We hit the end of the stream.
				if (!slash || !star)
				{
					m_Diagnostics.emplace_back(
						marker,
						DiagnosticSeverity::Error,
						"multiline comment did not end"
					);
				}

				m_Marker.Pos += m_Marker.Length;
				m_Marker.Length = 0;
			}
			else if (LookAhead('=')) { PushToken(TokenType::SlashEqual); }
			else { PushToken(TokenType::Slash); }
			break;
		// Literals
		case '"': StringLiteral(); break;
		// Numbers
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': NumberLiteral(c); break;
		// Whitespace
		case ' ':
		case '\r':
		case '\t':
		case '\n':
			m_Marker.Pos += m_Marker.Length;
			m_Marker.Length = 0;
			break;
		case EOF:
			goto End;
		default:
			if (IsAlphabet(c)) { Identifier(c); }
			else
			{
				std::ostringstream ss;
				ss << "Unexpected character '" << c << "'";

				m_Diagnostics.emplace_back(
					m_Marker,
					DiagnosticSeverity::Error,
					ss.str()
				);
				m_Marker.Pos += m_Marker.Length;
				m_Marker.Length = 0;
			}
		}
	}

	PushToken(TokenType::Null);

	End:

	if (m_Context.IsDebugOutputEnabled()) 
	{
		std::cout << "LEXER OUTPUT: \n\n";
		PrettyPrint(); 
	}
}

void Lexer::PrettyPrint()
{
	for (auto& token : m_Tokens)
	{
		std::cout << "Pos: " << token.Marker.Pos << ", Length: " << token.Marker.Length << "\n";
		PrettyPrint(token);

		std::cout << "\n\n";
	}
}

void Lexer::PrettyPrint(const Token& token)
{
	switch (token.Type)
	{
	case TokenType::LeftParenthesis: std::cout << "("; break;
	case TokenType::RightParenthesis: std::cout << ")"; break;
	case TokenType::LeftBrace: std::cout << "{"; break;
	case TokenType::RightBrace: std::cout << "}"; break;
	case TokenType::LeftIndex: std::cout << "["; break;
	case TokenType::RightIndex: std::cout << "]"; break;
	case TokenType::Comma: std::cout << ","; break;
	case TokenType::Period: std::cout << "."; break;
	case TokenType::Minus: std::cout << "-"; break;
	case TokenType::MinusEqual: std::cout << "-="; break;
	case TokenType::Plus: std::cout << "+"; break;
	case TokenType::PlusEqual: std::cout << "+="; break;
	case TokenType::Colon: std::cout << ":"; break;
	case TokenType::Semicolon: std::cout << ";"; break;
	case TokenType::Slash: std::cout << "/"; break;
	case TokenType::SlashEqual: std::cout << "/="; break;
	case TokenType::Star: std::cout << "*"; break;
	case TokenType::StarEqual: std::cout << "*="; break;
	case TokenType::Percentage: std::cout << "%"; break;
	case TokenType::PercentageEqual: std::cout << "%="; break;
	case TokenType::Not: std::cout << "!"; break;
	case TokenType::NotEqual: std::cout << "!="; break;
	case TokenType::Equal: std::cout << "="; break;
	case TokenType::EqualEqual: std::cout << "=="; break;
	case TokenType::Greater: std::cout << ">"; break;
	case TokenType::GreaterEqual: std::cout << ">="; break;
	case TokenType::Lesser: std::cout << "<"; break;
	case TokenType::LesserEqual: std::cout << "<="; break;
	case TokenType::Identifier: std::visit([](auto& val) { std::cout << val; }, token.Value); break;
	case TokenType::String: std::visit([](auto& val) { std::cout << val; }, token.Value); break;
	case TokenType::Integer: std::visit([](auto& val) { std::cout << val; }, token.Value); break;
	case TokenType::Real: std::visit([](auto& val) { std::cout << val; }, token.Value); break;
	case TokenType::And: std::cout << "and"; break;
	case TokenType::Or: std::cout << "or"; break;
	case TokenType::If: std::cout << "if"; break;
	case TokenType::Else: std::cout << "else"; break;
	case TokenType::True: std::cout << "true"; break;
	case TokenType::False: std::cout << "false"; break;
	case TokenType::For: std::cout << "for"; break;
	case TokenType::In: std::cout << "in"; break;
	case TokenType::While: std::cout << "while"; break;
	case TokenType::Break: std::cout << "break"; break;
	case TokenType::Continue: std::cout << "continue"; break;
	case TokenType::Try: std::cout << "try"; break;
	case TokenType::Catch: std::cout << "catch"; break;
	case TokenType::Throw: std::cout << "throw"; break;
	case TokenType::Enum: std::cout << "enum"; break;
	case TokenType::Tuple: std::cout << "tuple"; break;
	case TokenType::Class: std::cout << "class"; break;
	case TokenType::Construct: std::cout << "construct"; break;
	case TokenType::Abstract: std::cout << "abstract"; break;
	case TokenType::Static: std::cout << "static"; break;
	case TokenType::Copy: std::cout << "copy"; break;
	case TokenType::Const: std::cout << "const"; break;
	case TokenType::Public: std::cout << "public"; break;
	case TokenType::Protected: std::cout << "protected"; break;
	case TokenType::Private: std::cout << "private"; break;
	case TokenType::Self: std::cout << "self"; break;
	case TokenType::Super: std::cout << "super"; break;
	case TokenType::Function: std::cout << "func"; break;
	case TokenType::Return: std::cout << "return"; break;
	case TokenType::Variable: std::cout << "var"; break;
	case TokenType::Type: std::cout << "type"; break;
	case TokenType::TypeOf: std::cout << "typeof"; break;
	case TokenType::IntegerType: std::cout << "int"; break;
	case TokenType::RealType: std::cout << "real"; break;
	case TokenType::CharType: std::cout << "char"; break;
	case TokenType::BoolType: std::cout << "bool"; break;
	case TokenType::Module: std::cout << "module"; break;
	case TokenType::Import: std::cout << "import"; break;
	case TokenType::Extern: std::cout << "extern"; break;
	case TokenType::As: std::cout << "as"; break;
	case TokenType::Export: std::cout << "export"; break;
	case TokenType::Null: break;
	}
}

const std::vector<Wave::Diagnostic>& Lexer::GetDiagnostics()
{
	return m_Diagnostics;
}

const std::vector<Wave::Token>& Lexer::GetTokens() const
{
	return m_Tokens;
}

char Lexer::GetChar()
{
	char c;
	m_Stream.get(c);

	m_Marker.Length++;
	
	return c;
}

bool Lexer::LookAhead(char c)
{
	char next;
	m_Stream.get(next);

	if (next == c)
	{
		m_Marker.Length++;
		return true;
	}
	else
	{
		m_Stream.seekg(-1, std::ios_base::cur);
		return false;
	}
}

char Lexer::Peek()
{
	char next;
	m_Stream.get(next);
	m_Stream.seekg(-1, std::ios_base::cur);
	return next;
}

void Lexer::PushToken(TokenType type)
{
	m_Tokens.emplace_back(m_Marker, type);
	m_Marker.Pos += m_Marker.Length;
	m_Marker.Length = 0;
}

void Lexer::PushToken(TokenType type, const std::variant<std::string, int64_t, double>& value)
{
	m_Tokens.emplace_back(m_Marker, type, value);
	m_Marker.Pos += m_Marker.Length;
	m_Marker.Length = 0;
}

void Lexer::StringLiteral()
{
	std::string source;
	bool quote, slash;

	do
	{
		if (LookAhead('\n'))
		{
			m_Diagnostics.emplace_back(
				m_Marker,
				DiagnosticSeverity::Error,
				"string not terminated"
			);
			return;
		}

		char c = GetChar();
		
		// All just to allow escaped double quotes
		slash = c == '\\';
		quote = LookAhead('"');

		if (quote && slash)
		{
			source += '"';
		}
		else
		{
			source += c;
		}
	} while (!quote || (quote && slash));

	if (!m_Stream.good())
	{
		m_Diagnostics.emplace_back(
			m_Marker,
			DiagnosticSeverity::Error,
			"string not terminated"
		);
		return;
	}

	std::string value;

	// Unescaping other stuff now
	for (uint64_t i = 0; i < source.size(); i++)
	{
		if (source[i] == '\\')
		{
			switch (source[i + 1])
			{
			case 'a': value += '\a'; break;
			case 'n': value += '\n'; break;
			case 't': value += '\t'; break;
			case '\\': value += '\\'; break;
			default:
				std::ostringstream ss;
				FileMarker marker = m_Marker;
				marker.Pos += i + 2;
				marker.Length = 2;
				ss << "unrecognized escape sequence '\\" << source[i + 1] << '\'';
				m_Diagnostics.emplace_back(
					marker,
					DiagnosticSeverity::Error,
					ss.str()
				);
				break;
			}
		}
		else
		{
			value += source[i];
		}
	}

	PushToken(TokenType::String, value);
}

void Lexer::NumberLiteral(char c)
{
	std::string literal;
	literal += c;

	char n = Peek();
	while (n >= '0' && n <= '9')
	{
		literal += n;
		GetChar();
		n = Peek();
	}

	bool isDecimal = false;
	if (n == '.')
	{
		GetChar();
		isDecimal = true;

		literal += n;
		n = Peek();
		while (n >= '0' && n <= '9')
		{
			literal += n;
			GetChar();
			n = Peek();
		}
	}

	if (isDecimal)
	{
		PushToken(TokenType::Real, std::stod(literal));
	}
	else
	{
		PushToken(TokenType::Integer, std::stoll(literal));
	}
}

bool IsAlphanumeric(char c)
{
	return IsAlphabet(c) ||
		(c >= '0' && c <= '9');
}

static std::map<std::string, TokenType> s_Reserved =
{
	{ "and", TokenType::And },
	{ "or", TokenType::Or },
	{ "if", TokenType::If },
	{ "else", TokenType::Else },
	{ "true", TokenType::True },
	{ "false", TokenType::False },
	{ "for", TokenType::For },
	{ "in", TokenType::In },
	{ "while", TokenType::While },
	{ "break", TokenType::Break },
	{ "continue", TokenType::Continue },
	{ "try", TokenType::Try },
	{ "catch", TokenType::Catch },
	{ "throw", TokenType::Throw },
	{ "enum", TokenType::Enum },
	{ "tuple", TokenType::Tuple },
	{ "class", TokenType::Class },
	{ "construct", TokenType::Construct },
	{ "abstract", TokenType::Abstract },
	{ "static", TokenType::Static },
	{ "copy", TokenType::Copy },
	{ "const", TokenType::Const },
	{ "public", TokenType::Public },
	{ "protected", TokenType::Protected },
	{ "private", TokenType::Private },
	{ "self", TokenType::Self },
	{ "super", TokenType::Super },
	{ "func", TokenType::Function },
	{ "return", TokenType::Return },
	{ "var", TokenType::Variable },
	{ "type", TokenType::Type },
	{ "typeof", TokenType::TypeOf },
	{ "int", TokenType::IntegerType },
	{ "real", TokenType::RealType },
	{ "char", TokenType::CharType },
	{ "bool", TokenType::BoolType },
	{ "module", TokenType::Module },
	{ "import", TokenType::Import },
	{ "extern", TokenType::Extern },
	{ "as", TokenType::As },
	{ "export", TokenType::Export }
};

void Lexer::Identifier(char c)
{
	std::string literal;
	literal += c;

	char n = Peek();
	while (IsAlphanumeric(n))
	{
		literal += n;
		GetChar();
		n = Peek();
	}

	if (s_Reserved.count(literal))
	{
		PushToken(s_Reserved[literal]);
	}
	else
	{
		PushToken(TokenType::Identifier, literal);
	}
}

}
