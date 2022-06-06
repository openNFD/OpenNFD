package main;

import org.antlr.v4.runtime.misc.NotNull;

import nflanguage.NFCompilerBaseVisitor;
import nflanguage.NFCompilerParser;
import nflanguage.NFCompilerParser.ActionContext;
import nflanguage.NFCompilerParser.Const_intContext;
import nflanguage.NFCompilerParser.Dec_basicContext;
import nflanguage.NFCompilerParser.Dec_mapContext;
import nflanguage.NFCompilerParser.Dec_ruleContext;
import nflanguage.NFCompilerParser.Dec_setContext;
import nflanguage.NFCompilerParser.ExpressionContext;
import nflanguage.NFCompilerParser.FieldContext;
import nflanguage.NFCompilerParser.FieldsContext;
import nflanguage.NFCompilerParser.ItemContext;
import nflanguage.NFCompilerParser.LrexpContext;
import nflanguage.NFCompilerParser.NullContext;
import nflanguage.NFCompilerParser.SetContext;
import symbols.SymbolTable;

/**
 * @author hongyi
 *
 */
public class TypeVisitor extends NFCompilerBaseVisitor<String> {
	@Override
	public String visitId(NFCompilerParser.IdContext ctx) {
		System.out.print("type visit "+ctx.getText());
		// flow, state
		if (SymbolTable.getTypeByName(ctx.IDENT().getText()).equals("flow")) {
			
			return visit(ctx.fields());
		}
		// strip  "unordered_set< >" | "unordered_map< >"
		else if (SymbolTable.getTypeByName(ctx.IDENT().getText()).equals("state")) {
			String subtype = SymbolTable.getSubtypeByName(ctx.IDENT().getText());
			for (ExpressionContext ec:ctx.expression()) {
				subtype = subtype.substring(subtype.indexOf(",")+1, subtype.lastIndexOf(">"));
			}
			return subtype;
		}
		return null;
	}
	@Override 
	public String visitFunc(NFCompilerParser.FuncContext ctx) {
		return "function";
	}
	@Override
	public String visitRule_flow(NFCompilerParser.Rule_flowContext ctx) {
		return "rule";
	}
	@Override
	public String visitConst_ip(NFCompilerParser.Const_ipContext ctx) {
		return "IP";
	}
	
	@Override
	public String visitConst_int(Const_intContext ctx) {
		return "int";
	}
	@Override
	public String visitSet(SetContext ctx) {
		String t1 = visit(ctx.contents().item(0));
		for (ItemContext ic : ctx.contents().item()) {
			if (t1.equals(visit (ic))){
				continue;
			}
			System.out.println("set type incompatible");
			return null;
		}
		return "unordered_set<"+t1+">";
	}
	@Override
	public String visitAction(ActionContext ctx) {
		return "IP";
	}
	@Override
	public String visitLrexp(LrexpContext ctx) {
		return visit(ctx.expression());
	}
	@Override
	public String visitNull(NullContext ctx) {
		return "null";
	}
	@Override
	public String visitFields(FieldsContext ctx) {
		if (
				ctx.getText().equals("sip")
				||ctx.getText().equals("dip")
			) {
			return "IP";
		} else if (ctx.getText().equals("payload")) {
			return "char";
		} else {
			return "int";
		}
	}
	@Override
	public String visitAtom(NFCompilerParser.AtomContext ctx) {
		return visit(ctx.item());
	}
	@Override
	public String visitSingle(NFCompilerParser.SingleContext ctx) {
		return visit(ctx.expression());
	}
	@Override
	public String visitDouble(NFCompilerParser.DoubleContext ctx) {
		String lt=visit(ctx.left);
		String rt=visit(ctx.right);
		if (lt.equals(rt)) {
			return lt;
		}
		else {
			System.out.println(ctx.getText()+" ::"+lt+" ::"+rt+" ::expression's op_left and op_left incompatible in type");
			System.exit(0);
			return null;
		}
	}
	@Override
	public String visitDec_set(Dec_setContext ctx) {
		return "unordered_set<"+ctx.BASIC_TYPE().getText()+">";
	}
	@Override
	public String visitDec_rule(Dec_ruleContext ctx) {
		return "rule";
	}
	@Override
	public String visitDec_map(Dec_mapContext ctx) {
		return "unordered_map<"+visit(ctx.type(0))+","+visit(ctx.type(1))+">";
	}
	@Override
	public String visitDec_basic(Dec_basicContext ctx) {
		return ctx.getText();
	}
	

}
