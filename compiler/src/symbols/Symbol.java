package symbols;

public class Symbol {
	private String name;
	private String type;
	private String subtype;
	private String value;
	private String granularity;
	/*
	 * name: ident
	 * type :      subtype:
	 * 		program
	 * 		flow
	 * 		rule
	 * 		state
	 * 		var
	 * 				map
	 * 				set
	 * 				int
	 * 				IP
	 *value: 
	 *		int             for int-type state
	 *		IP              for IP-type state
	 *granularity:
	 *		per-flow
	 *		per-nf
	 * 
	 */
	
	public Symbol(String n, String t,String s, String v, String g) {
		this.name= n;
		this.type =t;
		this.subtype =s;
		this.value=v;
		this.granularity=g;
	}
	public String getName() {
		return name;
	}
	public String getType() {
		return type;
	}
	public String getValue() {
		return value;
	}
	public String getGranularity() {
		return granularity;
	}
	public String getSubtype() {
		return subtype;
	}
}
