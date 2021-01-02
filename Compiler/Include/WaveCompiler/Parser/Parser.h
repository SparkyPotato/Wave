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

#include "Expression.h"

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
	/// Parse expression.
	sp<Expression> Expr();

	/// Parse equality.
	sp<Expression> Equality();

	/// Parse comparison.
	sp<Expression> Comparision();

	/// Parse term.
	sp<Expression> Term();

	/// Parse factor.
	sp<Expression> Factor();

	/// Parse unary.
	sp<Expression> Unary();

	/// Parse primary.
	sp<Expression> Primary();

	/// Look ahead at the next token.
	///
	/// \param type TokenType to match with.
	/// 
	/// \return If the next token type matched type.
	bool LookAhead(TokenType type);

	/// Get the next token and advance the stream.
	///
	/// \return The next token.
	const Token& Advance();

	/// Check if the stream still has tokens.
	///
	/// \return If the stream has more tokens.
	bool IsGood();

	/// Peek at the current token.
	///
	/// \return The current token.
	const Token& Peek();

	/// Get the last token.
	///
	/// \return The previous token.
	const Token& Previous();

	/// Error check.
	///
	/// \param type Type to check for.
	/// \param message Error message.
	/// 
	/// \return The consumed token.
	const Token& ErrorCheck(TokenType type, const std::string& message);

	/// Synchronize after error.
	void Synchronize();

	const std::vector<Token>& m_Tokens;
	uint64_t m_Tok = 0;
	std::vector<Diagnostic> m_Diagnostics;
	std::vector<sp<Expression>> m_Expressions;
};

}
