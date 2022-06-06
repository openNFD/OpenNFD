package log;

import java.util.ArrayList;

import org.antlr.v4.runtime.ParserRuleContext;

public class ErrorLogger{
	private ArrayList<String> err_info;
	public ErrorLogger() {
		this.err_info = new ArrayList<String>();
	}
	public void addLog(String log) {
		this.err_info.add(log);
	}
	public void printLog() {
		int counter = 0;
		for (String s: err_info) {
			System.out.println(++counter + s);
		}
	}
}
