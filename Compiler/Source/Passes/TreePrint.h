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

#include <string>

#include "Parser/Grammar.h"

namespace Wave {

/// Pretty-print the AST to standard output.
class TreePrinter : ASTVisitor
{
public:
	/// Constructor.
	///
	/// \param module The module to print.
	TreePrinter(Module* module);

	void Print();

	virtual void Visit(Abstract& node, std::any& context) override;

	virtual void Visit(ArrayIndex& node, std::any& context) override;

	virtual void Visit(ArrayType& node, std::any& context) override;

	virtual void Visit(Assignment& node, std::any& context) override;

	virtual void Visit(Binary& node, std::any& context) override;

	virtual void Visit(Block& node, std::any& context) override;

	virtual void Visit(BoolType& node, std::any& context) override;

	virtual void Visit(Break& node, std::any& context) override;

	virtual void Visit(Call& node, std::any& context) override;

	virtual void Visit(ClassDefinition& node, std::any& context) override;

	virtual void Visit(ClassType& node, std::any& context) override;

	virtual void Visit(ClassVar& node, std::any& context) override;

	virtual void Visit(Constructor& node, std::any& context) override;

	virtual void Visit(Continue& node, std::any& context) override;

	virtual void Visit(ExpressionStatement& node, std::any& context) override;

	virtual void Visit(For& node, std::any& context) override;

	virtual void Visit(Function& node, std::any& context) override;

	virtual void Visit(FunctionDefinition& node, std::any& context) override;

	virtual void Visit(FuncType& node, std::any& context) override;

	virtual void Visit(GenericType& node, std::any& context) override;

	virtual void Visit(Getter& node, std::any& context) override;

	virtual void Visit(Group& node, std::any& context) override;

	virtual void Visit(If& node, std::any& context) override;

	virtual void Visit(IntType& node, std::any& context) override;

	virtual void Visit(Literal& node, std::any& context) override;

	virtual void Visit(Logical& node, std::any& context) override;

	virtual void Visit(Method& node, std::any& context) override;

	virtual void Visit(RealType& node, std::any& context) override;

	virtual void Visit(Return& node, std::any& context) override;

	virtual void Visit(Setter& node, std::any& context) override;

	virtual void Visit(CharType& node, std::any& context) override;

	virtual void Visit(Throw& node, std::any& context) override;

	virtual void Visit(Try& node, std::any& context) override;

	virtual void Visit(TypeOf& node, std::any& context) override;

	virtual void Visit(Unary& node, std::any& context) override;

	virtual void Visit(VarAccess& node, std::any& context) override;

	virtual void Visit(VarDefinition& node, std::any& context) override;

	virtual void Visit(While& node, std::any& context) override;

private:
	std::string Indent(const std::any& indentLevel);
	void IncIndent(std::any& indentLevel) { indentLevel = std::any_cast<uint64_t>(indentLevel) + 2; }
	void DecIndent(std::any& indentLevel) { indentLevel = std::any_cast<uint64_t>(indentLevel) - 2; }

	Module* m_Module;
};

}
