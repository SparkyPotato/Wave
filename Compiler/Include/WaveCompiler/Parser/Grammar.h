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

#include "WaveCompiler/Lexer.h"

namespace Wave {

/// A single identifier like 'Module.Class.Method'.
struct Identifier
{
	/// List of identifiers in the path.
	std::vector<Token> Path;
};

/// Structure representing an imported module.
struct ModuleImport
{
	/// The module that was imported.
	Identifier Imported;

	/// The name the module was imported into.
	Identifier As;
};

struct CImport
{
	/// Imported C file.
	Token Path;
};

class ASTVisitor;

/// A statement.
struct Statement
{
	/// Virtual destructor.
	virtual ~Statement() {}

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) = 0;
};

/// The definition of a class, function, or variable.
struct Definition : Statement
{
	/// Local identifier of the definition.
	Token Ident;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) = 0;
};

/// A global definition in a module.
struct GlobalDefinition
{
	/// If the definition is exported.
	bool Exported = false;

	/// The definition.
	up<Definition> Def = nullptr;
};

/// Structure representing a module,
/// which is a single source file.
struct Module
{
	/// Definition of the module.
	Identifier Def;

	/// List of all imports.
	std::vector<ModuleImport> Imports;

	/// List of all C imports.
	std::vector<CImport> CImports;

	/// List of all global definitions.
	std::vector<GlobalDefinition> Definitions;

	/// Path of the module file.
	std::filesystem::path FilePath;
};

/// A data type.
struct Type
{
	/// Virtual destructor.
	virtual ~Type() {}

	Token Tok;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) = 0;
};

/// Type of integer.
struct IntType : Type
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Type of real.
struct RealType : Type
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Type of string.
struct CharType : Type
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Type of bool.
struct BoolType : Type
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

struct GenericType : Type 
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Type of function.
struct FuncType : Type
{
	up<Type> ReturnType;
	std::vector<up<Type>> ParamTypes;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Type of a class.
struct ClassType : Type
{
	Identifier Ident;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A parameter of a function.
struct Parameter
{
	bool IsConst = false;
	Token Ident;
	up<Type> DataType;
};

/// Class function.
struct ClassFunc : Definition
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) = 0;
};

/// An abstract method in a class.
struct Abstract : ClassFunc
{
	Token Ident;
	std::vector<Parameter> Params;
	up<Type> ReturnType;
	bool IsReturnConst = false;
	bool IsConst = false;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Class definition.
struct ClassDefinition : Definition
{
	std::vector<Identifier> Bases;
	std::vector<up<Definition>> Public;
	std::vector<up<Definition>> Protected;
	std::vector<up<Definition>> Private;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Enum definition.
struct EnumDefinition : Definition
{
	std::vector<Token> Elements;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Block of statements.
struct Block : Statement
{
	std::vector<up<Statement>> Statements;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

struct OperatorOverload : ClassFunc
{
	Token Operator;
	bool IsUnary = false;
	Parameter Left, Right;
	up<Block> ExecBlock;
	up<Type> ReturnType;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Class constructor.
struct Constructor : ClassFunc
{
	std::vector<Parameter> Params;
	up<Block> ExecBlock;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Class getter.
struct Getter : ClassFunc
{
	Token Ident;
	up<Type> GetType;
	up<Block> ExecBlock;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Class setter.
struct Setter : ClassFunc
{
	Token Ident;
	Parameter SetParam;
	up<Block> ExecBlock;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Break statement.
struct Break : Statement
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Continue statement.
struct Continue : Statement
{
	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// An expression.
struct Expression
{
	/// Virtual destructor.
	virtual ~Expression() {}

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) = 0;
};

struct ArrayType : Type
{
	up<Type> HoldType;
	up<Expression> Size;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

struct TupleType : Type
{
	std::vector<up<Type>> Types;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Type of an expression preceded by 'typeof'
struct TypeOf : Type
{
	up<Expression> Expr;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Return statement.
struct Return : Statement
{
	up<Expression> Value;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Definition of a variable.
struct VarDefinition : Definition
{
	Token VarType;
	up<Type> DataType;
	up<Expression> Value;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Statement which evaluates an expression and discards the result.
struct ExpressionStatement : Statement
{
	up<Expression> Expr;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// While loop.
struct While : Statement
{
	up<Expression> Condition;
	up<Block> ExecBlock;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// For loop condition.
struct ForCond
{
	std::variant<up<Expression>, up<Definition>> Initializer;
	up<Expression> Condition;
	up<Expression> Increment;
};

/// For loop range
struct ForRange
{
	Token Ident;
	up<Expression> Range;
};

/// For loop.
struct For : Statement
{
	std::variant<ForCond, ForRange> Condition;
	up<Block> ExecBlock;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Else if statement.
struct ElseIf
{
	up<Expression> Condition;
	up<Block> True;
};

/// If statement.
struct If : Statement
{
	up<Expression> Condition;
	up<Block> True;
	std::vector<ElseIf> ElseIfs;
	up<Block> Else;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

struct Catch
{
	up<Block> ExecBlock;
	Parameter Param;
};

struct Try : Statement
{
	up<Block> ExecBlock;
	std::vector<Catch> Catches;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

struct Throw : Statement
{
	up<Expression> Value;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A function, which could be anonymous.
struct Function : Expression
{
	std::vector<Parameter> Params;
	up<Type> ReturnType;
	bool IsReturnConst = false;
	bool IsVariadic = false;
	up<Block> ExecBlock;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// Function definition.
struct FunctionDefinition : Definition
{
	up<Function> Func;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};


/// Class member function.
struct Method : ClassFunc
{
	bool IsStatic = false;
	bool IsConst = false;
	up<FunctionDefinition> Def;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A variable assignment expression.
struct Assignment : Expression
{
	Identifier Var;
	up<Expression> Value;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A logical expression.
struct Logical : Expression
{
	up<Expression> Left;
	Token Operator;
	up<Expression> Right;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A binary expression.
struct Binary : Expression
{
	up<Expression> Left;
	Token Operator;
	up<Expression> Right;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};


/// A unary expression.
struct Unary : Expression
{
	Token Operator;
	up<Expression> Right;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A call expression.
struct Call : Expression
{
	up<Expression> Callee;
	std::vector<up<Expression>> Args;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A literal expression.
struct Literal : Expression
{
	Token Value;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A grouping expression.
struct Group : Expression
{
	up<Expression> Expr;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

struct InitializerList : Expression
{
	std::vector<up<Expression>> Data;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A variable access expression.
struct VarAccess : Expression
{
	Identifier Var;
	bool IsCopy = false;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

struct ArrayIndex : VarAccess
{
	up<Expression> Index;

	/// Accept a visitor.
	///
	/// \param visitor Visitor to accept.
	/// \param context Context argument to pass on to visit.
	virtual void Accept(ASTVisitor& visitor, std::any& context) override;
};

/// A visitor of the AST.
class ASTVisitor
{
public:
	/// Virtual destructor.
	virtual ~ASTVisitor() {}

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Abstract& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(ArrayIndex& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(ArrayType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Assignment& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Binary& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Block& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(BoolType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Break& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Call& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(CharType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(ClassDefinition& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(ClassType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Constructor& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Continue& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(EnumDefinition& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(ExpressionStatement& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(For& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Function& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(FunctionDefinition& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(FuncType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(GenericType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Getter& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Group& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(If& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(InitializerList& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(IntType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Literal& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Logical& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Method& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(OperatorOverload& node, std::any& context) = 0;
	
	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(RealType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Return& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Setter& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Throw& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Try& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(TupleType& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(TypeOf& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(Unary& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(VarAccess& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(VarDefinition& node, std::any& context) = 0;

	/// Visit an AST Node.
	///
	/// \param node Node to visit.
	/// \param context Parameter passed to node.Visit().
	virtual void Visit(While& node, std::any& context) = 0;
};

}
