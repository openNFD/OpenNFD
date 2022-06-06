package main;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.lang.String;

import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.misc.NotNull;
import org.antlr.v4.runtime.tree.ParseTree;

import nflanguage.NFCompilerBaseVisitor;
import nflanguage.NFCompilerLexer;
import nflanguage.NFCompilerParser;
import nflanguage.NFCompilerParser.EntryContext;
import nflanguage.NFCompilerParser.ExpressionContext;
import nflanguage.NFCompilerParser.Function_callContext;
import nflanguage.NFCompilerParser.ItemContext;
import nflanguage.NFCompilerParser.Key_valueContext;
import nflanguage.NFCompilerParser.ProgramContext;
import symbols.Symbol;
import symbols.SymbolTable;

public class EvalVisitor extends NFCompilerBaseVisitor<String> {
	private String pre_declare;
	private int var_count = 0;
	private HashSet<String> read_write;
	private HashSet<String> write;
	private HashSet<String> read;

	public EvalVisitor() {
		read = new HashSet<String>();
		write = new HashSet<String>();
		read_write = new HashSet<String>();
	}

	@Override
	public String visitProgram(@NotNull NFCompilerParser.ProgramContext ctx) {
		
		SymbolTable.add(ctx.IDENT().getText(), "program", "program", null, "per-nf");
		SymbolTable.add("f", "flow", "flow", null, "per-nf");
		if (GlobalVars.hasTimer == true) {
			Outputer.pre_write("#define TIMER\n");
			Outputer.pre_write("Flow f_sig1(new int(1));\n" + "void handler(int sig){\n" + "    if (sig==SIGALRM){\n"
					+ "        process(f_sig1);\n" + "    }\n" + "}\n" + "\n" + "void _init_(){\n"
					+ "    (new F_Type())->init();\n" + "\n" + "    sigset_t newmask;\n"
					+ "    sigaddset(&newmask, SIGALRM);\n" + "    signal(SIGALRM,handler);\n"
					+ "    memset(&tick, 0, sizeof(tick));\n" + "    tick.it_value.tv_sec = 0;\n"
					+ "    tick.it_value.tv_usec = 0;\n" + "    tick.it_interval.tv_sec = 0;\n"
					+ "    tick.it_interval.tv_usec = 0;\n" + "}");
		} else {
			Outputer.pre_write("void _init_(){\n" + "    (new F_Type())->init();\n" + "}");
		}
		if (GlobalVars.enableOpenNF) {
			Outputer.pre_write("#define OPENNF_STATE "+GlobalVars.OpenNFState);
		}

		visit(ctx.declaration());
		Outputer.pre_write("#define FUNCTION " + SymbolTable.getProgramName() + "\n");
		
		if (GlobalVars.isTCP == true) {	
			Outputer.write(new File("template/slice3.txt"));
		} else {
			Outputer.writen("int process(Flow &f){\n");
		}
		if (GlobalVars.hasTimer == true) {
			Outputer.writen("if(sigprocmask(SIG_BLOCK,&newmask,NULL)==-1) std::cout<<\"block failed\"<<endl;");
		}
		visit(ctx.entries());
		if (GlobalVars.hasTimer == true) {
			Outputer.writen("if (sigprocmask(SIG_UNBLOCK, &newmask,NULL)==-1) std::cout<<\"unblock failed\"<<endl;");
		}
		if (GlobalVars.isTCP == true) {
			Outputer.write(new File("template/slice4.txt"));
		} else {
			Outputer.writen("        f.clean();\n" + "        return 0;");
			Outputer.writen("}");
			Outputer.writen("int " + ctx.IDENT().getText() + "(u_char * pkt,int totallength) {\n"
					+ "    f_glb= Flow(pkt,totallength);\n" + "    return process(f_glb);\n" + "}");
		}
		for (String item : write) {
			System.out.println("add write field " + item);
		}
		for (String item : read) {
			System.out.println("add read field " + item);
		}
		Template tp;
		try {
			tp = new Template();
			tp.DumpTemplate(write, read);
			tp.AppendTemplate();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

	@Override
	public String visitDeclaration(NFCompilerParser.DeclarationContext ctx) {
		System.out.println("declare");
		return visitChildren(ctx);
	}

	@Override
	public String visitDeclare_entry(@NotNull NFCompilerParser.Declare_entryContext ctx) {
		// type IDENT (LB granu=expression RB)? ( ASSIGN value=expression )? SEMICOLON
		String typename = ctx.type().getText();
		if (typename.startsWith("rule")) {
			String type = new TypeVisitor().visit(ctx.type());
			SymbolTable.add(ctx.IDENT().getText(), type, type, visit(ctx.expression(0)), "per-nf");
			return null;
		} else {
			// map, set, int, IP
			StringBuffer sb = new StringBuffer();
			String state_type = new TypeVisitor().visit(ctx.type());

			String granularity = "per-nf";
			String value = "0";
			if (ctx.granu != null) {
				granularity = ctx.granu.getText();
				for (String ss : granularity.split("&")) {
					read.add(ss);
				}
			}
			if (ctx.value != null) {
				value = ctx.value.getText();
			}
			SymbolTable.add(ctx.IDENT().getText(), "state", state_type, value, granularity);
			Outputer.write("State<" + state_type + "> " + ctx.IDENT().getText() + "(");
			System.out.println(state_type + "`~~");
			if (state_type.equals("int")) {
				if (ctx.value == null)
					Outputer.write("*(new " + state_type + "(" + "0" + "))");
				else
					Outputer.write(visit(ctx.value));
			} else if (state_type.equals("IP")) {
				if (ctx.value == null)
					Outputer.write("*(new " + state_type + "(" + "0.0.0.0/0" + "))");
				else
					Outputer.write(visit(ctx.value));
			} else if (state_type.startsWith("unordered_set") || state_type.startsWith("unordered_map")) {
				if (ctx.value == null)
					Outputer.write("*(new " + state_type + "())");
				else {
					Outputer.write(visit(ctx.value));
				}
			}
			if (ctx.granu != null) {
				Outputer.write(",\"" + ctx.granu.getText() + "\"");
			}
			Outputer.writen(");");

			return null;
		}
	}

	@Override
	public String visitCondition(NFCompilerParser.ConditionContext ctx) {
		Outputer.write(visit(ctx.expression()));
		return null;
	}

	@Override
	public String visitStatement(NFCompilerParser.StatementContext ctx) {
		if (ctx.assignment() != null) {
			Outputer.writen(visit(ctx.assignment()));
		} else if (ctx.function_call() != null) {
			Outputer.writen(visit(ctx.function_call()));
		}
		for (NFCompilerParser.StatementContext c : ctx.statement()) {
			visit(c);
		}
		return null;
	}

	@Override
	public String visitAssignment(NFCompilerParser.AssignmentContext ctx) {
		StringBuffer sb = new StringBuffer();
		String exp2 = ctx.expression(1).getText();
		// VISIT the left expression
		if (ctx.BASIC_TYPE() != null) {
			/* Add local var to symbol table */
			SymbolTable.add(ctx.expression(0).getText(), "var", ctx.BASIC_TYPE().getText(), null, "per-nf");
			if (!exp2.startsWith("connect") && !exp2.startsWith("accept"))
				sb.append(ctx.BASIC_TYPE().getText() + " ");
		}
		read_write = new HashSet<String>();
		String ret1 = visit(ctx.expression(0));
		for (String item : read_write) {
			write.add(item);
			/* for all to-write field, we should read it first */
			read.add(item);
		}
		// VISIT the right expression
		read_write = new HashSet<String>();
		String ret2 = visit(ctx.expression(1));
		for (String item : read_write) {
			read.add(item);
		}

		if (ret2.equals("DROP")) {
			sb.append("return -1;\n");
		}
		// FOR L4, append certain clauses
		else if (exp2.startsWith("connect") || exp2.startsWith("accept")) {
			sb.append(ret2);
			sb.append(ctx.BASIC_TYPE().getText() + " " + ret1 + "= _tpt;\n");
			sb.append("max_sd = max( max_sd," + ret1 + " );\n");
		} else {
			sb.append(ret1);
			sb.append("=");
			sb.append(ret2);
			sb.append(";\n");
		}
		return sb.toString();
	}

	@Override
	public String visitFunction_call(Function_callContext ctx) {
		StringBuffer sb = new StringBuffer();
		if (ctx.funcs().RESUBMIT() != null) {
			sb.append("process(f);\n");
		} else if (ctx.funcs().TIMER() != null) {
			sb.append("tick.it_value.tv_sec = " + ctx.expression_list().expression(0).getText() + ";\n");
			sb.append("tick.it_value.tv_usec =" + ctx.expression_list().expression(1).getText() + ";\n");
			sb.append("setitimer(ITIMER_REAL, &tick, NULL);\n");
		} else if (ctx.funcs().ACCEPT() != null) {
			if (ctx.expression_list().expression().size() != 2) {
				System.out.println("Error! Need one parameters to call this funcion");
				System.exit(0);
			}
			Outputer.pre_write("#define SERVER_ADDR  \""+ctx.expression_list().expression(0).getText()+"\"");
			Outputer.pre_write("#define SERVER_PORT  "+ctx.expression_list().expression(1).getText());
			sb.append("_tpt = accept(listen_sd, NULL, NULL);\n" + "					if (new_sd < 0)\n"
					+ "					{\n" + "						if (errno != EWOULDBLOCK)\n"
					+ "						{\n" + "							perror(\"  accept() failed\");\n"
					+ "							end_server = TRUE;\n" + "						}\n"
					+ "						break;\n" + "					}\n"
					+ "					FD_SET(_tpt, &master_set);\n");
		} else if (ctx.funcs().CONNECT() != null) {
			if (ctx.expression_list().expression().size() != 2) {
				System.out.println("Error! Need two parameters to call connect funcion");
				System.exit(0);
			}
			sb.append("					TCPClient tcp;\n" 
					+ "					_tpt = tcp.setup("
					+ visit(ctx.expression_list().expression(0)) + ".showAddr()," + visit(ctx.expression_list().expression(1))
					+ ");\n" + "					FD_SET(_tpt, &master_set);\n");
		} else if (ctx.funcs().SEND_BACK() != null) {
			sb.append("send(i, buffer, len, 0);\n");
		} else if (ctx.funcs().SEND_TO() != null) {
			if (ctx.expression_list().expression().size() != 1) {
				System.out.println("Error! Need one parameter to call connect funcion");
				System.exit(0);
			}
			sb.append("send(" + visit(ctx.expression_list().expression(0)) + "[i], buffer, len, 0);");
		} else if (ctx.funcs().RECV() != null) {
			sb.append("						recv(i, buffer, sizeof(buffer), 0);\n");
		} else if (ctx.funcs().ENCRYPT() != null){
			GlobalVars.flags.add("encrypt");
			GlobalVars.values.put("key", ctx.expression_list().expression(2).getText());
			String encrypted = visit(ctx.expression_list().expression(0));
			encrypted = encrypted.substring(2, encrypted.length()-1);
			String enc_len = visit(ctx.expression_list().expression(1));
			String to_write = visit(ctx.expression_list().expression(0));
			to_write = to_write.substring(2, to_write.length()-1);
			write.add(to_write.substring(to_write.indexOf('[')+1, to_write.indexOf(']')).toLowerCase());
			if (GlobalVars.enableGPU) {
				sb.append(String.format("gpu_encrypt(key, %s, %s + 34, %s);", encrypted, enc_len , to_write));
			} else{
				sb.append(String.format("cpu_encrypt(key, %s, %s + 34, %s);", encrypted, enc_len , to_write));
			}
		}
		return sb.toString();
	}

	@Override
	public String visitEntries(NFCompilerParser.EntriesContext ctx) {
		int counter = 0;
		for (EntryContext entry : ctx.entry()) {
			if (counter == 0) {
				Outputer.write("if ");
			} else {
				Outputer.write("else if ");
			}
			visit(entry);
			counter++;
		}
		return null;
	}

	@Override
	public String visitEntry(@NotNull NFCompilerParser.EntryContext ctx) {
		System.out.println("entry");
		return visitChildren(ctx);
	}

	@Override
	public String visitSingle(@NotNull NFCompilerParser.SingleContext ctx) {
		StringBuffer sb = new StringBuffer();
		if (ctx.op().getText().equals("connection")) {
			if (!ctx.expression().getText().equals("f")) {
				System.out.println("Here should be a f");
				System.exit(0);
			} else {
				return "(i == listen_sd)";
			}
		} else if (ctx.op().getText().equals("data")) {
			if (!ctx.expression().getText().equals("f")) {
				System.out.println("Here should be a f");
				System.exit(0);
			} else {
				return "(i != listen_sd)";
			}
		} else {
			sb.append(ctx.op().getText());
			sb.append(visit(ctx.expression()));
		}
		return sb.toString();
	}

	@Override
	public String visitDouble(@NotNull NFCompilerParser.DoubleContext ctx) {
		StringBuffer sb = new StringBuffer();
		if (ctx.op().getText().equals("matches")) {
			String flow_name = ctx.left.getText();
			sb.append(visit(ctx.right).replaceAll("_flow", flow_name));
		} else if (ctx.op().getText().equals("mismatches")) {
			String flow_name = ctx.left.getText();
			sb.append(visit(ctx.right).replaceAll("_flow", flow_name).replaceAll("<=", "!="));
		} else if (ctx.op().getText().equals("in")) {
			sb.append(visit(ctx.right) + ".find(" + visit(ctx.left) + ")!=" + visit(ctx.right) + ".end()");
		} else if (ctx.op().getText().equals("|")) {
			// TODO
			String type = new TypeVisitor().visit(ctx);
			sb.append("union_set<" + type + ">(" + visit(ctx.left) + "," + visit(ctx.right) + ")");
		} else {
			sb.append(visit(ctx.left) + " ");
			sb.append(ctx.op().getText());
			sb.append(" " + visit(ctx.right));
		}
		return sb.toString();
	}

	@Override
	public String visitId(NFCompilerParser.IdContext ctx) {
		System.out.println("visit id " + ctx.IDENT().getText());
		String type = SymbolTable.getTypeByName(ctx.IDENT().getText());
		List<ExpressionContext> l = ctx.expression();

		if (l.size() == 0) {
			if (type.equals("rule")) {
				Symbol sym = SymbolTable.getSymbolByName(ctx.IDENT().getText());
				String val = sym.getValue();
				// TODO
				return val;
			} else if (type.equals("state")) {
				return ctx.IDENT().getText() + "[f]";
			} else if (type.equals("flow")) {
				TypeVisitor tv = new TypeVisitor();
				System.out.println(ctx.getText());
				return "(*(" + (tv.visit(ctx.fields())) + "*) " + ctx.IDENT().getText() + ".headers[" + visit(ctx.fields())
						+ "]" + ")";
			} else {
				return ctx.getText();
			}
		} else {
			Iterator<ExpressionContext> it = l.iterator();
			ExpressionContext last = new ExpressionContext();
			StringBuffer sb = new StringBuffer();
			sb.append(ctx.IDENT().getText() + "[f]");
			while (it.hasNext()) {
				last = it.next();
				sb.append("[" + visit(last) + "]");
			}
			TypeVisitor tv = new TypeVisitor();
			// return "(*(" + (tv.visit(ctx)) + "*) " + sb.toString() + ")";
			return sb.toString();
		}
	}

	@Override
	public String visitRule_flow(NFCompilerParser.Rule_flowContext ctx) {
		// TODO
		return visit(ctx.flow_or_rule_entry());
	}

	@Override
	public String visitSet(NFCompilerParser.SetContext ctx) {
		// {}
		StringBuffer sb = new StringBuffer();
		// check type
		String type = new TypeVisitor().visit(ctx.contents());
		int count = ctx.contents().item().size();
		sb.append("create_set<" + type + ">(" + "*(new unordered_set<" + type + ">())," + count);
		for (ItemContext it : ctx.contents().item()) {
			sb.append(",&(" + visit(it) + ")");
		}
		sb.append(")");
		return sb.toString();
	}

	@Override
	public String visitMap(NFCompilerParser.MapContext ctx) {
		// { :, :, :}
		StringBuffer sb = new StringBuffer();
		// check type
		String type1 = new TypeVisitor().visit(ctx.key_pairs().key_value(0).constant(0));
		String type2 = new TypeVisitor().visit(ctx.key_pairs().key_value(0).constant(1));
		int count = ctx.key_pairs().key_value().size() * 2;
		sb.append("create_map<" + type1 + "," + type2 + ">(" + "*(new unordered_map<" + type1 + "," + type2 + ">()),"
				+ count);
		for (Key_valueContext it : ctx.key_pairs().key_value()) {
			sb.append(",&(" + visit(it.constant(0)) + "),&(" + visit(it.constant(1)) + ")");
		}
		sb.append(")");
		return sb.toString();
	}

	@Override
	public String visitNull(NFCompilerParser.NullContext ctx) {
		return ctx.getText();
	}

	@Override
	public String visitLrexp(NFCompilerParser.LrexpContext ctx) {
		return '(' + visit(ctx.expression()) + ')';
	}

	@Override
	public String visitAction(NFCompilerParser.ActionContext ctx) {
		if (ctx.ACTION().getText().equals("DROP")) {
			return "DROP";
		}
		return ctx.getText();
	}

	@Override
	public String visitFields(NFCompilerParser.FieldsContext ctx) {
		try {
			read.add(ctx.getText());
			read_write.add(ctx.getText());
		} catch (NullPointerException e) {
			System.out.println(e.getStackTrace());
		}
		System.out.println("!");
		//return "\"" + ctx.getText() + "\"";
		return ctx.getText().toUpperCase();
	}

	@Override
	public String visitFlow_or_rule_entry(NFCompilerParser.Flow_or_rule_entryContext ctx) {
		// _flow[ KEY ]== VALUE
		StringBuffer sb = new StringBuffer();
		if (ctx.fields().getText().equals("sip") || ctx.fields().getText().equals("dip")) {
			sb.append("*((IP*) ");
			sb.append("_flow.headers[");
			sb.append(visit(ctx.fields()));
			sb.append("])<=");
		} else {
			sb.append("*((int*) ");
			sb.append("_flow.headers[");
			sb.append(visit(ctx.fields()));
			sb.append("])==");
		}
		sb.append(visit(ctx.constant()));
		return sb.toString();
	}

	@Override
	public String visitConst_ip(NFCompilerParser.Const_ipContext ctx) {
		String var = "_t" + String.valueOf(++var_count);
		Outputer.pre_write("IP " + var + "(\"" + ctx.getText() + "\");");
		return var;
	}

	@Override
	public String visitConst_int(NFCompilerParser.Const_intContext ctx) {
		String var = "_t" + String.valueOf(++var_count);
		Outputer.pre_write("int " + var + "=" + ctx.getText() + ";");
		return var;
	}

	@Override
	public String visitMatch_action(NFCompilerParser.Match_actionContext ctx) {
		Outputer.write("(");
		if (ctx.match_flow() == null && ctx.match_state() == null) {
			Outputer.write("1");
		} else if (ctx.match_flow() != null && ctx.match_state() != null) {
			Outputer.write("(");
			visit(ctx.match_flow());
			Outputer.write(")&&(");
			visit(ctx.match_state());
			Outputer.write(")");
		} else if (ctx.match_flow() != null) {
			visit(ctx.match_flow());
		} else if (ctx.match_state() != null) {
			visit(ctx.match_state());
		}
		Outputer.writen("){");

		visit(ctx.action_statements());
		Outputer.writen("}");
		return null;
	}
}
