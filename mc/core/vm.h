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

#ifndef __CORE_VM_H
#define __CORE_VM_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

/*#ifdef _MITTEN_PLATFORM_DARWIN
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/JIT.h>
#else*/
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/JIT.h>
//#endif

#define VM_WARN cout<<"[Mitten VM] "

namespace vm
{
	using namespace std;
	using namespace llvm;

	class VM;

	/*! for storing basic type information. */
	class LiteralType
	{
	private:
		int val;
		int elem;

	public:
		const static int _uninitialized = -1;
		const static int _char = 0;
		const static int _int = 1;
		const static int _double = 2;
		const static int _addr = 3;

		/*! initializes empty type (will return NULL LLVM Type). */
		LiteralType();

		/*! initializes type with type code `v'. */
		LiteralType(int v);

		/*! initializes type with type code `v' and element type `e'. */
		LiteralType(int v, int e);

		/*! checks if the type is initialized. */
		bool initialized();

		/*! gets the type code. */
		int get();

		/*! gets the element type code. */
		int getElement();

		/*! generates the LLVM Type. */
		Type *gen(VM &vm);

		string display(string pref);

		/*! displays a formatted string. */
		string display();
	};

	/*! for literal character values. */
	class LiteralChar
	{
	private:
		bool init;
		char val;

	public:
		LiteralChar();
		LiteralChar(char v);
		bool initialized();
		char get();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for literal integer values. */
	class LiteralInt
	{
	private:
		bool init;
		int val;

	public:
		LiteralInt();
		LiteralInt(int v);
		bool initialized();
		int get();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for literal double values. */
	class LiteralDouble
	{
	private:
		bool init;
		double val;

	public:
		LiteralDouble();
		LiteralDouble(double v);
		bool initialized();
		double get();																																																																																																																																																																																																																				
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for literal address values. */
	class LiteralAddress
	{
	private:
		bool init;
		LiteralType elem;
		void *val;

	public:
		LiteralAddress();
		LiteralAddress(void *v);
		LiteralAddress(LiteralType e, void *v);
		bool initialized();
		LiteralType getElementType();
		void *get();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for literal values of any type. */
	class LiteralValue
	{
	private:
		shared_ptr<LiteralType> type;
		shared_ptr<LiteralChar> _char;
		shared_ptr<LiteralInt> _int;
		shared_ptr<LiteralDouble> _double;
		shared_ptr<LiteralAddress> _addr;
		string name;

	public:
		LiteralValue();
		LiteralValue(LiteralChar v);
		LiteralValue(LiteralInt v);
		LiteralValue(LiteralDouble v);
		LiteralValue(LiteralAddress v);
		LiteralValue(string v);
		bool initialized();
		bool isNamedVariable();
		string getName();
		LiteralType getType();
		LiteralChar getChar();
		LiteralInt getInt();
		LiteralDouble getDouble();
		LiteralAddress getAddress();
		LiteralChar castChar();
		LiteralInt castInt();
		LiteralDouble castDouble();
		LiteralAddress castAddress();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	class MutableDefinition;
	class MutableAssignment;
	class Operation;
	class Call;
	class IfStatement;
	class ForLoop;

	/*! for functional programming nodes. */
	class FunctionalNode
	{
	public:
		const static int _uninitialized = -1;
		const static int _mut_definition = 0;
		const static int _mut_assign = 1;
		const static int _value = 2;
		const static int _arith = 3;
		const static int _call = 4;
		const static int _if = 5;
		const static int _forl = 6;

	private:
		int type;

		shared_ptr<MutableDefinition> md;
		shared_ptr<MutableAssignment> ma;
		shared_ptr<LiteralValue> val;
		shared_ptr<Operation> arith;
		shared_ptr<Call> call;
		shared_ptr<IfStatement> ifs;
		shared_ptr<ForLoop> forl;

	public:
		FunctionalNode();
		FunctionalNode(MutableDefinition d);
		FunctionalNode(MutableAssignment a);
		FunctionalNode(LiteralValue v);
		FunctionalNode(Operation o);
		FunctionalNode(Call c);
		FunctionalNode(IfStatement i);
		FunctionalNode(ForLoop f);
		bool initialized();
		int getType();
		string getTypeString();
		MutableDefinition getMutableDefinition();
		MutableAssignment getMutableAssignment();
		LiteralValue getValue();
		Operation getOperation();
		Call getCall();
		IfStatement getIfStatement();
		ForLoop getForLoop();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for definitions of mutable objects. */
	class MutableDefinition
	{
	private:
		LiteralType type;
		string name;
		FunctionalNode init;

	public:
		MutableDefinition();
		MutableDefinition(LiteralType t, string n);
		MutableDefinition(LiteralType t, string n, FunctionalNode v);
		bool initialized();
		LiteralType getType();
		string getName();
		FunctionalNode getValue();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for assignments of mutable objects. */
	class MutableAssignment
	{
	private:
		string name;
		FunctionalNode val;

	public:
		MutableAssignment();
		MutableAssignment(string n, FunctionalNode v);
		bool initialized();
		string getName();
		FunctionalNode getValue();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for operations between functional nodes. */
	class Operation
	{
	private:
		int op;
		FunctionalNode left;
		FunctionalNode right;

	public:
		const static int uninitialized = -1;
		const static int iadd = 0;
		const static int fadd = 1;
		const static int isub = 2;
		const static int fsub = 3;
		const static int imul = 4;
		const static int fmul = 5;
		const static int idiv = 6;
		const static int fdiv = 7;
		const static int iand = 8;
		const static int ior = 9;
		const static int ixor = 10;
		const static int ilt = 11;
		const static int flt = 12;
		const static int igt = 13;
		const static int fgt = 14;
		const static int ile = 15;
		const static int fle = 16;
		const static int ige = 17;
		const static int fge = 18;
		const static int ieq = 19;
		const static int feq = 20;
		const static int ine = 21;
		const static int fne = 22;
		const static int ineg = 23;
		const static int casti8 = 24;
		const static int casti32 = 25;
		const static int castf32 = 26;
		const static int castxi8 = 27;
		const static int castxi32 = 28;
		const static int castxf32 = 29;
		const static int derefxi8 = 30;
		const static int derefxi32 = 31;
		const static int derefxf32 = 32;
		const static int xaddr = 33;

		Operation();
		Operation(FunctionalNode l, int o, FunctionalNode r);
		Operation(int o, FunctionalNode r);
		bool initialized();
		int getOp();
		FunctionalNode getLeft();
		FunctionalNode getRight();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for function calls. */
	class Call
	{
	private:
		string func;
		vector<FunctionalNode> args;

	public:
		Call();
		Call(string f, vector<FunctionalNode> a);
		bool initialized();
		string getName();
		Value *gen(VM &vm);
		string display();
		string display(string pref);
	};

	/*! for if statements. */
	class IfStatement
	{
	private:
		bool init;
		FunctionalNode cond;
		FunctionalNode then;
		FunctionalNode other;

	public:
		IfStatement();
		IfStatement(FunctionalNode c, FunctionalNode t, FunctionalNode e);
		bool initialized();
		FunctionalNode getCondition();
		FunctionalNode getThen();
		FunctionalNode getElse();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for for loops. */
	class ForLoop
	{
	private:
		string var;
		FunctionalNode init;
		FunctionalNode end;
		FunctionalNode step;
		FunctionalNode body;

	public:
		ForLoop();
		ForLoop(string v, FunctionalNode i, FunctionalNode e, FunctionalNode s, FunctionalNode b);
		bool initialized();
		string getVariable();
		FunctionalNode getInitial();
		FunctionalNode getFinal();
		FunctionalNode getStep();
		FunctionalNode getBody();
		Value *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for function prototypes. */
	class Prototype
	{
	private:
		LiteralType rtn;
		vector<pair<LiteralType, string> > args;
		string name;

	public:
		Prototype();
		Prototype(string n, LiteralType r, vector<pair<LiteralType, string> > a);
		bool initialized();
		unsigned int size();
		LiteralType getReturnType();
		string getName();
		string getArgumentName(unsigned int idx);
		LiteralType getArgumentType(unsigned int idx);
		Function *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for function bodies. */
	class Body
	{
	private:
		Prototype ptype;
		vector<FunctionalNode> nodes;

	public:
		Body();
		Body(Prototype p);
		bool initialized();
		Prototype getPrototype();
		int appendFunctionalNode(FunctionalNode n);
		Function *gen(VM &vm);
		string display(string pref);
		string display();
	};

	/*! for storing all VM data. */
	class VM
	{
	public:
		shared_ptr<Module> module;
		shared_ptr<IRBuilder<> > builder;
		shared_ptr<FunctionPassManager> fpm;
		ExecutionEngine *ee;
		map<string, AllocaInst *> variables; 
		map<string, Function *> funcs;

		AllocaInst *createEntryBlockAlloca(Function *f, string n, Type *t);
		Value *loadMutable(string n);
		Value *storeMutable(string n, Value *v);

		VM();
		~VM();
		int setOptimizationLevel(int l);
		int linkExternal(Prototype p, void *f);
		void *JIT(Body b, bool d);
		void *JIT(Body b); 
		vector<string> listJITedFunctions();
		int unJIT(string n);
	};
}

/*
Here's an example of how to make a function to increment stuff with the -_- VM and call it from with in the C++ parent program.
{{VM vm = VM();
vm.setOptimizationLevel(1); // this is required to initialize the opimizer.
// if you don't want optimization pass a level of 0.

// let's create the argument of the function for the prototype
pair&lt;LiteralType, string&gt; arg;
arg.first = LiteralType(LiteralType::_int);
arg.second = "x";

// now let's make the argument list for the prototype
vector&lt;pair&lt;LiteralType, string&gt; &gt; args;
args.push_back(arg);

// time to make the actual prototype itself
Prototype proto = Prototype("increment", LiteralType(LiteralType::_int), args);

// proto is now a function called "increment" which takes one integer argument called "x" and returns an integer
Body body = Body(proto);

// Now we need an operation to add one to the argument
LiteralValue left = LiteralValue("x"); // this is a reference to a variable named "x"
LiteralValue right = LiteralValue(LiteralInt(1)); // this is a constant integer equaling 1
Operation op = Operation(left, Operation::iadd, right); // add the two integers

// Let's put the operation in the function
body.appendFunctionalNode(op); // the last node appended to a function automatically becomes the return value

// Compile and cast!
typedef int (*myFunctionType)(int);
myFunctionType increment = (myFunctionType)vm.JIT(body);

// Call!
printf("%i\n", increment(5)); // should print 6}}
*/

#endif
