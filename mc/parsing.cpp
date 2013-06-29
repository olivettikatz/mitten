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
		t.token("TypeExpressionPrefix", t() == "@");
		t.token("EndOfLine", t() == ";");
		t.token("ValueBool", t() == "true" || t() == "True" || t() == "TRUE" || t() == "false" || t() == "False" || t() == "FALSE");
		t.token("ValueChar", t() < "'" && t() > "'");
		t.token("ValueString", t() < "\"" && t() > "\"");
		t.token("ValueInt", t() += "0123456789-");
		t.token("ValueFloat", t() += "0123456789-.");
		t.token("TypeSymbol", t() == "int" || t() == "vector" || t() == "string");
		t.token("Symbol", t() += lettersLower+lettersUpper+digits+"_");
		t.combine(t() == "-", t() == ".");
		t.combine(t() == "-.", t() += "-.0123456789");
		t.combine(t() == "-", t() += "-.0123456789");
		t.combine(t() == ".", t() += "-.0123456789");
		t.combine(t() += "-.0123456789", t() += "-.0123456789");
		t.combine(t() == "x", t() < "\"" && t() > "\"");

		return t;
	}

	Grammar createParser(Tokenizer &t)
	{
		Grammar g = Grammar();
		Builder b = Builder();
		b.createDefaultParsers();

		g.createFromTokenizer(b, "One", t);
		g["Symbol"].enableKeep();
		g["ValueInt"].enableKeep();
		g["OperatorAssign"].enableKeep();
		g["OperatorAccess"].enableKeep();
		g["OperatorAdd"].enableKeep();
		g["OperatorSubtract"].enableKeep();
		g["OperatorMultiply"].enableKeep();
		g["OperatorDivide"].enableKeep();
		g["OperatorModulate"].enableKeep();
		g["OperatorAndBitwise"].enableKeep();
		g["OperatorAndLogical"].enableKeep();
		g["OperatorOrBitwise"].enableKeep();
		g["OperatorOrLogical"].enableKeep();
		g["OperatorXOrBitwise"].enableKeep();
		g["OperatorNegateBitwise"].enableKeep();
		g["OperatorNegateBitwise"].enableKeep();
		g["OperatorBitShiftLeft"].enableKeep();
		g["OperatorBitShiftRight"].enableKeep();
		g["OperatorLessThan"].enableKeep();
		g["OperatorLessThanOrEqualTo"].enableKeep();
		g["OperatorGreaterThan"].enableKeep();
		g["OperatorGreaterThanOrEqualTo"].enableKeep();
		g["OperatorEqualTo"].enableKeep();
		g["OperatorNotEqualTo"].enableKeep();
		g["OperatorAddAssign"].enableKeep();
		g["OperatorIncrement"].enableKeep();
		g["OperatorSubtractAssign"].enableKeep();
		g["OperatorDecrement"].enableKeep();
		g["OperatorMultiplyAssign"].enableKeep();
		g["OperatorDivideAssign"].enableKeep();
		g["OperatorModulateAssign"].enableKeep();
		g["OperatorAndBitwiseAssign"].enableKeep();
		g["OperatorOrBitwiseAssign"].enableKeep();
		g["OperatorXOrBitwiseAssign"].enableKeep();
		g["OperatorNegateBitwiseAssign"].enableKeep();
		g["OperatorBitShiftLeftAssign"].enableKeep();
		g["OperatorBitShiftRightAssign"].enableKeep();

		g << b("Parallel", "SymbolOrValueInt");
		g["SymbolOrValueInt"].append(g["Symbol"]);
		g["SymbolOrValueInt"].append(g["ValueInt"]);

		g << b("Many", "InternalAssemblyArgs");
		g["InternalAssemblyArgs"].append(g["SymbolOrValueInt"]);

		g << b("Sequence", "OneInternalAssemblyLine");
		g["OneInternalAssemblyLine"].append(g["Symbol"]);
		g["OneInternalAssemblyLine"].append(g["InternalAssemblyArgs"]);
		g["OneInternalAssemblyLine"].append(g["EndOfLine"]);

		g << b("Many", "InternalAssemblyLines");
		g["InternalAssemblyLines"].append(g["OneInternalAssemblyLine"]);

		g << b("Sequence", "InternalAssemblyExpression");
		g["InternalAssemblyExpression"].append(g["InternalAssemblySymbol"]);
		g["InternalAssemblyExpression"].append(g["BoundaryBeginScope"]);
		g["InternalAssemblyExpression"].append(g["InternalAssemblyLines"]);
		g["InternalAssemblyExpression"].append(g["BoundaryEndScope"]);

		g << b("Many", "TypeQualifiers");
		g["TypeQualifiers"].append(g["TypeSymbol"]);
		
		g << b("Parallel", "TypeExpression");
		g["TypeExpression"].assumeSize(1);

		g << b("Sequence", "TypeExpressionArguments");
		g["TypeExpressionArguments"].assumeSize(5);
		g["TypeExpressionArguments"].append(g["TypeExpressionPrefix"]);
		g["TypeExpressionArguments"].append(g["TypeQualifiers"]);
		g["TypeExpressionArguments"].append(g["BoundaryBeginExpression"]);
		g["TypeExpressionArguments"].append(g["TypeExpression"]);
		g["TypeExpressionArguments"].append(g["BoundaryEndExpression"]);

		g << b("Sequence", "TypeExpressionNoArguments");
		g["TypeExpressionNoArguments"].assumeSize(1);
		g["TypeExpressionNoArguments"].append(g["TypeQualifiers"]);

		g["TypeExpression"].append(g["TypeExpressionArguments"]);
		g["TypeExpression"].append(g["TypeExpressionNoArguments"]);

		g << b("Parallel", "Any");
		g["Any"].assumeSize(0);

		g << b("Sequence", "OneComplexValueElement");
		g["OneComplexValueElement"].append(g["Any"]);
		g["OneComplexValueElement"].append(g["ArgumentSeparator"]);

		g << b("Many", "ComplexValueElements");
		g["ComplexValueElements"].append(g["OneComplexValueElement"]);

		g << b("Sequence", "ComplexValue");
		g["ComplexValue"].append(g["BoundaryBeginComplex"]);
		g["ComplexValue"].append(g["ComplexValueElements"]);
		g["ComplexValue"].append(g["OneComplexValueElement"]);
		g["ComplexValue"].append(g["BoundaryEndComplex"]);

		g << b("Parallel", "OperatorUnaryLeft");
		g["OperatorUnaryLeft"].append(g["OperatorNegateBitwise"]);
		g["OperatorUnaryLeft"].append(g["OperatorNegateLogical"]);

		g << b("Parallel", "OperatorUnaryRight");
		g["OperatorUnaryRight"].append(g["OperatorIncrement"]);
		g["OperatorUnaryRight"].append(g["OperatorDecrement"]);

		g << b("Parallel", "OperatorBinary");
		g["OperatorBinary"].append(g["OperatorAssign"]);
		g["OperatorBinary"].append(g["OperatorAccess"]);
		g["OperatorBinary"].append(g["OperatorAdd"]);
		g["OperatorBinary"].append(g["OperatorSubtract"]);
		g["OperatorBinary"].append(g["OperatorMultiply"]);
		g["OperatorBinary"].append(g["OperatorDivide"]);
		g["OperatorBinary"].append(g["OperatorModulate"]);
		g["OperatorBinary"].append(g["OperatorAndBitwise"]);
		g["OperatorBinary"].append(g["OperatorAndLogical"]);
		g["OperatorBinary"].append(g["OperatorOrBitwise"]);
		g["OperatorBinary"].append(g["OperatorOrLogical"]);
		g["OperatorBinary"].append(g["OperatorXOrBitwise"]);
		g["OperatorBinary"].append(g["OperatorBitShiftLeft"]);
		g["OperatorBinary"].append(g["OperatorBitShiftRight"]);
		g["OperatorBinary"].append(g["OperatorLessThan"]);
		g["OperatorBinary"].append(g["OperatorLessThanOrEqualTo"]);
		g["OperatorBinary"].append(g["OperatorGreaterThan"]);
		g["OperatorBinary"].append(g["OperatorGreaterThanOrEqualTo"]);
		g["OperatorBinary"].append(g["OperatorEqualTo"]);
		g["OperatorBinary"].append(g["OperatorNotEqualTo"]);
		g["OperatorBinary"].append(g["OperatorAddAssign"]);
		g["OperatorBinary"].append(g["OperatorSubtractAssign"]);
		g["OperatorBinary"].append(g["OperatorMultiplyAssign"]);
		g["OperatorBinary"].append(g["OperatorDivideAssign"]);
		g["OperatorBinary"].append(g["OperatorModulateAssign"]);
		g["OperatorBinary"].append(g["OperatorAndBitwiseAssign"]);
		g["OperatorBinary"].append(g["OperatorOrBitwiseAssign"]);
		g["OperatorBinary"].append(g["OperatorXOrBitwiseAssign"]);
		g["OperatorBinary"].append(g["OperatorNegateBitwiseAssign"]);
		g["OperatorBinary"].append(g["OperatorBitShiftLeftAssign"]);
		g["OperatorBinary"].append(g["OperatorBitShiftRightAssign"]);

		g << b("Sequence", "OperationUnaryLeft");
		g["OperationUnaryLeft"].append(g["OperatorUnaryLeft"]);
		g["OperationUnaryLeft"].append(g["Any"]);

		g << b("Sequence", "OperationUnaryRight");
		g["OperationUnaryRight"].append(g["Any"]);
		g["OperationUnaryRight"].append(g["OperatorUnaryRight"]);

		g << b("Sequence", "OperationBinary");
		g["OperationBinary"].append(g["Any"]);
		g["OperationBinary"].append(g["OperatorBinary"]);
		g["OperationBinary"].append(g["Any"]);

		g << b("Parallel", "AnyUnboundedOperation");
		g["AnyUnboundedOperation"].append(g["OperationUnaryLeft"]);
		g["AnyUnboundedOperation"].append(g["OperationUnaryRight"]);
		g["AnyUnboundedOperation"].append(g["OperationBinary"]);

		g << b("Parallel", "Operation");
		g["Operation"].assumeSize(2);
		g["Operation"].append(g["AnyUnboundedOperation"]);

		g << b("Sequence", "AnyBoundedOperation");
		g["AnyBoundedOperation"].append(g["BoundaryBeginExpression"]);
		g["AnyBoundedOperation"].append(g["Operation"]);
		g["AnyBoundedOperation"].append(g["BoundaryEndExpression"]);
		g["Operation"].append(g["AnyBoundedOperation"]);

		g << b("Sequence", "OneArgumentVectorElementSeparator");
		g["OneArgumentVectorElementSeparator"].append(g["Any"]);
		g["OneArgumentVectorElementSeparator"].append(g["ArgumentSeparator"]);

		g << b("Parallel", "OneArgumentVectorElement");
		g["OneArgumentVectorElement"].append(g["OneArgumentVectorElementSeparator"]);
		g["OneArgumentVectorElement"].append(g["Any"]);

		g << b("Many", "ArgumentVectorElements");
		g["ArgumentVectorElements"].append(g["OneArgumentVectorElement"]);

		g << b("Sequence", "ArgumentVectorNonNull");
		g["ArgumentVectorNonNull"].append(g["ArgumentVectorElements"]);
		g["ArgumentVectorNonNull"].append(g["Any"]);

		g << b("Sequence", "ArgumentVectorNull");

		g << b("Parallel", "ArgumentVectorContents");
		g["ArgumentVectorContents"].append(g["ArgumentVectorNonNull"]);
		g["ArgumentVectorContents"].append(g["ArgumentVectorNull"]);

		g << b("Sequence", "ArgumentVector");
		g["ArgumentVector"].append(g["BoundaryBeginScope"]);
		g["ArgumentVector"].append(g["ArgumentVectorContents"]);
		g["ArgumentVector"].append(g["BoundaryBeginScope"]);

		g << b("Sequence", "OneLine");
		g["OneLine"].append(g["Any"]);
		g["OneLine"].append(g["EndOfLine"]);

		g << b("Many", "Lines");
		g["Lines"].append(g["OneLine"]);

		g << b("Sequence", "Body");
		g["Body"].append(g["BoundaryBeginScope"]);
		g["Body"].append(g["Lines"]);
		g["Body"].append(g["BoundaryEndScope"]);

		g << b("Sequence", "Where");
		g["Where"].append(g["Where"]);
		g["Where"].append(g["Body"]);

		g << b("Sequence", "ExpressionSymbolComplex");
		g["ExpressionSymbolComplex"].append(g["Symbol"]);
		g["ExpressionSymbolComplex"].append(g["ComplexValue"]);

		g << b("Sequence", "ExpressionSymbolArgument");
		g["ExpressionSymbolArgument"].append(g["Symbol"]);
		g["ExpressionSymbolArgument"].append(g["ArgumentVector"]);

		g << b("Sequence", "ExpressionSymbolBody");
		g["ExpressionSymbolBody"].append(g["Symbol"]);
		g["ExpressionSymbolBody"].append(g["Body"]);

		g << b("Sequence", "ExpressionSymbolBodyWhere");
		g["ExpressionSymbolBodyWhere"].append(g["Symbol"]);
		g["ExpressionSymbolBodyWhere"].append(g["Body"]);
		g["ExpressionSymbolBodyWhere"].append(g["Where"]);

		g << b("Sequence", "ExpressionSymbolComplexArgument");
		g["ExpressionSymbolComplexArgument"].append(g["Symbol"]);
		g["ExpressionSymbolComplexArgument"].append(g["ComplexValue"]);
		g["ExpressionSymbolComplexArgument"].append(g["ArgumentVector"]);

		g << b("Sequence", "ExpressionSymbolArgumentBody");
		g["ExpressionSymbolArgumentBody"].append(g["Symbol"]);
		g["ExpressionSymbolArgumentBody"].append(g["ArgumentVector"]);
		g["ExpressionSymbolArgumentBody"].append(g["Body"]);

		g << b("Sequence", "ExpressionSymbolArgumentBodyWhere");
		g["ExpressionSymbolArgumentBodyWhere"].append(g["Symbol"]);
		g["ExpressionSymbolArgumentBodyWhere"].append(g["ArgumentVector"]);
		g["ExpressionSymbolArgumentBodyWhere"].append(g["Body"]);
		g["ExpressionSymbolArgumentBodyWhere"].append(g["Where"]);

		g << b("Sequence", "ExpressionSymbolComplexArgumentBody");
		g["ExpressionSymbolComplexArgumentBody"].append(g["Symbol"]);
		g["ExpressionSymbolComplexArgumentBody"].append(g["ComplexValue"]);
		g["ExpressionSymbolComplexArgumentBody"].append(g["ArgumentVector"]);
		g["ExpressionSymbolComplexArgumentBody"].append(g["Body"]);

		g << b("Sequence", "ExpressionSymbolComplexArgumentBodyWhere");
		g["ExpressionSymbolComplexArgumentBodyWhere"].append(g["Symbol"]);
		g["ExpressionSymbolComplexArgumentBodyWhere"].append(g["ComplexValue"]);
		g["ExpressionSymbolComplexArgumentBodyWhere"].append(g["ArgumentVector"]);
		g["ExpressionSymbolComplexArgumentBodyWhere"].append(g["Body"]);
		g["ExpressionSymbolComplexArgumentBodyWhere"].append(g["Where"]);

		g << b("Sequence", "ExpressionTypeSymbolComplex");
		g["ExpressionTypeSymbolComplex"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolComplex"].append(g["Symbol"]);
		g["ExpressionTypeSymbolComplex"].append(g["ComplexValue"]);

		g << b("Sequence", "ExpressionTypeSymbolArgument");
		g["ExpressionTypeSymbolArgument"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolArgument"].append(g["Symbol"]);
		g["ExpressionTypeSymbolArgument"].append(g["ArgumentVector"]);

		g << b("Sequence", "ExpressionTypeSymbolBody");
		g["ExpressionTypeSymbolBody"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolBody"].append(g["Symbol"]);
		g["ExpressionTypeSymbolBody"].append(g["Body"]);

		g << b("Sequence", "ExpressionTypeSymbolBodyWhere");
		g["ExpressionTypeSymbolBodyWhere"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolBodyWhere"].append(g["Symbol"]);
		g["ExpressionTypeSymbolBodyWhere"].append(g["Body"]);
		g["ExpressionTypeSymbolBodyWhere"].append(g["Where"]);

		g << b("Sequence", "ExpressionTypeSymbolComplexArgument");
		g["ExpressionTypeSymbolComplexArgument"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolComplexArgument"].append(g["Symbol"]);
		g["ExpressionTypeSymbolComplexArgument"].append(g["ComplexValue"]);
		g["ExpressionTypeSymbolComplexArgument"].append(g["ArgumentVector"]);

		g << b("Sequence", "ExpressionTypeSymbolArgumentBody");
		g["ExpressionTypeSymbolArgumentBody"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolArgumentBody"].append(g["Symbol"]);
		g["ExpressionTypeSymbolArgumentBody"].append(g["ArgumentVector"]);
		g["ExpressionTypeSymbolArgumentBody"].append(g["Body"]);

		g << b("Sequence", "ExpressionTypeSymbolArgumentBodyWhere");
		g["ExpressionTypeSymbolArgumentBodyWhere"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolArgumentBodyWhere"].append(g["Symbol"]);
		g["ExpressionTypeSymbolArgumentBodyWhere"].append(g["ArgumentVector"]);
		g["ExpressionTypeSymbolArgumentBodyWhere"].append(g["Body"]);
		g["ExpressionTypeSymbolArgumentBodyWhere"].append(g["Where"]);

		g << b("Sequence", "ExpressionTypeSymbolComplexArgumentBody");
		g["ExpressionTypeSymbolComplexArgumentBody"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolComplexArgumentBody"].append(g["Symbol"]);
		g["ExpressionTypeSymbolComplexArgumentBody"].append(g["ComplexValue"]);
		g["ExpressionTypeSymbolComplexArgumentBody"].append(g["ArgumentVector"]);
		g["ExpressionTypeSymbolComplexArgumentBody"].append(g["Body"]);

		g << b("Sequence", "ExpressionTypeSymbolComplexArgumentBodyWhere");
		g["ExpressionTypeSymbolComplexArgumentBodyWhere"].append(g["TypeExpression"]);
		g["ExpressionTypeSymbolComplexArgumentBodyWhere"].append(g["Symbol"]);
		g["ExpressionTypeSymbolComplexArgumentBodyWhere"].append(g["ComplexValue"]);
		g["ExpressionTypeSymbolComplexArgumentBodyWhere"].append(g["ArgumentVector"]);
		g["ExpressionTypeSymbolComplexArgumentBodyWhere"].append(g["Body"]);
		g["ExpressionTypeSymbolComplexArgumentBodyWhere"].append(g["Where"]);

		g << b("Parallel", "Expression");
		g["Expression"].append(g["ExpressionSymbolComplex"]);
		g["Expression"].append(g["ExpressionSymbolArgument"]);
		g["Expression"].append(g["ExpressionSymbolBody"]);
		g["Expression"].append(g["ExpressionSymbolBodyWhere"]);
		g["Expression"].append(g["ExpressionSymbolComplexArgument"]);
		g["Expression"].append(g["ExpressionSymbolArgumentBody"]);
		g["Expression"].append(g["ExpressionSymbolArgumentBodyWhere"]);
		g["Expression"].append(g["ExpressionSymbolComplexArgumentBody"]);
		g["Expression"].append(g["ExpressionSymbolComplexArgumentBodyWhere"]);
		g["Expression"].append(g["ExpressionTypeSymbolComplex"]);
		g["Expression"].append(g["ExpressionTypeSymbolArgument"]);
		g["Expression"].append(g["ExpressionTypeSymbolBody"]);
		g["Expression"].append(g["ExpressionTypeSymbolBodyWhere"]);
		g["Expression"].append(g["ExpressionTypeSymbolComplexArgument"]);
		g["Expression"].append(g["ExpressionTypeSymbolArgumentBody"]);
		g["Expression"].append(g["ExpressionTypeSymbolArgumentBodyWhere"]);
		g["Expression"].append(g["ExpressionTypeSymbolComplexArgumentBody"]);
		g["Expression"].append(g["ExpressionTypeSymbolComplexArgumentBodyWhere"]);

		g["Any"].append(g["InternalAssemblyExpression"]);
		g["Any"].append(g["TypeExpression"]);
		g["Any"].append(g["ComplexValue"]);
		g["Any"].append(g["Operation"]);
		g["Any"].append(g["Expression"]);

		g << b("Many", "Page");
		g["Page"].append(g["Any"]);

		return g;
	}
}
