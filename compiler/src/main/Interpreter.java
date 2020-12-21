package main;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.Charset;

import org.antlr.v4.runtime.ANTLRInputStream;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.tree.ParseTree;

import nflanguage.NFCompilerLexer;
import nflanguage.NFCompilerParser;

public class Interpreter {
	
	public static void main(String[] args) {
		
		CharStream input;
		try {
			Outputer opt = new Outputer("NF/NF.c");
			
			File f = new File("NF/");
			if(!f.exists()) {
				f.mkdirs();
			}
			boolean flag= false;
			String finput = new String();
			for (String item: f.list()) {
				if (item.endsWith(".txt")) {finput = item; flag=true; break;}
			}
			if(flag==false) {
				System.out.println("No model txt found!");
				System.exit(-1);
			}
			input = CharStreams.fromFileName("NF/"+finput, Charset.forName("utf-8"));
			NFCompilerLexer lexer = new NFCompilerLexer(input);
			CommonTokenStream tokens = new CommonTokenStream(lexer);
			NFCompilerParser parser = new NFCompilerParser(tokens);
			ParseTree tree = parser.program();
			String answer =  new EvalVisitor().visit(tree);
			System.out.println(answer);
			
			opt.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
