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

#include "Passes/TreePrint.h"

#include <iostream>

namespace Wave {

TreePrinter::TreePrinter(Module* module)
	: m_Module(module)
{}

std::ostream& operator<<(std::ostream& os, const Identifier& id)
{
	for (auto& ident : id.Path)
	{
		Lexer::PrettyPrint(ident);
		std::cout << ".";
	}

	return os;
}

void TreePrinter::Print()
{
	std::cout << "Module ";
	std::cout << m_Module->Def;
	std::cout << "\n\n";

	std::cout << "Imports: \n";
	for (auto& import : m_Module->Imports)
	{
		std::cout << import.Imported;

		std::cout << " as ";

		std::cout << import.As;
	}
	std::cout << "\n\n";

	std::cout << "External Imports: \n";
	for (auto& import : m_Module->CImports)
	{
		Lexer::PrettyPrint(import.Path);
	}
	std::cout << "\n\n";

	std::cout << "Definitions: \n";
	for (auto& def : m_Module->Definitions)
	{
		if (def.Exported) { std::cout << "exported "; }
		std::any indentLevel = uint64_t(0);
		def.Def->Accept(*this, indentLevel);
		std::cout << "\n\n";
	}
}

void TreePrinter::Visit(Abstract& node, std::any& context)
{
	if (node.IsConst) { std::cout << "const "; }
	std::cout << "abstract ";
	Lexer::PrettyPrint(node.Ident);
	std::cout << " (";
	for (auto& param : node.Params)
	{
		if (param.IsConst) { std::cout << "const "; }
		Lexer::PrettyPrint(param.Ident);
		std::cout << " : ";
		param.DataType->Accept(*this, context);
		std::cout << ", ";
	}
	std::cout << "): ";
	if (node.ReturnType) { node.ReturnType->Accept(*this, context); }
}

void TreePrinter::Visit(ArrayIndex& node, std::any& context)
{
	if (node.IsCopy) { std::cout << "copy "; }
	std::cout << node.Var << "[";
	node.Index->Accept(*this, context);
	std::cout << "]";
}

void TreePrinter::Visit(ArrayType& node, std::any& context)
{
	node.HoldType->Accept(*this, context);
	std::cout << "[";
	if (node.Size) { node.Size->Accept(*this, context); }
	std::cout << "]";
}

void TreePrinter::Visit(Assignment& node, std::any& context)
{
	std::cout << node.Var;
	node.Value->Accept(*this, context);
}

void TreePrinter::Visit(Binary& node, std::any& context)
{
	node.Left->Accept(*this, context);
	Lexer::PrettyPrint(node.Operator);
	node.Right->Accept(*this, context);
}

void TreePrinter::Visit(Block& node, std::any& context)
{
	std::cout << "{\n";
	IncIndent(context);
	for (auto& state : node.Statements)
	{
		state->Accept(*this, context);
		std::cout << "\n" << Indent(context);
	}
	DecIndent(context);
	std::cout << "}\n" << Indent(context);
}

void TreePrinter::Visit(BoolType& node, std::any& context)
{
	std::cout << "bool";
}

void TreePrinter::Visit(Break& node, std::any& context)
{
	std::cout << "break;";
}

void TreePrinter::Visit(Call& node, std::any& context)
{
	node.Callee->Accept(*this, context);
	std::cout << "(";
	for (auto& arg : node.Args)
	{
		arg->Accept(*this, context);
		std::cout << ", ";
	}
	std::cout << ")";
}

void TreePrinter::Visit(ClassDefinition& node, std::any& context)
{
	std::cout << "class ";
	
	Lexer::PrettyPrint(node.Ident);
	for (auto& base : node.Bases)
	{
		std::cout << base << ", ";
	}
	std::cout << " {\n" << Indent(context) << "public:\n" << Indent(context);
	IncIndent(context);
	for (auto& pub : node.Public)
	{
		pub->Accept(*this, context);
	}
	DecIndent(context);
	std::cout << "\n" << Indent(context);

	std::cout << Indent(context) << "protected:\n" << Indent(context);
	IncIndent(context);
	for (auto& pub : node.Protected)
	{
		pub->Accept(*this, context);
	}
	DecIndent(context);

	std::cout << Indent(context) << "private:\n" << Indent(context);
	IncIndent(context);
	for (auto& pub : node.Private)
	{
		pub->Accept(*this, context);
	}
	DecIndent(context);

	std::cout << Indent(context) << "}";
}

void TreePrinter::Visit(ClassType& node, std::any& context)
{
	std::cout << node.Ident;
}

void TreePrinter::Visit(Constructor& node, std::any& context)
{
	std::cout << "construct (";
	for (auto& par : node.Params)
	{
		if (par.IsConst) { std::cout << "const "; }
		Lexer::PrettyPrint(par.Ident);
		std::cout << " : ";
		par.DataType->Accept(*this, context);
		std::cout << ", ";
	}
	node.ExecBlock->Accept(*this, context);
}

void TreePrinter::Visit(Continue& node, std::any& context)
{
	std::cout << "continue;";
}

void TreePrinter::Visit(EnumDefinition& node, std::any& context)
{
	std::cout << "enum ";
	Lexer::PrettyPrint(node.Ident);
	std::cout << "{\n" << Indent(context);
	IncIndent(context);
	for (auto& tok : node.Elements)
	{
		Lexer::PrettyPrint(tok);
		std::cout << "\n" << Indent(context);
	}
	DecIndent(context);
	std::cout << "};";
}

void TreePrinter::Visit(ExpressionStatement& node, std::any& context)
{
	node.Expr->Accept(*this, context);
	std::cout << "\n" << Indent(context);
}

void TreePrinter::Visit(For& node, std::any& context)
{
	std::cout << "for ";
	if (node.Condition.index() == 0)
	{
		ForCond& cond = std::get<ForCond>(node.Condition);
		std::visit([&](auto& v) { v->Accept(*this, context); }, cond.Initializer);
		std::cout << " ; ";
		if (cond.Condition) { cond.Condition->Accept(*this, context); }
		std::cout << " ; ";
		if (cond.Increment) { cond.Increment->Accept(*this, context); }
	}
	else
	{
		ForRange& range = std::get<ForRange>(node.Condition);
		Lexer::PrettyPrint(range.Ident);
		std::cout << " in ";
		range.Range->Accept(*this, context);
		std::cout << " ";
	}

	node.ExecBlock->Accept(*this, context);
}

void TreePrinter::Visit(Function& node, std::any& context)
{
	std::cout << "(";
	for (auto& par : node.Params)
	{
		if (par.IsConst) { std::cout << "const "; }
		Lexer::PrettyPrint(par.Ident);
		std::cout << " : ";
		par.DataType->Accept(*this, context);
		std::cout << ", ";
	}
	std::cout << ") : ";
	if (node.IsReturnConst) { std::cout << "const"; }
	if (node.ReturnType) { node.ReturnType->Accept(*this, context); }
	node.ExecBlock->Accept(*this, context);
}

void TreePrinter::Visit(FunctionDefinition& node, std::any& context)
{
	std::cout << "func ";
	Lexer::PrettyPrint(node.Ident);
	std::cout << " ";
	node.Func->Accept(*this, context);
}

void TreePrinter::Visit(FuncType& node, std::any& context)
{
	std::cout << "func (";
	for (auto& par : node.ParamTypes)
	{
		par->Accept(*this, context);
		std::cout << ", ";
	}
	std::cout << ")";
	if (node.ReturnType) { std::cout << ": "; node.ReturnType->Accept(*this, context); }
}

void TreePrinter::Visit(GenericType& node, std::any& context)
{
	std::cout << "generic";
}

void TreePrinter::Visit(Getter& node, std::any& context)
{
	Lexer::PrettyPrint(node.Ident);
	std::cout << ": ";
	node.GetType->Accept(*this, context);
	node.ExecBlock->Accept(*this, context);
}

void TreePrinter::Visit(Group& node, std::any& context)
{
	std::cout << "(";
	node.Expr->Accept(*this, context);
	std::cout << ")";
}

void TreePrinter::Visit(If& node, std::any& context)
{
	std::cout << "if ";
	node.Condition->Accept(*this, context);
	node.True->Accept(*this, context);
	for (auto& elif : node.ElseIfs)
	{
		elif.Condition->Accept(*this, context);
		elif.True->Accept(*this, context);
	}
	if (node.Else) { node.Else->Accept(*this, context); }
}

void TreePrinter::Visit(InitializerList& node, std::any& context)
{
	std::cout << "{\n";
	IncIndent(context);
	std::cout << Indent(context);
	for (auto& exp : node.Data)
	{
		exp->Accept(*this, context);
		std::cout << ",\n" << Indent(context);
	}
	DecIndent(context);
	std::cout << "\n" << Indent(context) << "}";
}

void TreePrinter::Visit(IntType& node, std::any& context)
{
	std::cout << "int";
}

void TreePrinter::Visit(Literal& node, std::any& context)
{
	Lexer::PrettyPrint(node.Value);
}

void TreePrinter::Visit(Logical& node, std::any& context)
{
	node.Left->Accept(*this, context);
	Lexer::PrettyPrint(node.Operator);
	node.Right->Accept(*this, context);
}

void TreePrinter::Visit(Method& node, std::any& context)
{
	if (node.IsConst) { std::cout << "const "; }
	else if (node.IsStatic) { std::cout << "static "; }
	node.Def->Accept(*this, context);
}

void TreePrinter::Visit(RealType& node, std::any& context)
{
	std::cout << "real";
}

void TreePrinter::Visit(Return& node, std::any& context)
{
	std::cout << "return ";
	node.Value->Accept(*this, context);
}

void TreePrinter::Visit(Setter& node, std::any& context)
{
	Lexer::PrettyPrint(node.Ident);
	std::cout << "(";
	if (node.SetParam.IsConst) { std::cout << "const "; }
	Lexer::PrettyPrint(node.SetParam.Ident);
	std::cout << " : ";
	node.SetParam.DataType->Accept(*this, context);
	std::cout << ") ";
	node.ExecBlock->Accept(*this, context);
}

void TreePrinter::Visit(CharType& node, std::any& context)
{
	std::cout << "char";
}

void TreePrinter::Visit(Throw& node, std::any& context)
{
	std::cout << "throw ";
	node.Value->Accept(*this, context);
}

void TreePrinter::Visit(Try& node, std::any& context)
{
	std::cout << "try ";
	node.ExecBlock->Accept(*this, context);
	for (auto& cat : node.Catches)
	{
		std::cout << "catch ";
		if (cat.Param.IsConst) { std::cout << "const "; }
		Lexer::PrettyPrint(cat.Param.Ident);
		std::cout << " : ";
		cat.Param.DataType->Accept(*this, context);
		std::cout << " ";
		cat.ExecBlock->Accept(*this, context);
	}
}

void TreePrinter::Visit(TypeOf& node, std::any& context)
{
	std::cout << "typeof  ";
	node.Expr->Accept(*this, context);
}

void TreePrinter::Visit(Unary& node, std::any& context)
{
	Lexer::PrettyPrint(node.Operator);
	node.Right->Accept(*this, context);
}

void TreePrinter::Visit(VarAccess& node, std::any& context)
{
	if (node.IsCopy) { std::cout << "copy "; }
	std::cout << node.Var;
}

void TreePrinter::Visit(VarDefinition& node, std::any& context)
{
	Lexer::PrettyPrint(node.VarType);
	std::cout << " ";
	Lexer::PrettyPrint(node.Ident);
	if (node.DataType) { std::cout << ": "; node.DataType->Accept(*this, context); }
	if (node.Value) { std::cout << " = "; node.Value->Accept(*this, context); }
}

void TreePrinter::Visit(While& node, std::any& context)
{
	std::cout << "while ";
	node.Condition->Accept(*this, context);
	node.ExecBlock->Accept(*this, context);
}

std::string TreePrinter::Indent(const std::any& indentLevel)
{
	return std::string(std::any_cast<uint64_t>(indentLevel), ' ');
}

}
