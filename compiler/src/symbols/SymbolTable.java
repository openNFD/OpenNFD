package symbols;

import java.util.HashMap;


public class SymbolTable {
	private static HashMap<String, Symbol> table= new HashMap<String, Symbol>();
	
	public static boolean add(String name, String type, String subtype, String value, String granularity) {
		if (SymbolTable.table.containsKey(name)) {
			System.out.println("symbol already exists");
			return false;
		}else {
			SymbolTable.table.put(name, new Symbol(name, type, subtype, value, granularity));
			return true;
		}
	}
	public static Symbol getSymbolByName(String name) {
		if (! SymbolTable.table.containsKey(name)) {
			System.out.println("Can't find "+name+" inside symbol table");
			return null;
		}
		else {
			return SymbolTable.table.get(name);
		}
	}
	public static String getTypeByName(String name) {
		Symbol sym = SymbolTable.getSymbolByName(name);
		return sym.getType();
	}
	public static String getValueByName(String name) {
		Symbol sym= SymbolTable.getSymbolByName(name);
		return sym.getValue();
	}
	public static String getGranularityByName(String name) {
		Symbol sym= SymbolTable.getSymbolByName(name);
		return sym.getGranularity();
	}
	public static String getSubtypeByName(String name) {
		Symbol sym= SymbolTable.getSymbolByName(name);
		return sym.getSubtype();
	}
	public static String getProgramName() {
		for (Symbol sym : table.values()) {
			if (sym.getType().equals("program")) {
				return sym.getName();
			}
		}
		System.out.println("no program name in symbol table!");
		return null;
	}
}
