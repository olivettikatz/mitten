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
	Tokenizer createTokenizer()
	{
		Tokenizer t = Tokenizer();
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
		/*t.token("OperatorAssign", t() == "=");
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
		t.token("OperatorBitShiftRightAssign", t() == ">>=");*/
		t.token("OperatorBinary", t() == "=");
		t.token("OperatorBinary", t() == ".");
		t.token("OperatorBinary", t() == "+");
		t.token("OperatorBinary", t() == "-");
		t.token("OperatorBinary", t() == "*");
		t.token("OperatorBinary", t() == "/");
		t.token("OperatorBinary", t() == "%");
		t.token("OperatorBinary", t() == "&");
		t.token("OperatorBinary", t() == "&&");
		t.token("OperatorBinary", t() == "|");
		t.token("OperatorBinary", t() == "||");
		t.token("OperatorBinary", t() == "^");
		t.token("OperatorUnaryLeft", t() == "~");
		t.token("OperatorUnaryLeft", t() == "!");
		t.token("OperatorBinary", t() == "<<");
		t.token("OperatorBinary", t() == ">>");
		t.token("OperatorBinary", t() == "<");
		t.token("OperatorBinary", t() == "<=");
		t.token("OperatorBinary", t() == ">");
		t.token("OperatorBinary", t() == ">=");
		t.token("OperatorBinary", t() == "==");
		t.token("OperatorBinary", t() == "!=");
		t.token("OperatorBinary", t() == "+=");
		t.token("OperatorUnaryRight", t() == "++");
		t.token("OperatorBinary", t() == "-=");
		t.token("OperatorUnaryRight", t() == "--");
		t.token("OperatorBinary", t() == "*=");
		t.token("OperatorBinary", t() == "/=");
		t.token("OperatorBinary", t() == "%=");
		t.token("OperatorBinary", t() == "&=");
		t.token("OperatorBinary", t() == "|=");
		t.token("OperatorBinary", t() == "^=");
		t.token("OperatorBinary", t() == "~=");
		t.token("OperatorBinary", t() == "<<=");
		t.token("OperatorBinary", t() == ">>=");
		t.token("TypeExpressionPrefix", t() == "@");
		t.token("EndOfLine", t() == ";");
		t.token("ValueChar", t() < "'" && t() > "'");
		t.token("ValueString", t() < "\"" && t() > "\"");
		t.token("ValueInt", t() += "0123456789-");
		t.token("ValueFloat", t() += "0123456789-.");
		t.token("Symbol", t() += lettersLower+lettersUpper+digits+"_");
		t.combine(t() == "-", t() == ".");
		t.combine(t() == "-.", t() += "-.0123456789");
		t.combine(t() == "-", t() += "-.0123456789");
		t.combine(t() == ".", t() += "-.0123456789");
		t.combine(t() += "-.0123456789", t() += "-.0123456789");
		t.combine(t() == "x", t() < "\"" && t() > "\"");

		return t;
	}

	ScopeParser createScopeParser()
	{
		ScopeParser p;

		p.bindScope("BoundaryBeginScope", "BoundaryEndScope");
		p.bindScope("BoundaryBeginComplex", "BoundaryEndComplex");
		p.bindScope("BoundaryBeginExpression", "BoundaryEndExpression");

		return p;
	}

	RDP createRDP()
	{
		RDP p;

		p.setPrecedence("+", 1);
		p.setPrecedence("*", 2);

		p.addElement(ASTE("TypeExpression") << ASTE("Prefix", "TypeExpressionPrefix") << ASTE("Name", "Symbol") << ASTE("BoundaryBegin", "BoundaryBeginExpression") << ASTE("Arguments", ASTE::_scope) << ASTE("BoundaryEnd", "BoundaryEndExpression"));

		p.addElement(ASTE("Operation") << ASTE("Value", "ValueInt") << ASTE("Operator", "OperatorUnaryLeft"));
		p.addElement(ASTE("Operation") << ASTE("Operator", "OperatorUnaryRight") << ASTE("Value", "ValueInt"));
		p.addElement(ASTE("Operation") << ASTE("Value", "ValueInt") << ASTE("Operator", "OperatorBinary") << ASTE("Value", "ValueInt"));

		p.addElement(ASTE("Operation") << ASTE("Value", ASTE::_scope) << ASTE("Operator", "OperatorUnaryLeft"));
		p.addElement(ASTE("Operation") << ASTE("Operator", "OperatorUnaryRight") << ASTE("Value", ASTE::_scope));
		p.addElement(ASTE("Operation") << ASTE("Value", ASTE::_scope) << ASTE("Operator", "OperatorBinary") << ASTE("Value", ASTE::_scope));
		p.addElement(ASTE("Operation") << ASTE("Value", ASTE::_scope) << ASTE("Operator", "OperatorBinary") << ASTE("Value", "ValueInt"));
		p.addElement(ASTE("Operation") << ASTE("Value", "ValueInt") << ASTE("Operator", "OperatorBinary") << ASTE("Value", ASTE::_scope));

		return p;
	}
}
