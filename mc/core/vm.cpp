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

#include "vm.h"

namespace vm
{
	LiteralType::LiteralType()
	{
		val = _uninitialized;
		elem = _uninitialized;
	}

	LiteralType::LiteralType(int v)
	{
		elem = _uninitialized;

		if (v == _char || v == _int || v == _double || v == _addr)
		{
			val = v;
			if (v == _addr)
			{
				VM_WARN << "cannot create address without element type";
			}
		}
		else
		{
			VM_WARN << "cannot create illegal type: " << v;
			val = _uninitialized;
		}
	}

	LiteralType::LiteralType(int v, int e)
	{
		if (e == _char || e == _int || e == _double)
			elem = e;
		else
		{
			VM_WARN << "cannot create illegal element type: " << e;
			elem = _uninitialized;
		}

		if (v == _char || v == _int || v == _double || v == _addr)
			val = v;
		else
		{
			VM_WARN << "cannot create illegal type: " << v;
			val = _uninitialized;
		}
	}

	bool LiteralType::initialized()
	{
		return (val != _uninitialized);
	}

	int LiteralType::get()
	{
		return val;
	}

	int LiteralType::getElement()
	{
		return elem;
	}

	Type *LiteralType::gen(VM &vm)
	{
		if (val == _char)
			return Type::getInt8Ty(getGlobalContext());
		else if (val == _int)
			return Type::getInt32Ty(getGlobalContext());
		else if (val == _double)
			return Type::getDoubleTy(getGlobalContext());
		else if (val == _addr)
		{
			if (elem < 0)
			{
				VM_WARN << "illegal address type: " << elem;
				return NULL;
			}
			return PointerType::getUnqual(LiteralType(elem).gen(vm));
		}
		else
		{
			VM_WARN << "illegal type: " << val;
			return NULL;
		}
	}

	string LiteralType::display(string pref)
	{
		if (val == _uninitialized)
			return pref+"<uninitialized>";
		else if (val == _char)
			return pref+"char";
		else if (val == _int)
			return pref+"int";
		else if (val == _double)
			return pref+"double";
		else if (val == _addr)
			return pref+"addr("+LiteralType(elem).display("")+")";
		else
		{
			stringstream ss;
			ss << pref << "<unknown:" << val << ">";
			return ss.str();
		}
	}

	string LiteralType::display()
	{
		return display("");
	}

	LiteralChar::LiteralChar()
	{
		init = false;
		val = 0;
	}

	LiteralChar::LiteralChar(char v)
	{
		init = true;
		val = v;
	}

	bool LiteralChar::initialized()
	{
		return init;
	}

	char LiteralChar::get()
	{
		return val;
	}

	Value *LiteralChar::gen(VM &vm)
	{
		return ConstantInt::get(getGlobalContext(), APInt(8, val, true));
	}

	string LiteralChar::display(string pref)
	{
		if (init)
		{
			stringstream ss;
			ss << pref << "'" << (char)val << "'";
			return ss.str();
		}
		else
		{
			return pref+"<uninitialized>";
		}
	}

	string LiteralChar::display()
	{
		return display("");
	}

	LiteralInt::LiteralInt()
	{
		init = false;
		val = 0;
	}

	LiteralInt::LiteralInt(int v)
	{
		init = true;
		val = v;
	}

	bool LiteralInt::initialized()
	{
		return init;
	}

	int LiteralInt::get()
	{
		return val;
	}

	Value *LiteralInt::gen(VM &vm)
	{
		return ConstantInt::get(getGlobalContext(), APInt(32, val, true));
	}

	string LiteralInt::display(string pref)
	{
		if (init)
		{
			stringstream ss;
			ss << pref << val;
			return ss.str();
		}
		else
		{
			return pref+"<uninitialized>";
		}
	}

	string LiteralInt::display()
	{
		return display("");
	}

	LiteralDouble::LiteralDouble()
	{
		init = false;
		val = 0.00;
	}

	LiteralDouble::LiteralDouble(double v)
	{
		init = true;
		val = v;
	}

	bool LiteralDouble::initialized()
	{
		return init;
	}

	double LiteralDouble::get()
	{
		return val;
	}

	Value *LiteralDouble::gen(VM &vm)
	{
		return ConstantFP::get(getGlobalContext(), APFloat(val));
	}

	string LiteralDouble::display(string pref)
	{
		if (init)
		{
			stringstream ss;
			ss << pref << val;
			return ss.str();
		}
		else
		{
			return pref+"<uninitialized>";
		}
	}

	string LiteralDouble::display()
	{
		return display("");
	}

	LiteralAddress::LiteralAddress()
	{
		init = false;
		val = NULL;
	}

	LiteralAddress::LiteralAddress(void *v)
	{
		init = true;
		elem = LiteralType(LiteralType::_char);
		val = v;
	}

	LiteralAddress::LiteralAddress(LiteralType e, void *v)
	{
		init = true;
		elem = e;
		val = v;
	}

	bool LiteralAddress::initialized()
	{
		return init;
	}

	LiteralType LiteralAddress::getElementType()
	{
		return elem;
	}

	void *LiteralAddress::get()
	{
		return val;
	}

	Value *LiteralAddress::gen(VM &vm)
	{
		return vm.builder->CreatePointerCast(ConstantInt::get(getGlobalContext(), APInt(64, (long int)val, true)), LiteralType(LiteralType::_addr, elem.get()).gen(vm), "constx64_cast");
	}

	string LiteralAddress::display(string pref)
	{
		if (init)
		{
			stringstream ss;
			ss << pref << "(" << elem.display() << ")" << val;
			if (elem.get() == LiteralType::_char)
			{
				char *buf = (char *)val;
				if (buf[0] != 0)
					ss << " == \"" << buf << "\"";
			}
			return ss.str();
		}
		else
		{
			return pref+"<uninitialized>";
		}
	}

	string LiteralAddress::display()
	{
		return display("");
	}

	LiteralValue::LiteralValue()
	{
		type = shared_ptr<LiteralType>(new LiteralType());
		_char = shared_ptr<LiteralChar>();
		_int = shared_ptr<LiteralInt>();
		_double = shared_ptr<LiteralDouble>();
		_addr = shared_ptr<LiteralAddress>();
		name = "";
	}

	LiteralValue::LiteralValue(LiteralChar v)
	{
		if (v.initialized())
		{
			type = shared_ptr<LiteralType>(new LiteralType(LiteralType::_char));
			_char = shared_ptr<LiteralChar>(new LiteralChar(v));
			_int = shared_ptr<LiteralInt>();
			_double = shared_ptr<LiteralDouble>();
			_addr = shared_ptr<LiteralAddress>();
			name = "";
		}
		else
		{
			VM_WARN << "cannot create atomic value from uninitialized char";
			type = shared_ptr<LiteralType>(new LiteralType());
			_char = shared_ptr<LiteralChar>();
			_int = shared_ptr<LiteralInt>();
			_double = shared_ptr<LiteralDouble>();
			_addr = shared_ptr<LiteralAddress>();
			name = "";
		}
	}

	LiteralValue::LiteralValue(LiteralInt v)
	{
		if (v.initialized())
		{
			type = shared_ptr<LiteralType>(new LiteralType(LiteralType::_int));
			_char = shared_ptr<LiteralChar>();
			_int = shared_ptr<LiteralInt>(new LiteralInt(v));
			_double = shared_ptr<LiteralDouble>();
			_addr = shared_ptr<LiteralAddress>();
			name = "";
		}
		else
		{
			VM_WARN << "cannot create atomic value from uninitialized int";
			type = shared_ptr<LiteralType>(new LiteralType());
			_char = shared_ptr<LiteralChar>();
			_int = shared_ptr<LiteralInt>();
			_double = shared_ptr<LiteralDouble>();
			_addr = shared_ptr<LiteralAddress>();
			name = "";
		}
	}

	LiteralValue::LiteralValue(LiteralDouble v)
	{
		if (v.initialized())
		{
			type = shared_ptr<LiteralType>(new LiteralType(LiteralType::_double));
			_char = shared_ptr<LiteralChar>();
			_int = shared_ptr<LiteralInt>();
			_double = shared_ptr<LiteralDouble>(new LiteralDouble(v));
			_addr = shared_ptr<LiteralAddress>();
			name = "";
		}
		else
		{
			VM_WARN << "cannot create atomic value from uninitialized double";
			type = shared_ptr<LiteralType>(new LiteralType());
			_char = shared_ptr<LiteralChar>();
			_int = shared_ptr<LiteralInt>();
			_double = shared_ptr<LiteralDouble>();
			_addr = shared_ptr<LiteralAddress>();
			name = "";
		}
	}

	LiteralValue::LiteralValue(LiteralAddress v)
	{
		if (v.initialized())
		{
			type = shared_ptr<LiteralType>(new LiteralType(LiteralType::_addr, v.getElementType().get()));
			_char = shared_ptr<LiteralChar>();
			_int = shared_ptr<LiteralInt>();
			_double = shared_ptr<LiteralDouble>();
			_addr = shared_ptr<LiteralAddress>(new LiteralAddress(v));
			name = "";
		}
		else
		{
			VM_WARN << "cannot create atomic value from uninitialized addr";
			type = shared_ptr<LiteralType>(new LiteralType());
			_char = shared_ptr<LiteralChar>();
			_int = shared_ptr<LiteralInt>();
			_double = shared_ptr<LiteralDouble>();
			_addr = shared_ptr<LiteralAddress>();
			name = "";
		}
	}

	LiteralValue::LiteralValue(string v)
	{
		type = shared_ptr<LiteralType>(new LiteralType());
		_char = shared_ptr<LiteralChar>();
		_int = shared_ptr<LiteralInt>();
		_double = shared_ptr<LiteralDouble>();
		name = v;

		if (v.empty())
			VM_WARN << "cannot create atomic value from uninitialized variable";
	}

	bool LiteralValue::initialized()
	{
		return (type->initialized() || !name.empty());
	}

	bool LiteralValue::isNamedVariable()
	{
		return !name.empty();
	}

	string LiteralValue::getName()
	{
		return name;
	}

	LiteralType LiteralValue::getType()
	{
		return LiteralType(*type);
	}

	LiteralChar LiteralValue::getChar()
	{
		return LiteralChar(*_char);
	}

	LiteralInt LiteralValue::getInt()
	{
		return LiteralInt(*_int);
	}

	LiteralDouble LiteralValue::getDouble()
	{
		return LiteralDouble(*_double);
	}

	LiteralAddress LiteralValue::getAddress()
	{
		return LiteralAddress(*_addr);
	}

	LiteralChar LiteralValue::castChar()
	{
		if (type->get() == LiteralType::_char)
			return LiteralChar(*_char);
		else if (type->get() == LiteralType::_int)
			return LiteralChar((char)_int->get());
		else if (type->get() == LiteralType::_double)
			return LiteralChar((char)_double->get());
		else if (type->get() == LiteralType::_addr)
			return LiteralChar((char)(long int)_addr->get());
		else
			return LiteralChar();
	}

	LiteralInt LiteralValue::castInt()
	{
		if (type->get() == LiteralType::_char)
			return LiteralInt((int)_char->get());
		else if (type->get() == LiteralType::_int)
			return LiteralInt(*_int);
		else if (type->get() == LiteralType::_double)
			return LiteralInt((int)_double->get());
		else if (type->get() == LiteralType::_addr)
			return LiteralInt((long int)_addr->get());
		else
			return LiteralInt();
	}

	LiteralDouble LiteralValue::castDouble()
	{
		if (type->get() == LiteralType::_char)
			return LiteralDouble((double)_char->get());
		else if (type->get() == LiteralType::_int)
			return LiteralDouble((double)_int->get());
		else if (type->get() == LiteralType::_double)
			return LiteralDouble(*_double);
		else if (type->get() == LiteralType::_addr)
			return LiteralDouble((double)(long int)_addr->get());
		else
			return LiteralDouble();
	}

	LiteralAddress LiteralValue::castAddress()
	{
		if (type->get() == LiteralType::_char)
			return LiteralAddress(LiteralType::_char, (void *)(int)_char->get());
		else if (type->get() == LiteralType::_int)
			return LiteralAddress(LiteralType::_int, (void *)_int->get());
		else if (type->get() == LiteralType::_double)
			return LiteralAddress(LiteralType::_double, (void *)(int)_double->get());
		else if (type->get() == LiteralType::_addr)
			return LiteralAddress(*_addr);
		else
			return LiteralAddress();
	}

	Value *LiteralValue::gen(VM &vm)
	{
		if (name.empty())
		{
			if (type->get() == LiteralType::_char)
				return _char->gen(vm);
			else if (type->get() == LiteralType::_int)
				return _int->gen(vm);
			else if (type->get() == LiteralType::_double)
				return _double->gen(vm);
			else if (type->get() == LiteralType::_addr)
				return _addr->gen(vm);
			else
			{
				VM_WARN << "illegal type: " << type->get();
				NULL;
			}
		}
		else
		{
			return vm.loadMutable(name);
		}

		return NULL;
	}

	string LiteralValue::display(string pref)
	{
		stringstream ss;
		ss << pref;
		if (name.empty())
		{
			ss << "(" << type->display() << ")";
			if (type->get() == LiteralType::_char)
				ss << _char->display();
			else if (type->get() == LiteralType::_int)
				ss << _int->display();
			else if (type->get() == LiteralType::_double)
				ss << _double->display();
			else if (type->get() == LiteralType::_addr)
				ss << _addr->display();
			else
				ss << "<uninitialized>";
		}
		else
		{
			ss << "<symbol:" << name << ">";
		}
		return ss.str();
	}

	string LiteralValue::display()
	{
		return display("");
	}

	FunctionalNode::FunctionalNode()
	{
		type = _uninitialized;
	}

	FunctionalNode::FunctionalNode(MutableDefinition d)
	{
		if (d.initialized())
		{
			type = _mut_definition;
			md = shared_ptr<MutableDefinition>(new MutableDefinition(d));
		}
		else
		{
			VM_WARN << "cannot create functional node from uninitialized variable definition";
			type = _uninitialized;
		}
	}

	FunctionalNode::FunctionalNode(MutableAssignment a)
	{
		if (a.initialized())
		{
			type = _mut_assign;
			ma = shared_ptr<MutableAssignment>(new MutableAssignment(a));
		}
		else
		{
			VM_WARN << "cannot create functional node from uninitialized variable assignment";
			type = _uninitialized;
		}
	}

	FunctionalNode::FunctionalNode(LiteralValue v)
	{
		if (v.initialized())
		{
			type = _value;
			val = shared_ptr<LiteralValue>(new LiteralValue(v));
		}
		else
		{
			VM_WARN << "cannot create functional node from uninitialized atomic value";
			type = _uninitialized;
		}
	}

	FunctionalNode::FunctionalNode(Operation o)
	{
		if (o.initialized())
		{
			type = _arith;
			arith = shared_ptr<Operation>(new Operation(o));
		}
		else
		{
			VM_WARN << "cannot create functional node from uninitialized operation";
			type = _uninitialized;
		}
	}

	FunctionalNode::FunctionalNode(Call c)
	{
		if (c.initialized())
		{
			type = _call;
			call = shared_ptr<Call>(new Call(c));
		}
		else
		{
			VM_WARN << "cannot create functional node from uninitialized function call";
			type = _uninitialized;
		}
	}

	FunctionalNode::FunctionalNode(IfStatement i)
	{
		if (i.initialized())
		{
			type = _if;
			ifs = shared_ptr<IfStatement>(new IfStatement(i));
		}
		else
		{
			VM_WARN << "cannot create functional node from uninitialized if statement";
			type = _uninitialized;
		}
	}

	FunctionalNode::FunctionalNode(ForLoop f)
	{
		if (f.initialized())
		{
			type = _forl;
			forl = shared_ptr<ForLoop>(new ForLoop(f));
		}
		else
		{
			VM_WARN << "cannot create functional node from uninitialized for loop";
			type = _uninitialized;
		}
	}

	bool FunctionalNode::initialized()
	{
		if (type == _mut_definition)
			return md && md->initialized();
		else if (type == _mut_assign)
			return ma && ma->initialized();
		else if (type == _value)
			return val && val->initialized();
		else if (type == _arith)
			return arith && arith->initialized();
		else if (type == _call)
			return call && call->initialized();
		else if (type == _if)
			return ifs && ifs->initialized();
		else if (type == _forl)
			return forl && forl->initialized();
		else
			return false;
	}

	int FunctionalNode::getType()
	{
		return type;
	}

	string FunctionalNode::getTypeString()
	{
		stringstream ss;
		ss << type;
		switch(type)
		{
			case _uninitialized: ss << " (uninitialized)"; break;
			case _mut_definition: ss << " (variable definition)"; break;
			case _mut_assign: ss << " (variable assignment)"; break;
			case _value: ss << " (value)"; break;
			case _arith: ss << " (operation)"; break;
			case _call: ss << " (function call)"; break;
			case _if: ss << " (if statement)"; break;
			case _forl: ss << " (for loop)"; break;
			default: break;
		}

		return ss.str();
	}

	MutableDefinition FunctionalNode::getMutableDefinition()
	{
		return MutableDefinition(*md);
	}

	MutableAssignment FunctionalNode::getMutableAssignment()
	{
		return MutableAssignment(*ma);
	}

	LiteralValue FunctionalNode::getValue()
	{
		return LiteralValue(*val);
	}

	Operation FunctionalNode::getOperation()
	{
		return Operation(*arith);
	}

	Call FunctionalNode::getCall()
	{
		return Call(*call);
	}

	IfStatement FunctionalNode::getIfStatement()
	{
		return IfStatement(*ifs);
	}

	ForLoop FunctionalNode::getForLoop()
	{
		return ForLoop(*forl);
	}

	Value *FunctionalNode::gen(VM &vm)
	{
		if (type == _mut_definition)
			return md->gen(vm);
		else if (type == _mut_assign)
			return ma->gen(vm);
		else if (type == _value)
			return val->gen(vm);
		else if (type == _arith)
			return arith->gen(vm);
		else if (type == _call)
			return call->gen(vm);
		else if (type == _if)
			return ifs->gen(vm);
		else if (type == _forl)
			return forl->gen(vm);
		else
		{
			VM_WARN << "illegal node type: " << getTypeString();
			return NULL;
		}
	}

	string FunctionalNode::display(string pref)
	{
		if (type == _mut_definition)
			return md->display(pref);
		else if (type == _mut_assign)
			return ma->display(pref);
		else if (type == _value)
			return val->display(pref);
		else if (type == _arith)
			return arith->display(pref);
		else if (type == _call)
			return call->display(pref);
		else if (type == _if)
			return ifs->display(pref);
		else if (type == _forl)
			return forl->display(pref);
		else
			return pref+"<uninitialized>";
	}

	string FunctionalNode::display()
	{
		return display("");
	}

	MutableDefinition::MutableDefinition()
	{
	}

	MutableDefinition::MutableDefinition(LiteralType t, string n)
	{
		type = t;
		name = n;
		init = FunctionalNode();

		if (!type.initialized())
		{
			VM_WARN << "cannot create variable definition from uninitialized atomic type: " << name;
			name = "";
		}

		if (name.empty())
		{
			VM_WARN << "cannot create variable definition from uninitialized variable symbol";
		}
	}

	MutableDefinition::MutableDefinition(LiteralType t, string n, FunctionalNode v)
	{
		type = t;
		name = n;
		init = v;

		if (name.empty())
		{
			VM_WARN << "cannot create variable definition from uninitialized variable symbol";
		}

		if (!type.initialized())
		{
			VM_WARN << "JIT: cannot create variable definition from uninitialized atomic type: " << name;
			name = "";
		}

		if (!init.initialized())
		{
			VM_WARN << "cannot create variable definition from uninitialized functional node: " << name;
			name = "";
		}

		if (v.getType() == FunctionalNode::_value)
		{
			if (v.getValue().getType().get() != t.get())
			{
				VM_WARN << "mitmatched types in variable definition: " << name;
				name = "";
			}
		}
	}

	bool MutableDefinition::initialized()
	{
		return (!name.empty());
	}

	LiteralType MutableDefinition::getType()
	{
		return type;
	}

	string MutableDefinition::getName()
	{
		return name;
	}

	FunctionalNode MutableDefinition::getValue()
	{
		return init;
	}

	Value *MutableDefinition::gen(VM &vm)
	{
		if (vm.variables.find(name) != vm.variables.end())
		{
			VM_WARN << "cannot redefine variable: " << name;
			return NULL;
		}

		Function *f = vm.builder->GetInsertBlock()->getParent();
		if (f == NULL)
		{
			VM_WARN << "cannot define variable without parent function: " << name;
			return NULL;
		}

		AllocaInst *ali = vm.createEntryBlockAlloca(f, name, type.gen(vm));
		if (ali == NULL)
		{
			VM_WARN << "JIT: cannot create entry block allocation: " << name;
		}

		vm.variables[name] = ali;
		return vm.storeMutable(name, init.gen(vm));
	}

	string MutableDefinition::display(string pref)
	{
		if (name.empty())
		{
			return pref+"<uninitialized>";
		}
		else
		{
			stringstream ss;
			ss << pref << type.display() << " " << name << " =\n";
			ss << init.display(pref+"  ");
			return ss.str();
		}
	}

	string MutableDefinition::display()
	{
		return display("");
	}

	MutableAssignment::MutableAssignment()
	{
	}

	MutableAssignment::MutableAssignment(string n, FunctionalNode v)
	{
		name = n;
		val = v;

		if (!val.initialized())
		{
			VM_WARN << "cannot create variable assignment from uninitialized functional node: " << name;
			name = "";
		}
	}

	bool MutableAssignment::initialized()
	{
		return (!name.empty());
	}

	string MutableAssignment::getName()
	{
		return name;
	}

	FunctionalNode MutableAssignment::getValue()
	{
		return val;
	}

	Value *MutableAssignment::gen(VM &vm)
	{
		Value *tmp = val.gen(vm);
		if (tmp == NULL)
		{
			VM_WARN << "error when creating value for: " << name;
			return NULL;
		}

		if (vm.variables.find(name) == vm.variables.end())
		{
			VM_WARN << "cannot assign value to undefined variable: " << name;
			return NULL;
		}

		return vm.storeMutable(name, tmp);
	}

	string MutableAssignment::display(string pref)
	{
		if (name.empty())
		{
			return pref+"<uninitialized>";
		}
		else
		{
			stringstream ss;
			ss << pref << name << " =\n";
			ss << val.display(pref+"  ");
			return ss.str();
		}
	}

	string MutableAssignment::display()
	{
		return display("");
	}

	Operation::Operation()
	{
		op = uninitialized;
	}

	Operation::Operation(FunctionalNode l, int o, FunctionalNode r)
	{
		left = l;
		op = o;
		right = r;

		if (!left.initialized())
		{
			VM_WARN << "cannot create operation from uninitialized left functional node: " << op;
			op = uninitialized;
		}

		if (!right.initialized())
		{
			VM_WARN << "cannot create operation from uninitialized right functional node: " << op;
			op = uninitialized;
		}
	}

	Operation::Operation(int o, FunctionalNode r)
	{
		op = o;
		right = r;

		if (!right.initialized())
		{
			VM_WARN << "cannot create operation from uninitialized right functional node";
			VM_WARN << "  operand" << op;
			op = uninitialized;
		}
	}

	bool Operation::initialized()
	{
		return (op != uninitialized);
	}

	int Operation::getOp()
	{
		return op;
	}

	FunctionalNode Operation::getLeft()
	{
		return left;
	}

	FunctionalNode Operation::getRight()
	{
		return right;
	}

	Value *Operation::gen(VM &vm)
	{
		Value *lv = NULL;
		if (left.initialized())
		{
			lv = left.gen(vm);
			if (lv == NULL)
			{
				VM_WARN << "error when generating the left side for this operation";
				VM_WARN << "  operand" << op;
			}
		}
		Value *rv = NULL;
		if (right.initialized())
		{
			rv = right.gen(vm);
			if (rv == NULL)
			{
				VM_WARN << "error when generating the right side for this operation";
				VM_WARN << "  operand" << op;
			}
		}
		else
		{
			VM_WARN << "operations need to have at least one initialized branch (the right one) - there was probably an error creating the functional node for that";
			return NULL;
		}

		if (op == iadd)
			return vm.builder->CreateAdd(lv, rv, "iadd");
		else if (op == fadd)
			return vm.builder->CreateFAdd(lv, rv, "fadd");
		else if (op == isub)
			return vm.builder->CreateSub(lv, rv, "isub");
		else if (op == fsub)
			return vm.builder->CreateFSub(lv, rv, "fsub");
		else if (op == imul)
			return vm.builder->CreateMul(lv, rv, "imul");
		else if (op == fmul)
			return vm.builder->CreateFMul(lv, rv, "fmul");
		else if (op == idiv)
			return vm.builder->CreateFPToSI(
				vm.builder->CreateFDiv(
					vm.builder->CreateSIToFP(lv, LiteralType(LiteralType::_double).gen(vm), "idiv_cast_left"),
					vm.builder->CreateSIToFP(rv, LiteralType(LiteralType::_double).gen(vm), "idiv_cast_right"),
				"idiv"),
			LiteralType(LiteralType::_int).gen(vm), "idiv_cast_rtn");
		else if (op == fdiv)
			return vm.builder->CreateFDiv(lv, rv, "fdiv");
		else if (op == iand)
			return vm.builder->CreateZExt(vm.builder->CreateAnd(lv, rv, "iand"), LiteralType(LiteralType::_int).gen(vm), "iand_cast");
		else if (op == ior)
			return vm.builder->CreateZExt(vm.builder->CreateOr(lv, rv, "ior"), LiteralType(LiteralType::_int).gen(vm), "ior_cast");
		else if (op == ixor)
			return vm.builder->CreateZExt(vm.builder->CreateXor(lv, rv, "ixor"), LiteralType(LiteralType::_int).gen(vm), "ixor_cast");
		else if (op == ilt)
			return vm.builder->CreateZExt(vm.builder->CreateICmpSLT(lv, rv, "ilt"), LiteralType(LiteralType::_int).gen(vm), "ilt_cast");
		else if (op == flt)
			return vm.builder->CreateFPExt(vm.builder->CreateFCmpOLT(lv, rv, "flt"), LiteralType(LiteralType::_double).gen(vm), "flt_cast");
		else if (op == igt)
			return vm.builder->CreateZExt(vm.builder->CreateICmpSGT(lv, rv, "igt"), LiteralType(LiteralType::_int).gen(vm), "igt_cast");
		else if (op == fgt)
			return vm.builder->CreateZExt(vm.builder->CreateFCmpOGT(lv, rv, "fgt"), LiteralType(LiteralType::_double).gen(vm), "fgt_cast");
		else if (op == ile)
			return vm.builder->CreateZExt(vm.builder->CreateICmpSLE(lv, rv, "ile"), LiteralType(LiteralType::_int).gen(vm), "ile_cast");
		else if (op == fle)
			return vm.builder->CreateFPExt(vm.builder->CreateFCmpOLE(lv, rv, "fle"), LiteralType(LiteralType::_double).gen(vm), "fle_cast");
		else if (op == ige)
			return vm.builder->CreateZExt(vm.builder->CreateICmpSGE(lv, rv, "ige"), LiteralType(LiteralType::_int).gen(vm), "ige_cast");
		else if (op == fge)
			return vm.builder->CreateFPExt(vm.builder->CreateFCmpOGE(lv, rv, "fge"), LiteralType(LiteralType::_double).gen(vm), "fge_cast");
		else if (op == ieq)
			return vm.builder->CreateZExt(vm.builder->CreateICmpEQ(lv, rv, "ieq"), LiteralType(LiteralType::_int).gen(vm), "ieq_cast");
		else if (op == feq)
			return vm.builder->CreateFPExt(vm.builder->CreateFCmpOEQ(lv, rv, "feq"), LiteralType(LiteralType::_double).gen(vm), "feq_cast");
		else if (op == ine)
			return vm.builder->CreateZExt(vm.builder->CreateICmpNE(lv, rv, "ine"), LiteralType(LiteralType::_int).gen(vm), "ine_cast");
		else if (op == fne)
			return vm.builder->CreateFPExt(vm.builder->CreateFCmpONE(lv, rv, "fne"), LiteralType(LiteralType::_double).gen(vm), "fne_cast");
		else if (op == ineg)
			return vm.builder->CreateNeg(rv, "ineg");
		else if (op == casti8)
			return vm.builder->CreateSExt(rv, LiteralType(LiteralType::_char).gen(vm), "casti8");
		else if (op == casti32)
			return vm.builder->CreateSExt(rv, LiteralType(LiteralType::_int).gen(vm), "casti32");
		else if (op == castf32)
			return vm.builder->CreateFPExt(rv, LiteralType(LiteralType::_double).gen(vm), "castf32");
		else if (op == castxi8)
			return vm.builder->CreateIntToPtr(vm.builder->CreateSExt(rv, LiteralType(LiteralType::_int).gen(vm), "castxi8_casti32"), LiteralType(LiteralType::_addr, LiteralType::_char).gen(vm), "castxi8");
		else if (op == castxi32)
			return vm.builder->CreateIntToPtr(vm.builder->CreateSExt(rv, LiteralType(LiteralType::_int).gen(vm), "castxi32_casti32"), LiteralType(LiteralType::_addr, LiteralType::_int).gen(vm), "castxi32");
		else if (op == castxf32)
			return vm.builder->CreateIntToPtr(vm.builder->CreateSExt(rv, LiteralType(LiteralType::_int).gen(vm), "castxf32_casti32"), LiteralType(LiteralType::_addr, LiteralType::_double).gen(vm), "castxf32");
		else if (op == derefxi8)
			return vm.builder->CreateLoad(vm.builder->CreatePointerCast(rv, LiteralType(LiteralType::_addr, LiteralType::_char).gen(vm), "derefxi8_cast"), "derefxi8");
		else if (op == derefxi32)
			return vm.builder->CreateLoad(vm.builder->CreatePointerCast(rv, LiteralType(LiteralType::_addr, LiteralType::_int).gen(vm), "derefxi32_cast"), "derefxi32");
		else if (op == derefxf32)
			return vm.builder->CreateLoad(vm.builder->CreatePointerCast(rv, LiteralType(LiteralType::_addr, LiteralType::_double).gen(vm), "derefxf32_cast"), "derefxf32");
		else if (op == xaddr)
			return vm.builder->CreateGEP(rv, LiteralInt(0).gen(vm), "xaddr");
		else
		{
			VM_WARN << "unknown operation id: " << op;
			return NULL;
		}
	}

	string Operation::display(string pref)
	{
		stringstream ss;
		ss << pref << "operation " << op << "\n";
		ss << left.display(pref+"  ") << "\n";
		ss << right.display(pref+"  ");
		return ss.str();
	}

	string Operation::display()
	{
		return display("");
	}

	Call::Call()
	{
	}

	Call::Call(string f, vector<FunctionalNode> a)
	{
		func = f;
		args = a;

		if (func.empty())
		{
			VM_WARN << "cannot create function call from uninitialized function symbol";
		}
	}

	bool Call::initialized()
	{
		return !func.empty();
	}

	string Call::getName()
	{
		return func;
	}

	Value *Call::gen(VM &vm)
	{
		Function *f = vm.module->getFunction(func);
		if (f == NULL)
		{
			VM_WARN << "call to undefined function: " << func;
			return NULL;
		}

		if (f->arg_size() != args.size())
		{
			VM_WARN << "mismatched arguments to function: " << func;
			return NULL;
		}

		vector<Value *> ag;
		for (vector<FunctionalNode>::iterator i = args.begin(); i != args.end(); i++)
		{
			Value *tmp = i->gen(vm);
			if (tmp == NULL)
			{
				VM_WARN << "error when generating argument for: " << func;
				return NULL;
			}
			ag.push_back(tmp);
		}

		return vm.builder->CreateCall(f, ag, "call");
	}

	string Call::display(string pref)
	{
		stringstream ss;
		ss << pref << "call " << func;
		for (int i = 0; i < args.size(); i++)
			ss << "\n" << args[i].display(pref+"  ");
		return ss.str();
	}

	string Call::display()
	{
		return display("");
	}

	IfStatement::IfStatement()
	{
		init = false;
	}

	IfStatement::IfStatement(FunctionalNode c, FunctionalNode t, FunctionalNode e)
	{
		init = true;
		cond = c;
		then = t;
		other = e;

		if (!cond.initialized())
		{
			VM_WARN << "cannot create if statement from uninitialized condition functional node";
			init = false;
		}

		if (!then.initialized())
		{
			VM_WARN << "cannot create if statement from uninitialized then functional node";
			init = false;
		}

		if (!other.initialized())
		{
			VM_WARN << "cannot create if statement from uninitialized else functional node";
			init = false;
		}
	}

	bool IfStatement::initialized()
	{
		return init;
	}

	FunctionalNode IfStatement::getCondition()
	{
		return cond;
	}

	FunctionalNode IfStatement::getThen()
	{
		return then;
	}

	FunctionalNode IfStatement::getElse()
	{
		return other;
	}

	Value *IfStatement::gen(VM &vm)
	{
		Value *condv = cond.gen(vm);
		if (condv == NULL)
		{
			VM_WARN << "error when generating condition for if statement";
			return NULL;
		}

		condv = vm.builder->CreateICmpSGT(condv, LiteralInt(0).gen(vm), "ifcond");

		Function *f = vm.builder->GetInsertBlock()->getParent();
		if (f == NULL)
		{
			VM_WARN << "cannot generate if statement without parent function";
			return NULL;
		}
		BasicBlock *thenbb = BasicBlock::Create(getGlobalContext(), "then", f);
		BasicBlock *otherbb = BasicBlock::Create(getGlobalContext(), "other");
		BasicBlock *mergebb = BasicBlock::Create(getGlobalContext(), "merge");

		vm.builder->CreateCondBr(condv, thenbb, otherbb);

		vm.builder->SetInsertPoint(thenbb);
		Value *thenv = then.gen(vm);
		if (thenv == NULL)
		{
			VM_WARN << "error when generating then expression for if statement";
			return NULL;
		}

		vm.builder->CreateBr(mergebb);
		thenbb = vm.builder->GetInsertBlock();

		f->getBasicBlockList().push_back(otherbb);
		vm.builder->SetInsertPoint(otherbb);

		Value *otherv = other.gen(vm);
		if (otherv == NULL)
		{
			VM_WARN << "error when generating else expression for if statement";
			return NULL;
		}

		vm.builder->CreateBr(mergebb);
		otherbb = vm.builder->GetInsertBlock();

		f->getBasicBlockList().push_back(mergebb);
		vm.builder->SetInsertPoint(mergebb);
		PHINode *pn = vm.builder->CreatePHI(thenv->getType(), 2, "ifs");
		pn->addIncoming(thenv, thenbb);
		pn->addIncoming(otherv, otherbb);
		return pn;
	}

	string IfStatement::display(string pref)
	{
		stringstream ss;
		ss << pref << "if =\n";
		ss << cond.display(pref+"  ") << "\n";
		ss << pref << "then\n";
		ss << then.display(pref+"  ") << "\n";
		ss << pref << "else\n";
		ss << other.display(pref+"  ");
		return ss.str();
	}

	string IfStatement::display()
	{
		return display("");
	}

	ForLoop::ForLoop()
	{
	}

	ForLoop::ForLoop(string v, FunctionalNode i, FunctionalNode e, FunctionalNode s, FunctionalNode b)
	{
		var = v;
		init = i;
		end = e;
		step = s;
		body = b;

		if (var.empty())
		{
			VM_WARN << "cannot create for loop from uninitialized variable symbol";
		}

		if (!init.initialized())
		{
			VM_WARN << "cannot create for loop from uninitialized initial functional node: " << var;
			var = "";
		}

		if (!end.initialized())
		{
			VM_WARN << "cannot create for loop from uninitialized final functional node: " << var;
			var = "";
		}

		if (!step.initialized())
		{
			VM_WARN << "cannot create for loop from uninitialized step functional node: " << var;
			var = "";
		}

		if (!body.initialized())
		{
			VM_WARN << "cannot create for loop from uninitialized body functional node: " << var;
			var = "";
		}
	}

	bool ForLoop::initialized()
	{
		return !var.empty();
	}

	string ForLoop::getVariable()
	{
		return var;
	}

	FunctionalNode ForLoop::getInitial()
	{
		return init;
	}

	FunctionalNode ForLoop::getFinal()
	{
		return end;
	}

	FunctionalNode ForLoop::getStep()
	{
		return step;
	}

	FunctionalNode ForLoop::getBody()
	{
		return body;
	}

	Value *ForLoop::gen(VM &vm)
	{
		Function *f = vm.builder->GetInsertBlock()->getParent();
		if (f == NULL)
		{
			VM_WARN << "cannot generate for loop without parent function: " << var;
			return NULL;
		}

		AllocaInst *alloca = vm.createEntryBlockAlloca(f, var, LiteralType(init.getType()).gen(vm));

		Value *initv = init.gen(vm);
		if (initv == NULL)
		{
			VM_WARN << "error when generating initial value for for loop: " << var;
			return NULL;
		}

		vm.builder->CreateStore(initv, alloca);

		BasicBlock *loopbb = BasicBlock::Create(getGlobalContext(), "forl", f);
		vm.builder->CreateBr(loopbb);
		vm.builder->SetInsertPoint(loopbb);

		Value *bodyv = body.gen(vm);
		if (bodyv == NULL)
		{
			VM_WARN << "error when generating body expression for for loop: " << var;
			return NULL;
		}

		Value *stepv = step.gen(vm);
		if (stepv == NULL)
		{
			VM_WARN << "error when generating step value for for loop: " << var;
			return NULL;
		}

		Value *endv = end.gen(vm);
		if (endv == NULL)
		{
			VM_WARN << "error when generating final value for for loop: " << var;
			return NULL;
		}

		Value *curv = vm.builder->CreateLoad(alloca, var.c_str());
		Value *nextv = vm.builder->CreateAdd(curv, stepv, "forl_step");
		vm.builder->CreateStore(nextv, alloca);

		endv = vm.builder->CreateICmpSGT(endv, LiteralInt(0).gen(vm), "forl_cond");

		BasicBlock *afterbb = BasicBlock::Create(getGlobalContext(), "forl_after", f);
		vm.builder->CreateCondBr(endv, loopbb, afterbb);
		vm.builder->SetInsertPoint(afterbb);

		return LiteralInt(0).gen(vm);
	}

	string ForLoop::display(string pref)
	{
		stringstream ss;
		ss << pref << "for " << var << " =\n";
		ss << init.display(pref+"  ") << "\n";
		ss << pref << "<\n";
		ss << end.display(pref+"  ") << "\n";
		ss << pref << var << " =\n";
		ss << step.display(pref+"  ") << "\n";
		ss << pref << "do\n";
		ss << body.display(pref+"  ");
		return ss.str();
	}

	string ForLoop::display()
	{
		return display("");
	}

	Prototype::Prototype()
	{
	}

	Prototype::Prototype(string n, LiteralType r, vector<pair<LiteralType, string> > a)
	{
		rtn = r;
		args = a;
		name = n;

		if (n.empty())
		{
			VM_WARN << "cannot create prototype from uninitialized function symbol";
		}

		if (!rtn.initialized())
		{
			VM_WARN << "cannot create prototype from uninitialized return type: " << name;
			name = "";
		}
	}

	bool Prototype::initialized()
	{
		return (!name.empty());
	}

	unsigned int Prototype::size()
	{
		return args.size();
	}

	LiteralType Prototype::getReturnType()
	{
		return rtn;
	}

	string Prototype::getName()
	{
		return name;
	}

	string Prototype::getArgumentName(unsigned int idx)
	{
		return args[idx].second;
	}

	LiteralType Prototype::getArgumentType(unsigned int idx)
	{
		return args[idx].first;
	}

	Function *Prototype::gen(VM &vm)
	{
		vector<Type *> atypes;
		for (vector<pair<LiteralType, string> >::iterator i = args.begin(); i != args.end(); i++)
		{
			Type *tmp = i->first.gen(vm);
			if (tmp == NULL)
			{
				VM_WARN << "error when generating argument type for prototype: argument " << i->second << " in " << name;
				return NULL;
			}
			atypes.push_back(tmp);
		}

		Type *r = rtn.gen(vm);
		if (r == NULL)
		{
			VM_WARN << "error when generating return type for prototype: " << name;
			return NULL;
		}

		FunctionType *ft = FunctionType::get(r, atypes, false);
		if (ft == NULL)
		{
			VM_WARN << "unable to generate prototype: " << name;
			return NULL;
		}

		Function *f = Function::Create(ft, Function::ExternalLinkage, name, vm.module.get());
		if (f == NULL)
		{
			VM_WARN << "unable to generate function shell for prototype: " << name;
		}

		if (f->getName().compare(name) != 0)
		{
			f->eraseFromParent();
			f = vm.module->getFunction(name);

			if (!f->empty())
			{
				return NULL;
			}

			if (f->arg_size() != args.size())
			{
				return NULL;
			}
		}

		unsigned int idx = 0;
		for (Function::arg_iterator i = f->arg_begin(); idx < args.size(); i++, idx++)
			i->setName(args[idx].second);

		return f;
	}

	string Prototype::display(string pref)
	{
		stringstream ss;
		ss << pref << rtn.display() << " " << name << "(";
		for (int i = 0; i < args.size(); i++)
			ss << "\n" << args[i].first.display(pref+"  ") << " " << args[i].second;
		if (args.size() > 0)
			ss << pref;
		ss << ")";
		return ss.str();
	}

	string Prototype::display()
	{
		return display("");
	}

	Body::Body()
	{
	}

	Body::Body(Prototype p)
	{
		ptype = p;

		if (!ptype.initialized())
		{
			VM_WARN << "cannot create function body from uninitialized prototype";
		}
	}

	bool Body::initialized()
	{
		return ptype.initialized();
	}

	Prototype Body::getPrototype()
	{
		return ptype;
	}

	int Body::appendFunctionalNode(FunctionalNode n)
	{
		nodes.push_back(n);
		return nodes.size()-1;
	}

	Function *Body::gen(VM &vm)
	{
		Function *f = ptype.gen(vm);
		if (f == NULL)
		{
			VM_WARN << "error when generating prototype for function: " << ptype.getName();
			return NULL;
		}

		BasicBlock *bb = BasicBlock::Create(getGlobalContext(), "body", f);
		vm.builder->SetInsertPoint(bb);

		unsigned int idx = 0;
		for (Function::arg_iterator i = f->arg_begin(); idx < ptype.size(); i++, idx++)
		{
			AllocaInst *alloca = vm.createEntryBlockAlloca(f, ptype.getArgumentName(idx), ptype.getArgumentType(idx).gen(vm));
			vm.builder->CreateStore(i, alloca);
			vm.variables[ptype.getArgumentName(idx)] = alloca;
		}
		
		Value *rv = NULL;

		for (vector<FunctionalNode>::iterator i = nodes.begin(); i != nodes.end(); i++)
			rv = i->gen(vm);
		if (rv == NULL)
		{
			VM_WARN << "function is either empty or there was an error when generating: " << ptype.getName();
			return NULL;
		}

		vm.builder->CreateRet(rv);

		if (verifyFunction(*f))
		{
			VM_WARN << "function did not pass verification pass: " << ptype.getName();
		}

		vm.fpm->run(*f);
		return f;
	}

	string Body::display(string pref)
	{
		stringstream ss;
		ss << pref << ptype.display(pref);
		for (int i = 0; i < nodes.size(); i++)
			ss << "\n" << nodes[i].display(pref+"  ");
		return ss.str();
	}

	string Body::display()
	{
		return display("");
	}

	AllocaInst *VM::createEntryBlockAlloca(Function *f, string n, Type *t)
	{
	  IRBuilder<> tmpBuilder(&f->getEntryBlock(), f->getEntryBlock().begin());
	  return tmpBuilder.CreateAlloca(t, 0, n.c_str());
	}

	Value *VM::loadMutable(string n)
	{
		return builder->CreateLoad(variables[n], n.c_str());
	}

	Value *VM::storeMutable(string n, Value *v)
	{
		return builder->CreateStore(v, variables[n]);
	}

	VM::VM()
	{
		InitializeNativeTarget();
		module = shared_ptr<Module>(new Module("-_- VM", getGlobalContext()));
		builder = shared_ptr<IRBuilder<> >(new IRBuilder<>(getGlobalContext()));
		string estr;
		ee = EngineBuilder(module.get()).setErrorStr(&estr).create();
		if (ee == NULL)
		{
			VM_WARN << "error when creating execution engine (" << estr << ")";
		}
		fpm = shared_ptr<FunctionPassManager>(new FunctionPassManager(module.get()));
	}

	VM::~VM()
	{
	}

	int VM::setOptimizationLevel(int l)
	{
		if (l == 0)
		{
			fpm->doInitialization();
			return 0;
		}
		else if (l == 1)
		{
			fpm->add(createBasicAliasAnalysisPass());
			fpm->add(createPromoteMemoryToRegisterPass());
			fpm->add(createInstructionCombiningPass());
			fpm->add(createReassociatePass());
			fpm->add(createGVNPass());
			fpm->add(createCFGSimplificationPass());
			fpm->doInitialization();

			return 0;
		}
		else
		{
			fpm->doInitialization();
			return 1;
		}
	}

	int VM::linkExternal(Prototype p, void *f)
	{
		if (f == NULL)
			return 1;

		if (!p.initialized())
			return 2;

		Function *pf = p.gen(*this);
		if (pf == NULL)
			return 3;

		ee->addGlobalMapping(pf, f);
		return 0;
	}

	void *VM::JIT(Body b, bool d)
	{
		Function *f = b.gen(*this);
		if (f == NULL)
		{
			VM_WARN << "error when generating function: " << b.getPrototype().getName();
			return NULL;
		}

		if (d)
		{
			module->dump();
		}

		void *rtn = ee->getPointerToFunction(f);
		if (rtn == NULL)
		{
			VM_WARN << "error when compiling function to binary: " << b.getPrototype().getName();
		}
		else
		{
			funcs[b.getPrototype().getName()] = f;
		}
		return rtn;
	}

	void *VM::JIT(Body b)
	{
		return JIT(b, false);
	}

	vector<string> VM::listJITedFunctions()
	{
		vector<string> rtn;
		for (map<string, Function *>::iterator i = funcs.begin(); i != funcs.end(); i++)
		{
			if (i->second != NULL)
			{
				rtn.push_back(i->first);
			}
		}
		return rtn;
	}

	int VM::unJIT(string n)
	{
		if (funcs.find(n) == funcs.end())
		{
			return 1;
		}

		if (funcs[n] == NULL)
		{
			return 2;
		}

		ee->freeMachineCodeForFunction(funcs[n]);
		funcs[n] = NULL;

		return 0;
	}
}
