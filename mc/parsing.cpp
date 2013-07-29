/*
 * This file is part of Mitten.
 * 
 * Mitten is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mitten is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mitten.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 Oliver Katz */

#include "parsing.h"

namespace mc
{
	parsing::Tokenizer createTokenizer()
	{
		parsing::Tokenizer t = parsing::Tokenizer();
		t.noDelim(t() == "\"", t() == "\"");
		t.noDelim(t() == "'", t() == "'");
		t.skip(t() == "//", t() == "\n" || t() == "\r");
		t.skip(t() == "#", t() == "\n" || t() == "\r");
		t.skip(t() == "/*", t() == "*/");
		t.whitespace(t() == " ");
		t.whitespace(t() == "\t");
		t.whitespace(t() == "\r");
		t.whitespace(t() == "\n");
		t.categorizer("Where", t() == "where");
		t.categorizer("InternalAssemblySymbol", t() == "asm");
		t.categorizer("ValueBool", t() == "true" || t() == "True" || t() == "TRUE" || t() == "false" || t() == "False" || t() == "FALSE");
		t.token("BoundaryBeginExpression", t() == "(");
		t.token("BoundaryEndExpression", t() == ")");
		t.token("BoundaryBeginComplex", t() == "[");
		t.token("BoundaryEndComplex", t() == "]");
		t.token("BoundaryBeginScope", t() == "{");
		t.token("BoundaryEndScope", t() == "}");
		t.token("ArgumentSeparator", t() == ",");
		t.token("OperatorAssign", t() == "=");
		t.token("OperatorAccess", t() == ".");
		t.token("OperatorAdd", t() == "+");
		t.token("OperatorSubtract", t() == "-");
		t.token("OperatorMultiply", t() == "*");
		t.token("OperatorDivide", t() == "/");
		t.token("OperatorModulate", t() == "%");
		t.token("OperatorAndBitwise", t() == "&");
		t.token("OperatorAndLogical", t() == "&&");
		t.token("OperatorOrBitwise", t() == "|");
		t.token("OperatorOrLogical", t() == "||");
		t.token("OperatorXOrBitwise", t() == "^");
		t.token("OperatorNegateBitwise", t() == "~");
		t.token("OperatorNegateLogical", t() == "!");
		t.token("OperatorBitShiftLeft", t() == "<<");
		t.token("OperatorBitShiftRight", t() == ">>");
		t.token("OperatorLessThan", t() == "<");
		t.token("OperatorLessThanOrEqualTo", t() == "<=");
		t.token("OperatorGreaterThan", t() == ">");
		t.token("OperatorGreaterThanOrEqualTo", t() == ">=");
		t.token("OperatorEqualTo", t() == "==");
		t.token("OperatorNotEqualTo", t() == "!=");
		t.token("OperatorAddAssign", t() == "+=");
		t.token("OperatorIncrement", t() == "++");
		t.token("OperatorSubtractAssign", t() == "-=");
		t.token("OperatorDecrement", t() == "--");
		t.token("OperatorMultiplyAssign", t() == "*=");
		t.token("OperatorDivideAssign", t() == "/=");
		t.token("OperatorModulateAssign", t() == "%=");
		t.token("OperatorAndBitwiseAssign", t() == "&=");
		t.token("OperatorOrBitwiseAssign", t() == "|=");
		t.token("OperatorXOrBitwiseAssign", t() == "^=");
		t.token("OperatorNegateBitwiseAssign", t() == "~=");
		t.token("OperatorBitShiftLeftAssign", t() == "<<=");
		t.token("OperatorBitShiftRightAssign", t() == ">>=");
		t.token("EndOfLine", t() == ";");
		t.token("ValueChar", t() < "'" && t() > "'");
		t.token("ValueString", t() < "\"" && t() > "\"");
		t.token("ValueInt", t() += "0123456789-");
		t.token("ValueFloat", t() += "0123456789-.");
		t.token("Symbol", t() += parsing::lettersLower+parsing::lettersUpper+parsing::digits+"_");
		t.combine(t() == "-", t() == ".");
		t.combine(t() == "-.", t() += "-.0123456789");
		t.combine(t() == "-", t() += "-.0123456789");
		t.combine(t() == ".", t() += "-.0123456789");
		t.combine(t() += "-.0123456789", t() += "-.0123456789");
		t.combine(t() == "x", t() < "\"" && t() > "\"");

		return t;
	}

	parsing::ScopeParser createScopeParser()
	{
		parsing::ScopeParser p;

		p.bindScope("BoundaryBeginScope", "BoundaryEndScope");
		p.bindScope("BoundaryBeginComplex", "BoundaryEndComplex");
		p.bindScope("BoundaryBeginExpression", "BoundaryEndExpression");

		p.bindSeparator("EndOfLine", "Line");
		p.bindSeparator("BoundaryBeginScope", "EndOfLine", "Line");
		p.bindSeparator("BoundaryBeginComplex", "ArgumentSeparator", "ComplexElement");
		p.bindSeparator("BoundaryBeginExpression", "ArgumentSeparator", "Argument");

		return p;
	}

	parsing::RDP createRDP(parsing::Tokenizer t)
	{
		parsing::RDP p;

		p.setTag("int", "Type");
		p.setTag("string", "Type");
		p.setTag("vector", "Type");

		p.setTag("import", "Symbol");
		p.setTag("print", "Symbol");
		p.setTag("return", "Symbol");

		p.setTag(t["OperatorAssign"], "OperatorBinary");

		p.addTagCondition(parsing::ASTE() << parsing::ASTE("TypeExpression") << parsing::ASTE("Symbol", "Symbol"), parsing::RDP::_end, 0, "Symbol");

		p.setPrecedence("+", 1);
		p.setPrecedence("*", 2);

		p.addElement(parsing::ASTE("TypeExpression") << parsing::ASTE("TypeSymbol", parsing::ASTE::_tag, "Type"));
		p.addElement(parsing::ASTE("TypeExpression") << parsing::ASTE("TypeSymbol", parsing::ASTE::_tag, "Type") << parsing::ASTE("TypeExpression"));
		p.addElement(parsing::ASTE("TypeExpression") << parsing::ASTE("TypeSymbol", parsing::ASTE::_tag, "Type") << parsing::ASTE("BoundaryBegin", "BoundaryBeginExpression") << parsing::ASTE("TypeExpression") << parsing::ASTE("BoundaryEnd", "BoundaryEndExpression"));

		p("OperatorUnaryLeft", p.type("OperatorUnaryLeft", "OperatorNegateBitwise"));
		p("OperatorUnaryLeft", p.type("OperatorUnaryLeft", "OperatorNegateLogical"));

		p("OperatorUnaryRight", p.type("OperatorUnaryRight", "OperatorIncrement"));
		p("OperatorUnaryRight", p.type("OperatorUnaryRight", "OperatorDecrement"));

		p("OperatorBinary", p.type("OperatorBinary", "OperatorAssign"));

		p("Operation", p.type("Value", "ValueInt") << p.type("Operator", "OperatorUnaryLeft"));
		p("Operation", p.type("Operator", "OperatorUnaryRight") << p.type("Value", "ValueInt"));
		p("Operation", p.type("Value", "ValueInt") << p.tag("OperatorBinary") << p.type("Value", "ValueInt"));

		p("Operation", p.scope("Value") << p.type("Operator", "OperatorUnaryLeft"));
		p("Operation", p.type("Operator", "OperatorUnaryRight") << p.scope("Value"));
		p("Operation", p.scope("Value") << p.tag("OperatorBinary") << p.scope("Value"));
		p("Operation", p.scope("Value") << p.tag("OperatorBinary") << p.type("Value", "ValueInt"));
		p("Operation", p.type("Value", "ValueInt") << p.tag("OperatorBinary") << p.scope("Value"));

		p("Operation", p.type("Value", "Symbol") << p.type("Operator", "OperatorUnaryLeft"));
		p("Operation", p.type("Operator", "OperatorUnaryRight") << p.type("Value", "Symbol"));
		p("Operation", p.type("Value", "ValueInt") << p.tag("OperatorBinary") << p.type("Value", "Symbol"));
		p("Operation", p.type("Value", "Symbol") << p.tag("OperatorBinary") << p.type("Value", "ValueInt"));
		p("Operation", p.type("Value", "Symbol") << p.tag("OperatorBinary") << p.type("Value", "Symbol"));

		p.addElement(parsing::ASTE("ComplexValue") << parsing::ASTE("BoundaryBegin", "BoundaryBeginComplex") << parsing::ASTE("Content", parsing::ASTE::_scope) << parsing::ASTE("BoundaryEnd", "BoundaryEndComplex"));

		p.addElement(parsing::ASTE("ArgumentVector") << parsing::ASTE("BoundaryBegin", "BoundaryBeginExpression") << parsing::ASTE("Content", parsing::ASTE::_scope) << parsing::ASTE("BoundaryEnd", "BoundaryEndExpression"));

		p.addElement(parsing::ASTE("Body") << parsing::ASTE("BoundaryBegin", "BoundaryBeginScope") << parsing::ASTE("Content", parsing::ASTE::_scope) << parsing::ASTE("BoundaryEnd", "BoundaryEndScope"));

		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("Symbol", parsing::ASTE::_tag, "Symbol") << parsing::ASTE("ComplexValue"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("Symbol", parsing::ASTE::_tag, "Symbol") << parsing::ASTE("ArgumentVector"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("Symbol", parsing::ASTE::_tag, "Symbol") << parsing::ASTE("Body"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("Symbol", parsing::ASTE::_tag, "Symbol") << parsing::ASTE("ComplexValue") << parsing::ASTE("ArgumentVector"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("Symbol", parsing::ASTE::_tag, "Symbol") << parsing::ASTE("ArgumentVector") << parsing::ASTE("Body"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("Symbol", parsing::ASTE::_tag, "Symbol") << parsing::ASTE("ComplexValue") << parsing::ASTE("ArgumentVector") << parsing::ASTE("Body"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("TypeExpression") << parsing::ASTE("Symbol", "Symbol") << parsing::ASTE("ComplexValue"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("TypeExpression") << parsing::ASTE("Symbol", "Symbol") << parsing::ASTE("ArgumentVector"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("TypeExpression") << parsing::ASTE("Symbol", "Symbol") << parsing::ASTE("Body"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("TypeExpression") << parsing::ASTE("Symbol", "Symbol") << parsing::ASTE("ComplexValue") << parsing::ASTE("ArgumentVector"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("TypeExpression") << parsing::ASTE("Symbol", "Symbol") << parsing::ASTE("ArgumentVector") << parsing::ASTE("Body"));
		p.addElement(parsing::ASTE("Expression") << parsing::ASTE("TypeExpression") << parsing::ASTE("Symbol", "Symbol") << parsing::ASTE("ComplexValue") << parsing::ASTE("ArgumentVector") << parsing::ASTE("Body"));

		return p;
	}
}
