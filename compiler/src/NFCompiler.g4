/**
 * Define a grammar called Hello
 */
grammar NFCompiler;
options{
    language=Java;
}

@header{

	package nflanguage;

}
/*
r  : 'hello' ID ;         // match keyword hello followed by an identifier

ID : [a-z]+ ;             // match lower-case identifiers

WS : [ \t\r\n]+ -> skip ; // skip spaces, tabs, newlines
 */

program  :
	START  IDENT LCB
	declaration
	entries
	RCB
;
entries  :
	( entry )+
;

entry  :
	ENTRY LCB match_action RCB
;

match_action  :
	( match_flow )? 
	( match_state )?
	action_statements
;

action_statements  :
	( action_flow | action_state )+
;

match_flow	:
	MATCH_FLOW LCB condition RCB
;

match_state  :
	MATCH_STATE LCB condition RCB
;

action_flow  :
 	ACTION_FLOW LCB statement RCB
;

action_state  :
	ACTION_STATE LCB statement RCB
;

declaration :
	( declare_entry )*
;

declare_entry  :
    type IDENT (LB granu=expression RB)? ( ASSIGN value=expression )? SEMICOLON
;

type  :   BASIC_TYPE	#dec_basic
        | SET LT BASIC_TYPE BT	#dec_set
        | MAP LT type COMMA type BT	#dec_map
        //| FLOW	#dec_flow
        | RULE #dec_rule
;

condition  :
	expression
;

statement  :
    (assignment| function_call | PASS) SEMICOLON ( statement )*
;

assignment  :
	  (BASIC_TYPE)? expression ASSIGN expression
;

expression  :
	  item	#atom
	| op expression  #single
	| left=expression op right=expression  #double //1+(1*0)
;

item  :
      IDENT ( (LSB fields RSB)? | (LSB expression RSB)* ) #id   // f[1]
    | function_call  #func
	| flow_or_rule_entry  #rule_flow
	| constant  #const
	| LCB contents RCB  #set  // set
	| LCB key_pairs RCB  #map
	| NULL # null
	| LB expression RB #lrexp
	| ACTION  #action
	| fields   #field
;

contents  :
	item (COMMA item)*
;

key_pairs  :
	key_value (COMMA key_value)*
;

key_value  :
	constant COLON constant
;

function_call  :
    funcs LB ( expression_list )? RB
;

funcs  :
	TIMER | RESUBMIT | ACCEPT | SEND_TO | SEND_BACK | CONNECT | MAX | RECV | ENCRYPT
;

expression_list  :
    expression (COMMA expression )*
;

constant  :
	IP  #const_ip
	| INT  #const_int
	| STR #const_str
;

flow_or_rule_entry  :
	fields COLON constant
;

op   :
    expr_op | rel_op
;

expr_op  :
	ADD | SUB | MUL | DIV | MOD | OR | AND | NOT | UNION | INTERSECTION | DIFFER
	// 	operators '||' | '&&' | '\\' designed for TYPE sets
;

rel_op  :
	EQUAL | UNEQUAL | LOE | BOE | LT | BT | IN | SUBSET | MATCH | COLON | CONNECTION | DATA
;

fields  :
	FIELD
;

BASIC_TYPE  :'int' | 'IP';

SET  : 'set' ;
MAP  : 'map' ;
FLOW : 'flow' ;
RULE : 'rule' ;

FIELD : 'sip' | 'dip' | 'UDP'
	| 'sport' | 'dport' | 'payload' | 'payload_length' | 'flag_syn' | 'flag_fin' | 'flag_ack'
	| 'tag' | 'iplen' ;
ACTION: 'DROP' ;
ENTRY : 'entry' ;
PASS  : 'pass' ;
TIMER : 'timer' ;
RESUBMIT : 'resubmit' ;
ACCEPT : 'accept' ;
SEND_TO : 'send_to';
SEND_BACK : 'send_back';
CONNECT : 'connect';
MAX : 'max' ;
RECV  : 'recv' ;
ENCRYPT : 'encrypt';

ADD   : '+' ;
SUB   : '-' ;
MUL   : '*' ;
DIV   : '/' ;
MOD   : '%' ;
AND   : '&&' ;  //And rules
OR    : '||' ;  //OR rules
UNION : '|' ;	//Union sets
INTERSECTION : '&' ;  //Intersection sets or intersection fields
DIFFER: '\\' ;  //  A\B , difference, objects that belong to A and not to B
NOT   : '~' | '!' ;   //Negation
EQUAL : '==' ;
UNEQUAL:'!=' ;
LOE   : '<=' ;  //Less Or Equal
BOE   : '>=' ;  //Bigger Or Equal
LT    : '<' ;   //Less Than
BT    : '>' ;   //Bigger Than
IN    : 'in' ;  //element of a set
SUBSET: 'subset of' ;  //a set is a subset of another
MATCH : 'matches' | 'mismatches' ;
CONNECTION : 'connection' ;
DATA  : 'data' ;
COMMA : ',' ;
LB    :	'(' ;  //left bracket
RB    : ')' ;  //right bracket
LCB   : '{' ;  //left curly bracket == left brace
RCB   : '}' ;  //right curly bracket == right brace
LSB   : '[' ;  //left square bracket
RSB   : ']' ;  //right square bracket
COLON : ':' ;
START : 'program' ;
NULL  : 'null' ;
ASSIGN: '=' ;

SEMICOLON :	';' ;
MATCH_FLOW   : 'match_flow' ;
MATCH_STATE  : 'match_state' ;
ACTION_FLOW  : 'action_flow' ;
ACTION_STATE : 'action_state' ;

IP  :
    [0-9]+ '.' [0-9]+ '.' [0-9]+ '.' [0-9]+
        ( '/'[0-9]+ //255:255:255:255/32
            | [0-9]+ '.' [0-9]+ '.' [0-9]+ '.' [0-9]+
         )?           //255:255:255:255/255:255:255:255
;

INT  :
	[0-9]+
;

IDENT  :
	[a-zA-Z] [a-zA-Z0-9_]*
;

STR  :
    '"' .* '"'
;
BLOCK_COMMENT
	: '/*' .*? '*/' -> channel(HIDDEN)
	;
LINE_COMMENT
	: '//' ~[\r\n]* -> channel(HIDDEN)
	;
WS : [ \t\b\r\n\f]+ -> channel(HIDDEN) ;
