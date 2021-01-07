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

#include "Parser/AST.h"

namespace Wave {

void FuncType::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void ClassType::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Abstract::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void ClassDefinition::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Block::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Constructor::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Getter::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Setter::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Break::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Continue::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Return::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void VarDefinition::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void ExpressionStatement::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void While::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void ConditionFor::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void RangeFor::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void If::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Function::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void FunctionDefinition::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Method::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Assignment::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Logical::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Binary::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Unary::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Call::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Literal::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Group::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void VarAccess::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Try::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void TypeOf::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void Throw::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void ArrayType::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void ArrayIndex::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void EnumDefinition::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void InitializerList::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void TupleType::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void SimpleType::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

void OperatorOverload::Accept(ASTVisitor& visitor, std::any& context)
{
	visitor.Visit(*this, context);
}

}
