
#include "parserInterp.h"
#include <map>
#include <string>
#include <queue>
#include <iostream>
#include <fstream>

using namespace std;

extern map<Token, string> tokenPrint;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;
    bool executeStatements = true; // New flag to control execution
    
	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

// Function prototypes
bool TypeCompatible(Token varType, ValType valType);
Value DefaultValue(Token varType);

static int error_count = 0;

bool SkipStmt(istream& in, int& line) {
    bool previousExecutionState = Parser::executeStatements;
    Parser::executeStatements = false; // Disable execution
    bool status = Stmt(in, line);      // Parse the statement without executing
    Parser::executeStatements = previousExecutionState; // Restore execution state
    return status;
}

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

//07. PrintStmt ::= PRINT (ExprList)
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);
// 	// Debugging Statement
// 	cout << "Debug: PrintStmt - Received token " << t << " on line " << line << endl;
	
	if( t != LPAREN ) {
		ParseError(line, "Missing Left Parenthesis");
		delete ValQue;
		ValQue = nullptr;
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list after Print");
		while (!(*ValQue).empty()) {
			ValQue->pop();
		}
		delete ValQue;
		ValQue = nullptr;
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		ParseError(line, "Missing Right Parenthesis");
		while (!(*ValQue).empty()){
			ValQue->pop();
		}
		delete ValQue;
		ValQue = nullptr;
		return false;
	}
	
	// **Check if execution is enabled before printing**
    if (Parser::executeStatements) {
	//Evaluate: print out the list of expressions' values
    	while (!(*ValQue).empty()){
    		Value nextVal = (*ValQue).front();
    		cout << nextVal << ""; // Add space for readability
    		ValQue->pop();
    	}
    	cout << "\n(DONE)" << endl;
    }
    // Clean up the queue regardless of execution
    while (!ValQue->empty()){
        ValQue->pop();
    }
	delete ValQue;
	ValQue = nullptr;
	return true;
}//End of PrintStmt


//12. ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	
	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	if (ValQue != nullptr) {
        ValQue->push(retVal);
    }
    
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	}
	
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList


// Prog ::= PROGRAM IDENT CompStmt
bool Prog(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != PROGRAM) {
        ParseError(line, "Missing PROGRAM keyword");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok != IDENT) {
        ParseError(line, "Missing program name");
        return false;
    }

    if (!CompStmt(in, line)) {
        ParseError(line, "Invalid Program");
        return false;
    }

   
    return true;
}



// Expr, LogANDExpr, EqualExpr, etc., are implemented similarly
// Expr ::= LogANDExpr { '||' LogANDExpr }
bool Expr(istream& in, int& line, Value& retVal) {
    Value val1;
    if (!LogANDExpr(in, line, val1)) {
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    while (tok == OR) {
        Value val2;
        if (!LogANDExpr(in, line, val2)) {
            ParseError(line, "Missing operand after '||'");
            return false;
        }

        // Perform logical OR
        try {
            val1 = val1 || val2;
        } catch (runtime_error& e) {
            ParseError(line, e.what());
            return false;
        }

        tok = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(tok);

    retVal = val1;
    return true;
}

// CompStmt ::= '{' StmtList '}'
bool CompStmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != LBRACE) {
        ParseError(line, "Missing '{' at the beginning of compound statement");
        return false;
    }

    bool status = StmtList(in, line);

    if (!status) {
        ParseError(line, "Incorrect statement list");
    }

    tok = Parser::GetNextToken(in, line);
    if (tok != RBRACE) {
        ParseError(line, "Missing '}' at the end of compound statement");
        status = false;
    }

    return status;
}


//02. StmtList ::= Stmt { Stmt }
bool StmtList(istream& in, int& line) {
    bool status = true;
    while (true) {
        if (!Stmt(in, line)) {
            // Specific error already reported by Stmt
            // Attempt to recover by skipping tokens up to ';' or '}'
            LexItem tok;
            while ((tok = Parser::GetNextToken(in, line)) != SEMICOL && tok != RBRACE && tok != DONE) {
                // Skip tokens until ';', '}', or end of input
            }
            if (tok == SEMICOL) {
                // Continue parsing after ';'
                status = false;
                continue;
            } else if (tok == RBRACE || tok == DONE) {
                // End of compound statement or input
                Parser::PushBackToken(tok);
                status = false; // set status to false to error. 
                return status;
            }
            if (!Parser::executeStatements) {
                // Halt further execution
                return false;
            }
        }

        LexItem tok = Parser::GetNextToken(in, line);
        if (tok == RBRACE) {
            Parser::PushBackToken(tok);
            return status;
        }
        Parser::PushBackToken(tok);
    }
}


//03. Stmt ::= DeclStmt | ControlStmt | CompStmt
bool Stmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    Parser::PushBackToken(tok);

    if (tok == INT || tok == FLOAT || tok == BOOL || tok == CHAR || tok == STRING) {
        if (!DeclStmt(in, line)) {
            // Error occurred; tokens consumed in DeclStmt
            return false;
        }
    } else if (tok == IF || tok == PRINT || tok.GetToken() == IDENT) {
        if (!ControlStmt(in, line)) {
            // Error occurred; tokens consumed in ControlStmt
            ParseError(line, "Invalid control statement.");
            return false;
        }
    } else if (tok == LBRACE) {
        if (!CompStmt(in, line)) {
            // Error occurred; tokens consumed in CompStmt
            return false;
        }
    } else {
        ParseError(line, "Invalid statement");
        // Consume the invalid token to avoid infinite loop
        Parser::GetNextToken(in, line);
        return false;
    }
    
    // Execute the statement if allowed
    if (Parser::executeStatements) {
        // Statements are already executed in their respective functions
    }
    
    return true;
}


// Function prototypes for helper functions
bool DeclStmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    //Token type = tok.GetToken();

    // Pass the type token in idtok
    LexItem idtok = tok;

    if (!VarList(in, line, idtok)) {
        ParseError(line, "Invalid variable list in declaration");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok != SEMICOL) {
        ParseError(line, "Missing semicolon at the end of declaration");
        return false;
    }

    return true;
}

//05. VarList ::= Var [= Expr] { ,Var [= Expr] }
bool VarList(istream& in, int& line, LexItem& idtok) {
    LexItem varTok;
    if (!Var(in, line, varTok)) {
        ParseError(line, "Missing variable name");
        return false;
    }

    string varName = varTok.GetLexeme();
    Token type = idtok.GetToken();

     //Debugging Statement
     //cout << "Debug Parser: Declaring variable '" << varName << "' with type "  << " on line " << line + 1 << endl;

    // Check for variable redefinition
    if (SymTable.find(varName) != SymTable.end()) {
        ParseError(line, "Variable redefinition");
        return false;
    }

    // Add variable to symbol table
    SymTable[varName] = type;
    defVar[varName] = false;

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == ASSOP) {
        // Handle initialization
        Value exprVal;
        if (!Expr(in, line, exprVal)) {
            ParseError(line, "Invalid initialization expression");
            return false;
        }
        
        // Type checking
        if (!TypeCompatible(type, exprVal.GetType())) {
            ParseError(line, "Type mismatch in initialization");
            return false;
        }
        // **Type Conversion: Convert VINT to VREAL if necessary**
        if (type == FLOAT && exprVal.GetType() == VINT) {
            exprVal = Value(static_cast<double>(exprVal.GetInt()));
        }

        // Store the value
        TempsResults[varName] = exprVal;
        defVar[varName] = true;
        // // Debugging Statement
        // cout << "Debug Parser: Variable '" << varName << "' initialized with value " << exprVal << " on line " << line + 1 << endl;

    } else {
        Parser::PushBackToken(tok);
    }

    // Check for comma to parse more variables
    tok = Parser::GetNextToken(in, line);
    if (tok == COMMA) {
        // // Debugging Statement
        // cout << "Debug Parser: Found ',' in variable list on line " << line + 1 << endl;
        
        // Continue parsing variable list
        if (!VarList(in, line, idtok)) {
            return false;
        }
    } else {
        Parser::PushBackToken(tok);
    }

    return true;
}


//06. ControlStmt ::= AssignStmt ';' | IfStmt | PrintStmt ';'
bool ControlStmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == IF) {
        if (!IfStmt(in, line)) {
            // Error already reported in IfStmt
            return false;
        }
    } else if (tok == PRINT) {
        if (!PrintStmt(in, line)) {
            ParseError(line, "Invalid print statement");
            return false;
        }
        tok = Parser::GetNextToken(in, line);
        if (tok != SEMICOL) {
            ParseError(line, "Missing semicolon at the end of print statement");
            return false;
        }
    } else if (tok == IDENT) {
        // Push back the IDENT token so AssignStmt can read it
        Parser::PushBackToken(tok);
        
        if (!AssignStmt(in, line)) {
            // AssignStmt already reported the error
            return false;
        }
        
        // After AssignStmt, expect a semicolon
        LexItem semicolonTok = Parser::GetNextToken(in, line);
        //tok = Parser::GetNextToken(in, line);
        if (semicolonTok != SEMICOL){
            ParseError(line, "Missing semicolon at the end of assignment");
            return false;
        }
    } else {
        ParseError(line, "Invalid control statement.");
        // Consume the invalid token
        Parser::GetNextToken(in, line);
        return false;
    }
    return true;
}



// Var ::= IDENT
bool Var(istream& in, int& line, LexItem& idtok) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT) {
        idtok = tok;
        return true;
    } else {
        ParseError(line, "Missing variable name");
        return false;
    }
}

Value DefaultValue(Token varType) {
    switch (varType) {
        case INT:
            return Value(0);
        case FLOAT:
            return Value(0.0);
        case CHAR:
            return Value('\0');
        case STRING:
            return Value("");
        case BOOL:
            return Value(false);
        default:
            return Value(); // VERR type
    }
}


// Helper function to check type compatibility
bool TypeCompatible(Token varType, ValType valType) {
    if( valType == VERR ) {
        return false; // Explicitly disallow error types
    }
    if( varType == INT && (valType == VINT || valType == VCHAR) ) {
        return true;
    }
    if( varType == FLOAT && (valType == VREAL || valType == VINT || valType == VCHAR) ) {
        return true;
    }
    if( varType == CHAR && (valType == VCHAR || valType == VINT) ) { // Allow VINT for CHAR
        return true;
    }
    if( varType == STRING && valType == VSTRING ) {
        return true;
    }
    if( varType == BOOL && valType == VBOOL ) {
        return true;
    }
    return false;
}


// LogANDExpr ::= EqualExpr { '&&' EqualExpr }
// LogANDExpr ::= EqualExpr { '&&' EqualExpr }
bool LogANDExpr(istream& in, int& line, Value& retVal) {
    Value val1;
    if (!EqualExpr(in, line, val1)) {
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    while (tok == AND) {
        Value val2;
        if (!EqualExpr(in, line, val2)) {
            ParseError(line, "Missing operand after '&&'");
            return false;
        }

        // Perform logical AND
        try {
            val1 = val1 && val2;
        } catch (runtime_error& e) {
            ParseError(line, e.what());
            return false;
        }

        tok = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(tok);

    retVal = val1;
    return true;
}


//09. IfStmt ::= IF (Expr) Stmt [ ELSE Stmt ]
bool IfStmt(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != LPAREN) {
        ParseError(line, "Missing '(' after IF");
        return false;
    }
    
    Value condition;
    if (!Expr(in, line, condition)) {
        ParseError(line, "Invalid condition in IF statement");
        return false;
    }
    
    tok = Parser::GetNextToken(in, line);
    if (tok != RPAREN) {
        ParseError(line, "Missing ')' after IF condition");
        return false;
    }
    
    // Check if condition is boolean
    if (!condition.IsBool()) {
        ParseError(line, "IF condition must be boolean");
        return false;
    }
    
    // Determine which branch to execute
    if (condition.GetBool()) {
        // Execute the IF branch
        if (!Stmt(in, line)) {
            ParseError(line, "Invalid statement in IF branch");
            return false;
        }
        
        // Check for ELSE clause and skip it if present
        LexItem elseTok = Parser::GetNextToken(in, line);
        if (elseTok == ELSE) {
            if (!SkipStmt(in, line)) {
                ParseError(line, "Invalid statement in ELSE branch");
                return false;
            }
        } else {
            Parser::PushBackToken(elseTok);
        }
    } else {
        // Skip the IF branch
        if (!SkipStmt(in, line)) {
            ParseError(line, "Invalid statement in IF branch");
            return false;
        }
        
        // Check for ELSE clause and execute it if present
        LexItem elseTok = Parser::GetNextToken(in, line);
        if (elseTok == ELSE) {
            if (!Stmt(in, line)) {
                ParseError(line, "Invalid statement in ELSE branch");
                return false;
            }
        } else {
            Parser::PushBackToken(elseTok);
        }
    }
    
    return true;
}


//10. AssgnStmt ::= Var ( = | += | -= | *= | /= | %= ) Expr
bool AssignStmt(istream& in, int& line) {
    LexItem idtok;
    
    // Retrieve the identifier token
    if (!Var(in, line, idtok)) {
        ParseError(line, "Expected identifier in assignment");
        return false;
    }
    
    string varName = idtok.GetLexeme();
    if (SymTable.find(varName) == SymTable.end()) {
        ParseError(line, "Undeclared variable");
        return false;
    }

    // Get the assignment operator
    LexItem tok = Parser::GetNextToken(in, line);
    Token op = tok.GetToken();
    

    // Check if the operator is a valid assignment operator
    if (op != ASSOP && op != ADDASSOP && op != SUBASSOP &&
        op != MULASSOP && op != DIVASSOP && op != REMASSOP) {
        ParseError(line, "Missing or invalid assignment operator");
        return false;
    }

    // Parse and evaluate the expression on the right-hand side
    Value exprVal;
    if (!Expr(in, line, exprVal)) {
        //ParseError(line, "Invalid Expression evaluation in Assignment Statement");
        ParseError(line, "Incorrect Assignment Statement");
        return false;
    }
    
    // Debugging Statement
    //cout << "Debug: AssignStmt - Evaluated expression for '" << varName << "' = " << exprVal << " on line " << line + 1 << endl;


    // Retrieve the variable's type from the symbol table
    Token varType = SymTable[varName];

    // Perform type checking
    if (!TypeCompatible(varType, exprVal.GetType())) {
        ParseError(line, "Type mismatch in assignment");
        return false;
    }
    
    // **Check if using combined assignment operators and variable is initialized**
    if (op != ASSOP && !defVar[varName]) {
        ParseError(line, "Invalid Expression evaluation in Assignment Statement");
        Parser::executeStatements = false; // Halt execution
        return false;
    }


    // Check executeStatements flag
    if (!Parser::executeStatements) {
        // Skipping execution
        //cout << "Debug: AssignStmt - Skipping assignment for '" << varName << "' on line " << line + 1 << endl;
        return true;
    }

    // Get the current value of the variable, or default if uninitialized
    Value varVal;
    if (defVar[varName]) {
        varVal = TempsResults[varName];
    } else {
        varVal = DefaultValue(varType);
    }

    // Perform the assignment operation
    Value result;
    try {
        switch (op) {
            case ASSOP: // '='
                result = exprVal;
                break;
            case ADDASSOP: // '+='
                result = varVal + exprVal;
                break;
            case SUBASSOP: // '-='
                result = varVal - exprVal;
                break;
            case MULASSOP: // '*='
                result = varVal * exprVal;
                break;
            case DIVASSOP: // '/='
                result = varVal / exprVal;
                break;
            case REMASSOP: // '%='
                result = varVal % exprVal;
                break;
            default:
                ParseError(line, "Invalid assignment operator");
                return false;
        }
    } catch (runtime_error& e) {
        // Handle runtime errors from Value class operations
        ParseError(line, string("Runtime Error: ") + e.what());
        // Synchronize by consuming tokens up to ';'
        LexItem temp;
        while ((temp = Parser::GetNextToken(in, line)) != SEMICOL && temp != DONE) {
            // Skip tokens until ';' or EOF
        }
        return false;
    }
    
    // Debugging Statement
    //cout << "Debug: AssignStmt - Assigned '" << varName << "' = " << result << " on line " << line + 1 << endl;

    // Update the variable's value and mark it as initialized
    TempsResults[varName] = result;
    defVar[varName] = true;

    return true;
}


// EqualExpr ::= RelExpr [ ( '==' | '!=' ) RelExpr ]
bool EqualExpr(istream& in, int& line, Value& retVal) {
    Value val1;
    if (!RelExpr(in, line, val1)) {
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == EQ || tok == NEQ) {
        Token op = tok.GetToken();
        Value val2;
        if (!RelExpr(in, line, val2)) {
            ParseError(line, "Missing operand after equality operator");
            return false;
        }

        // Perform equality comparison
        try {
            if (op == EQ) {
                val1 = val1 == val2;
            } else { // NEQ
                val1 = val1 != val2;
            }
        } catch (runtime_error& e) {
            ParseError(line, e.what());
            return false;
        }
    } else {
        Parser::PushBackToken(tok);
    }

    retVal = val1;
    return true;
}


// RelExpr ::= AddExpr [ ( '>' | '<' ) AddExpr ]
bool RelExpr(istream& in, int& line, Value& retVal) {
    Value val1;
    if (!AddExpr(in, line, val1)) {
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == GTHAN || tok == LTHAN) {
        Token op = tok.GetToken();
        Value val2;
        if (!AddExpr(in, line, val2)) {
            ParseError(line, "Missing operand after relational operator");
            return false;
        }

        // Perform relational comparison
        try {
            if (op == GTHAN) {
                retVal = val1 > val2;
            } else { // LTHAN
                retVal = val1 < val2;
            }
        } catch (runtime_error& e) {
            ParseError(line, e.what());
            return false;
        }
    } else {
        Parser::PushBackToken(tok);
        retVal = val1;
    }
    return true;
}

//17. AddExpr ::= MultExpr { ( '+' | '-' ) MultExpr }
bool AddExpr(istream& in, int& line, Value& retVal) {
    Value val1;
    if (!MultExpr(in, line, val1)) {
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    while (tok == PLUS || tok == MINUS) {
        Token op = tok.GetToken();
        Value val2;
        if (!MultExpr(in, line, val2)) {
            ParseError(line, "Missing operand after additive operator");
            return false;
        }

        // Perform addition or subtraction
        try {
            if (op == PLUS) {
                val1 = val1 + val2;
            } else { // MINUS
                val1 = val1 - val2;
            }
        } catch (runtime_error& e) {
            ParseError(line, e.what());
            return false;
        }

        tok = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(tok);
    retVal = val1;
    return true;
}

//18. MultExpr ::= UnaryExpr { ( * | / | % ) UnaryExpr }
bool MultExpr(istream& in, int& line, Value& retVal) {
    Value val1;
    if (!UnaryExpr(in, line, val1)) {
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    while (tok == MULT || tok == DIV || tok == REM) {
        Token op = tok.GetToken();
        Value val2;
        if (!UnaryExpr(in, line, val2)) {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        // Perform multiplication, division, or remainder
        try {
            if (op == MULT) {
                val1 = val1 * val2;
            } else if (op == DIV) {
                val1 = val1 / val2;
            } else { // REM
                val1 = val1 % val2;
            }
        } catch (runtime_error& e) {
            ParseError(line, e.what());
            return false;
        }

        tok = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(tok);
    retVal = val1;
    return true;
}

// UnaryExpr ::= [ '-' | '!' ] PrimaryExpr
bool UnaryExpr(istream& in, int& line, Value& retVal) {
    LexItem tok = Parser::GetNextToken(in, line);
    int sign = 1; // Default sign is positive
    bool logicalNot = false;

    if (tok == MINUS) {
        sign = -1; // Unary minus
    } else if (tok == NOT) {
        logicalNot = true; // Logical NOT operator
    } else {
        Parser::PushBackToken(tok);
    }

    // Call PrimaryExpr with the sign parameter
    if (!PrimaryExpr(in, line, sign, retVal)) {
        return false;
    }

    // Apply logical NOT if necessary
    if (logicalNot) {
        try {
            retVal = !retVal;
        } catch (runtime_error& e) {
            ParseError(line, e.what());
            return false;
        }
    }

    return true;
}

// PrimaryExpr ::= IDENT | ICONST | RCONST | SCONST | BCONST | CCONST | '(' Expr ')'
bool PrimaryExpr(istream& in, int& line, int sign, Value& retVal) {
    LexItem tok = Parser::GetNextToken(in, line);

    switch (tok.GetToken()) {
        case IDENT: {
            string varName = tok.GetLexeme();
            if (SymTable.find(varName) == SymTable.end()) {
                ParseError(line, "Undeclared variable");
                return false;
            }
            if (!defVar[varName]) {
                ParseError(line, "Using uninitialized Variable");
                return false;
            }
            retVal = TempsResults[varName];

            // Apply sign if variable is numeric
            if (sign == -1) {
                if (retVal.IsInt()) {
                    retVal = Value(-retVal.GetInt());
                } else if (retVal.IsReal()) {
                    retVal = Value(-retVal.GetReal());
                } else {
                    ParseError(line, "Invalid use of '-' with non-numeric variable");
                    return false;
                }
            }
            break;
        }
        case ICONST:
            retVal = Value(sign * stoi(tok.GetLexeme()));
            break;
        case RCONST:
            retVal = Value(sign * stod(tok.GetLexeme()));
            break;
        case SCONST:
            if (sign == -1) {
                ParseError(line, "Invalid use of '-' with string constant");
                return false;
            }
            retVal = Value(tok.GetLexeme());
            break;
        case BCONST:
            if (sign == -1) {
                ParseError(line, "Invalid use of '-' with boolean constant");
                return false;
            }
            retVal = Value(tok.GetLexeme() == "TRUE");
            break;
        case CCONST:
            if (sign == -1) {
                ParseError(line, "Invalid use of '-' with character constant");
                return false;
            }
            retVal = Value(tok.GetLexeme()[0]);
            break;
        case LPAREN: {
            if (!Expr(in, line, retVal)) {
                ParseError(line, "Missing expression after '('");
                return false;
            }
            tok = Parser::GetNextToken(in, line);
            if (tok != RPAREN) {
                ParseError(line, "Missing ')' after expression");
                return false;
            }

            // Apply sign to the result of the expression
            if (sign == -1) {
                try {
                    if (retVal.IsInt()) {
                        retVal = Value(-retVal.GetInt());
                    } else if (retVal.IsReal()) {
                        retVal = Value(-retVal.GetReal());
                    } else {
                        ParseError(line, "Invalid use of '-' with non-numeric expression");
                        return false;
                    }
                } catch (runtime_error& e) {
                    ParseError(line, e.what());
                    return false;
                }
            }
            break;
        }
        default:
            ParseError(line, "Invalid primary expression");
            return false;
    }

    return true;
}



